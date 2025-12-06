#include <nbdl_spec/NbdlDialect.h>
#include <heavy/Context.h>
#include <heavy/Value.h>
#include <heavy/MlirHelper.h>
#include <llvm/Support/Casting.h>
#include <mlir/Dialect/Func/IR/FuncOps.h>
#include <memory>
#include <optional>
#include <tuple>

using Context = heavy::Context;
using ValueRefs = heavy::ValueRefs;
using CaptureList = heavy::CaptureList;
namespace mlir_helper = heavy::mlir_helper;
using llvm::cast;
using llvm::cast_or_null;
using llvm::dyn_cast;
using llvm::dyn_cast_or_null;
using llvm::isa;
using llvm::isa_and_nonnull;

namespace nbdl_spec {
extern std::tuple<std::string, heavy::SourceLocationEncoding*, mlir::Operation*>
translate_cpp(heavy::LexerWriterFnRef FnRef, mlir::Operation* Op);
}

extern "C" {
heavy::ContextLocal nbdl_current_module;
}

// Create a builder that appends to the nbdl module.
static std::optional<mlir::OpBuilder> getModuleBuilder(heavy::Context& C) {
  heavy::Value V = nbdl_current_module.get(C);
  mlir::Operation* Op = heavy::cast<mlir::Operation>(V);
  mlir::ModuleOp ModuleOp = heavy::dyn_cast_or_null<mlir::ModuleOp>(Op);
  if (!ModuleOp) {
    heavy::Error* E = C.CreateError(C.getLoc(),
        "invalid current-nbdl-module", heavy::Empty());
    C.Raise(E);
    return {};
  }
  mlir::OpBuilder Builder(ModuleOp);
  Builder.setInsertionPointToEnd(ModuleOp.getBody());
  return Builder;
}

extern "C" {
// Create a function and call the thunk with a new builder
// to insert into the function body.
// _num_store_params_ N
// _callback_ takes _num_store_params_ + 1 arguments which are the block arguments
//  with formals like (store1 store2 ... storeN fn)
// (%build_match_params _name_ _num_store_params_ _callback_)
void nbdl_spec_build_match_params(Context& C, ValueRefs Args) {
  if (Args.size() != 3)
    return C.RaiseError("invalid arity");

  std::optional<mlir::OpBuilder> BuilderOpt = getModuleBuilder(C);
  if (!BuilderOpt)
    return;
  mlir::OpBuilder Builder = BuilderOpt.value();

  heavy::SourceLocation Loc = Args[0].getSourceLocation();
  // Require a heavy::Symbol so it has a source location.
  heavy::Symbol* NameSym = dyn_cast<heavy::Symbol>(Args[0]);
  llvm::StringRef Name = NameSym->getStringRef();
  heavy::Value NumParamsVal = Args[1];
  heavy::Value Callback = Args[2];

  int32_t NumParams = isa<heavy::Int>(NumParamsVal)
      ? int32_t(cast<heavy::Int>(NumParamsVal))
      : int32_t(-1);

  if (NumParams < 0)
    return C.RaiseError("expecting positive integer for num_store_params");
  if (!NameSym || Name.empty())
    return C.RaiseError("expecting function name (symbol literal)");

  mlir::Location MLoc = mlir::OpaqueLoc::get(Loc.getOpaqueEncoding(),
                                             Builder.getContext());

  // Create the function type.
  llvm::SmallVector<mlir::Type, 8> InputTypes;
  for (unsigned i = 0; i < static_cast<uint32_t>(NumParams); i++)
    InputTypes.push_back(Builder.getType<nbdl_spec::StoreType>());

  // Push the visitor fn argument.
  InputTypes.push_back(Builder.getType<nbdl_spec::StoreType>());

  mlir::FunctionType FT = Builder.getFunctionType(InputTypes,
                                                  /*ResultTypes*/{});

  // Create the function.
  auto FuncOp = mlir::func::FuncOp::create(Builder, MLoc, Name, FT);
  FuncOp.addEntryBlock();

  heavy::Value Thunk = C.CreateLambda([FuncOp](Context& C, ValueRefs) mutable {
    heavy::Value Callback = C.getCapture(0);
    llvm::SmallVector<heavy::Value, 8> BlockArgs;
    assert(!FuncOp.getBody().empty() && "should have entry block");
    for (mlir::Value MVal : FuncOp.getBody().getArguments()) {
      heavy::Value V = C.CreateAny(MVal);
      BlockArgs.push_back(V);
    }

    C.PushCont([FuncOp](Context& C, ValueRefs) mutable {
      C.Cont(FuncOp.getOperation());
    }, CaptureList{});
    C.Apply(Callback, BlockArgs);
  }, CaptureList{Callback});

  // Call the thunk with a Builder at the entry point.
  Builder = mlir::OpBuilder(FuncOp.getBody());
  mlir_helper::with_builder_impl(C, Builder, Thunk);
}

// Translate a nbdl dialect operation to C++.
// (translate-cpp op port)
// The parameter `op` may be an mlir::Operation* or a StringLike
// which will be used to look up the name in the module.
// Currently the "port" has to be a tagged llvm::raw_ostream.
void nbdl_spec_translate_cpp(Context& C, ValueRefs Args) {
  if (Args.size() != 2 && Args.size() != 1)
    return C.RaiseError("invalid arity");
  auto* Op = dyn_cast<mlir::Operation>(Args[0]);
  if (!Op)
    return C.RaiseError("expecting mlir.operation: {}", Args[0]);

  llvm::raw_ostream* OS = nullptr;

  using ResultTy = std::tuple<std::string,
                              heavy::SourceLocationEncoding*,
                              mlir::Operation*>;
  auto Result = ResultTy();


  // Do not capture the emphemeral Any object.
  if (Args.size() == 2) {
    if (auto LWF = heavy::any_cast<heavy::LexerWriterFnRef>(Args[1])) {
      Result = nbdl_spec::translate_cpp(LWF, Op);
    } else if (auto* Raw = heavy::any_cast<::llvm::raw_ostream>(&Args[1])) {
      OS = Raw;
    } else {
      return C.RaiseError("expecting llvm::raw_ostream"
                          " or heavy::LexerWriterFnRef");
    }
  } else {
    OS = &llvm::outs();
  }
  if (OS) {
    auto LexerWriter = [&OS](heavy::SourceLocation, llvm::StringRef Buffer) {
      *OS << Buffer;
    };
    Result = nbdl_spec::translate_cpp(LexerWriter, Op);
  }

  auto& [ErrMsg, ErrLoc, Irritant] = Result;
  if (!ErrMsg.empty()) {
    heavy::SourceLocation Loc(ErrLoc);
    heavy::Error* Err = C.CreateError(Loc, ErrMsg,
        Irritant ? heavy::Value(Irritant) : heavy::Value(heavy::Undefined()));
    return C.Raise(Err);
  }
  C.Cont();
}

// If the current block has a terminator, wrap the
// entire block in a nbdl.scope. This supports the
// convention that only terminators may perform an
// operation that may invalidate child stores.
void nbdl_spec_close_previous_scope(Context& C, ValueRefs Args) {
  if (Args.size() != 0)
    return C.RaiseError("invalid arity");
  mlir::OpBuilder* Builder = mlir_helper::getCurrentBuilder(C);
  if (!Builder)
    return;  // error is already raised by getCurrentBuilder
  mlir::Block* Block = Builder->getBlock();
  assert(Block && isa<mlir::func::FuncOp>(Block->getParentOp())
      && "expecting func insertion point");
  if (Block->empty() || !Block->back().hasTrait<mlir::OpTrait::IsTerminator>())
    return C.Cont();

  mlir::Location Loc = Block->back().getLoc();

  // Create new Region for ScopeOp.
  auto ScopeBody = std::make_unique<mlir::Region>();
  mlir::Block& NewBlock = ScopeBody->emplaceBlock();
  while (!Block->empty())
    Block->front().moveBefore(&NewBlock, NewBlock.end());
  mlir::Operation* ScopeOp
    = nbdl_spec::ScopeOp::create(*Builder, Loc, std::move(ScopeBody));
  Builder->setInsertionPointAfter(ScopeOp);

  C.Cont();
}

// Initialize the nbdl_spec mlir module.
void nbdl_spec_module_init(heavy::Context& C, heavy::ValueRefs) {
  C.DialectRegistry->insert<nbdl_spec::NbdlDialect>();

  // Assume that the MLIRContext cleans up ModuleOps.
  mlir::OpBuilder Builder(C.MLIRContext.get());
  mlir::Location Loc = Builder.getUnknownLoc();
  mlir::ModuleOp ModuleOp
    = mlir::ModuleOp::create(Builder, Loc, "nbdl_spec_module");
  nbdl_current_module.set(C, ModuleOp.getOperation());
  C.Cont();
}
} //  extern "C"
