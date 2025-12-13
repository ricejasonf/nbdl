// Copyright Jason Rice 2025
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// Define functions and classes for transpiling Nbdl dialect operations to
// C++ which tries to maintain source locations to declarations in DSL code.
// 
#include <nbdl_spec/NbdlDialect.h>
#include <heavy/Source.h>
#include <heavy/Value.h>
#include <llvm/ADT/ScopedHashTable.h>
#include <llvm/ADT/ScopeExit.h>
#include <llvm/ADT/Twine.h>
#include <llvm/ADT/TypeSwitch.h>
#include <llvm/Support/Casting.h>
#include <mlir/IR/Value.h>
#include <mlir/IR/Operation.h>
#include <mlir/Dialect/Func/IR/FuncOps.h>
#include <memory>
#include <string>
#include <tuple>

namespace heavy {
class SourceLocationEncoding;
}

namespace {
using namespace nbdl_spec;
using llvm::isa;
using llvm::cast;
using llvm::dyn_cast;

template <typename Derived>
class NbdlSpecWriter {
public:
  // Map mlir values to names within a function scope.
  using ValueMapTy = llvm::ScopedHashTable<mlir::Value, llvm::StringRef>;
  using ValueMapScope = typename ValueMapTy::ScopeTy;

  ValueMapTy ValueMap;
  llvm::BumpPtrAllocator StringHeap;
  // Irritant may be nullptr even with error present.
  mlir::Operation* Irritant = nullptr;
  std::string ErrMsg;
  heavy::SourceLocationEncoding* ErrLoc;
  heavy::SourceLocation CurLoc;

  heavy::LexerWriterFnRef LexerWriter;
  std::string OutputBuffer;
  llvm::raw_string_ostream OS;

  // Track number of members of context
  // to generate anonymous identifiers if needed.
  unsigned CurrentMemberCount = 0;
  unsigned CurrentAnonVarCount = 0;
  NbdlSpecWriter(heavy::LexerWriterFnRef LexerWriter)
    : LexerWriter(LexerWriter),
      OS(OutputBuffer)
  { }

  class LocRAII {
    NbdlSpecWriter& NW;
    heavy::SourceLocation PrevLoc;

  public:
    LocRAII(NbdlSpecWriter& NW, mlir::Location NewLoc)
      : NW(NW),
        PrevLoc(NW.CurLoc)
    {
      NW.SetLoc(NewLoc);
    }

    LocRAII(NbdlSpecWriter& NW, mlir::Value V)
      : NW(NW),
        PrevLoc(NW.CurLoc)
    {
      NW.SetLocFromValue(V);
    }

    ~LocRAII() {
      NW.CurLoc = PrevLoc;
      NW.Flush();
    }
  };

  LocRAII MakeLocRAII(mlir::Location MLoc) {
    return LocRAII(*this, MLoc);
  }

  LocRAII MakeLocRAII(mlir::Value V) {
    return LocRAII(*this, V);
  }

  void SetLoc(mlir::Location MLoc) {
    auto Loc = heavy::SourceLocation(mlir::OpaqueLoc
      ::getUnderlyingLocationOrNull<heavy::SourceLocationEncoding*>(MLoc));
    assert(Loc.isValid() && "expecting valid source location");
    CurLoc = Loc;
  }

  void SetLocFromValue(mlir::Value V) {
    mlir::Operation* Op = nullptr;
    if (mlir::Operation* DefOp = V.getDefiningOp()) {
      Op = DefOp;
    } else if (auto BlockArg = dyn_cast<mlir::BlockArgument>(V)) {
      if (mlir::Block* Block = BlockArg.getOwner())
        Op = Block->getParentOp();
    }
    assert(Op && "expecting value attached to operation");
    SetLoc(Op->getLoc());
  }

  void Flush() {
    if (OutputBuffer.empty())
      return;
    LexerWriter(CurLoc, llvm::StringRef(OutputBuffer));
    OutputBuffer.clear();
  }

  Derived& getDerived() {
    return static_cast<Derived&>(*this);
  }

  bool CheckError() {
    //  - Returns true if there is an error.
    return !ErrMsg.empty();
  }

  void SetError(mlir::Location Loc, llvm::StringRef Msg) {
    if (!ErrMsg.empty())
      return;

    ErrMsg = Msg.str();
    ErrLoc = mlir::OpaqueLoc
      ::getUnderlyingLocationOrNull<heavy::SourceLocationEncoding*>(Loc);
  }

  void SetError(llvm::StringRef Msg, mlir::Operation* Op) {
    if (!ErrMsg.empty())
      return;

    SetError(Op->getLoc(), Msg);
    Irritant = Op;
  }

  void SetErrorV(llvm::StringRef Msg, mlir::Value V) {
    mlir::Operation* Op = V.getDefiningOp();
    if (!Op) {
      mlir::Region* R = V.getParentRegion();
      if (R)
        Op = R->getParentOp();
      assert(Op && "mlir.value has no parent operation");
    }
    SetError(Msg, Op);
  }

  llvm::StringRef GetLocalVal(mlir::Value V) {
    // ScopedHashTable has a weird interface.
    llvm::StringRef Name = ValueMap.lookup(V);
    if (Name.empty()) {
      SetError(V.getLoc(), "Name not in ValueMap");
      return "ERROR";
    }
    return Name;
  }

  // Stream the value with the source location from
  // a corresponding operation.
  void PrintLocalVal(mlir::Value V) {
    auto LocScope = MakeLocRAII(V);
    SetLocFromValue(V);
    OS << GetLocalVal(V);
  }

  llvm::StringRef SetLocalVal(mlir::Value V, llvm::Twine Twine) {
    llvm::SmallString<128> NameTemp;
    Twine.toVector(NameTemp);
    // Store the string to have a reliable llvm::StringRef.
    llvm::StringRef Name = NameTemp.str().copy(StringHeap);
    ValueMap.insert(V, Name);

    return Name;
  }

  // Use for variables names to suffix with a unique counter value.
  llvm::StringRef SetLocalVarName(mlir::Value V, llvm::StringRef Name) {
    return SetLocalVal(V,
      llvm::Twine(Name).concat(
          llvm::Twine(CurrentAnonVarCount++)));
  }

  llvm::StringRef SetLocalVarName(mlir::Value V) {
    // Create anonymous name for variable.
    return SetLocalVarName(V, "anon_");
  }

  /************************************
   *********** Expr Printing **********
   ************************************/

  void WriteExpr(mlir::Value V, bool IsFwd = false) {
    // We do not need to forward literals and junk.
    if (auto Op = V.getDefiningOp<LiteralOp>()) {
      WriteExpr(Op);
    } else if (auto Op = V.getDefiningOp<ConstexprOp>()) {
      WriteExpr(Op);
    } else if (auto Op = V.getDefiningOp<ConstOp>()) {
      WriteExpr(Op);
    } else {
      if (IsFwd) {
        OS << "static_cast<decltype(";
        PrintLocalVal(V);
        OS << ")>(";
        PrintLocalVal(V);
        OS << ")";
      } else {
        PrintLocalVal(V);
      }
    }
  }

  // Write the name expr possibly as a forwarded expr.
  void WriteNameExpr(mlir::Value V) {
    if (V.hasOneUse())
      return WriteExpr(V, /*IsFwd=*/true);

    // TODO
    // Check that V has exactly one use that is not borrowed
    // and that it appears last in... the block??
    WriteExpr(V, /*IsFwd=*/false);
  }

  // Special function to denote that we already checked for multiple uses.
  void WriteLastUseForwardedExpr(mlir::Value V) {
    WriteExpr(V, /*IsFwd=*/true);
  }

  void WriteExpr(ConstexprOp Op) {
    auto LocScope = MakeLocRAII(Op.getLoc());
    llvm::StringRef Expr = Op.getExpr();
    if (Expr.empty())
      SetError("expecting expr", Op);
    OS << Expr;
  }

  void WriteExpr(LiteralOp Op) {
    auto LocScope = MakeLocRAII(Op.getLoc());
    mlir::Attribute Attr = Op.getValue();
    if (auto IA = dyn_cast<mlir::IntegerAttr>(Attr);
        IA &&
        (IA.getType().isIndex() || IA.getType().isSignlessInteger())) {
      OS << IA.getInt();
    } else if (auto SA = dyn_cast<mlir::StringAttr>(Attr)) {
      OS << '"' << llvm::StringRef(SA) << '"';
    } else {
      SetError("unknown literal type", Op);
    }
  }

  void WriteExpr(ConstOp Op) {
    auto LocScope = MakeLocRAII(Op.getLoc());
    OS << "::std::as_const(";
    WriteExpr(Op.getArg());
    OS << ")";
  }

  /************************************
   *********** Type Printing **********
   ************************************/

  void VisitType(mlir::Value Val) {
    auto LocScope = MakeLocRAII(Val);
    if (mlir::Operation* Op = Val.getDefiningOp()) {
      if (isa<ContextOp>(Op))
        return VisitType(cast<ContextOp>(Op));
      else if (isa<StoreOp>(Op))
        return VisitType(cast<StoreOp>(Op));
      else if (isa<VariantOp>(Op))
        return VisitType(cast<VariantOp>(Op));
      else if (isa<StoreComposeOp>(Op))
        return VisitType(cast<StoreComposeOp>(Op));
      else if (isa<ConstexprOp>(Op))
        return VisitType(cast<ConstexprOp>(Op));
      else
        SetError("unhandled operation (VisitType): {}", Op);
    } else {
      // Handle mlir::BlockArgument.
      // Arguments cannot be `decltype(auto)`
      if (auto OpaqueType = dyn_cast<nbdl_spec::OpaqueType>(Val.getType())) {
        OS << "auto&&";
      } else {
        return SetErrorV("unsupported type for argument", Val);
      }
    }
  }

  void VisitType(ContextOp Op) {
    OS << Op.getSymName();
  }

  void VisitType(StoreOp Op) {
    OS << Op.getName();
  }

  void VisitType(VariantOp Op) {
    OS << "::nbdl::variant_holder<";
    llvm::interleaveComma(Op.getOperands(), OS,
        [&](mlir::Value const& Val) {
          mlir::Value V = Val;
          VisitType(V);
        });
    OS << ">";
  }

  void VisitType(StoreComposeOp Op) {
    OS << "::nbdl::store_composite<";
    assert(!Op.hasUnitKey() &&
      "unit key for store compose currently not supported");
    VisitTypeOrUnitType(Op.getKey());
    OS << ", ";
    VisitType(Op.getLhs());
    OS << ", ";
    VisitType(Op.getRhs());
    OS << ">";
  }

  void VisitType(ConstexprOp Op) {
    VisitType(Op.getLoc(), Op.getResult().getType());
  }

  void VisitTypeOrUnitType(mlir::Value V) {
    assert(V && isa<nbdl_spec::UnitType>(V.getType())
        && "printing unit type currently not supported");

    if (!V || !isa<nbdl_spec::UnitType>(V.getType()))
      VisitType(V);
  }

  void VisitType(mlir::Location, mlir::Type) {
    OS << "auto&&";
  }
};

class FuncWriter : public NbdlSpecWriter<FuncWriter> {
  public:
  using NbdlSpecWriter<FuncWriter>::NbdlSpecWriter;

  void Visit(mlir::Operation* Op) {
    Flush();
    if (CheckError()) return;
    auto LocScope = MakeLocRAII(Op->getLoc());

         if (isa<MatchOp>(Op))        return Visit(cast<MatchOp>(Op));
    else if (isa<OverloadOp>(Op))     return Visit(cast<OverloadOp>(Op));
    else if (isa<MemberNameOp>(Op))   return Visit(cast<MemberNameOp>(Op));
    else if (isa<GetOp>(Op))          return Visit(cast<GetOp>(Op));
    else if (isa<DiscardOp>(Op))      return Visit(cast<DiscardOp>(Op));
    else if (isa<VisitOp>(Op))        return Visit(cast<VisitOp>(Op));
    else if (isa<NoOp>(Op))           return Visit(cast<NoOp>(Op));
    else if (isa<MatchIfOp>(Op))      return Visit(cast<MatchIfOp>(Op));
    else if (isa<FuncOp>(Op))         return Visit(cast<FuncOp>(Op));
    else if (isa<StoreComposeOp>(Op)) return Visit(cast<StoreComposeOp>(Op));
    else if (isa<ScopeOp>(Op))
      return VisitRegion(cast<ScopeOp>(Op).getBody());
    else if (isa<ConstexprOp, LiteralOp>(Op)) return;
    else if (isa<UnitOp>(Op)) return;
    else if (isa<EmptyOp>(Op))
      return SetError("empty type does not map to c++ "
                      "because that would be absurd", Op);
    else
      return SetError("unhandled operation: {}", Op);
  }

  void VisitRegion(mlir::Region& R) {
    if (!R.hasOneBlock())
      return SetError("expecting a region with a single block",
                      R.getParentOp());
    for (mlir::Operation& Op : R.front())
      Visit(&Op);
  }

  void Visit(FuncOp Op) {
    mlir::FunctionType FT = Op.getFunctionType();
    llvm::StringRef Name = Op.getSymName();

    // Write the return type.
    if (FT.getNumResults() > 1)
      return SetError("Function should have less than 2 results.", Op);
    if (FT.getNumResults() == 0)
      OS << "void";
    else
      OS << "decltype(auto)";

    // Write the function name.
    OS << ' ' << Name;

    mlir::Region& Body = Op.getBody();
    if (Body.empty())
      return SetError("empty function body", Op);

    ValueMapScope Scope(ValueMap);
    // Write parameters.
    OS << '(';
    llvm::interleaveComma(Body.getArguments(), OS,
        [&](mlir::BlockArgument const& Arg) {
          OS << "auto&& " << SetLocalVarName(Arg, "arg_");
        });
    OS << ')';

    OS << "{\n";
    VisitRegion(Body);
    OS << '}';
  }

  void Visit(GetOp Op) {
    bool HasUnitKey = Op.hasUnitKey();
    nbdl_spec::MemberNameOp MemberNameOp;
    if (!HasUnitKey)
      MemberNameOp = Op.getKey().getDefiningOp<nbdl_spec::MemberNameOp>();

    OS << "auto&& "
       << SetLocalVarName(Op.getResult(), "get_")
       << " = ";
    if (MemberNameOp) {
      WriteExpr(Op.getState());
      OS << '.' << MemberNameOp.getName()
         << ";\n";
    } else {
      OS << "::nbdl::get(";
      WriteExpr(Op.getState());
      if (!HasUnitKey) {
        OS << ", ";
        WriteExpr(Op.getKey());
      }
      OS << ");\n";
    }
  }

  void Visit(VisitOp Op) {
    // If the return type is not the unit type assign
    // the result to a variable.
    if (!isa<nbdl_spec::UnitType>(Op.getResult().getType())) {
      OS << "auto&& "
         << SetLocalVarName(Op.getResult(), "result_")
         << " = ";
    }

    mlir::Value Fn = Op.getFn();
    mlir::OperandRange Args = Op.getArgs();

    if (auto MemberNameOp = Fn.getDefiningOp<nbdl_spec::MemberNameOp>()) {
      if (Args.empty()) {
        SetError("member literal callee expects at least one argument", Op);
        return;
      }
      WriteExpr(Args.front());
      OS << '.' << MemberNameOp.getName();
      Args = Args.drop_front();
    } else {
      WriteExpr(Op.getFn());
    }
    OS << '(';
    llvm::interleave(Op.getArgs(), OS,
        [&](mlir::Value V) {
          WriteExpr(V);
        }, ",\n");
    OS << ");\n";
  }

  void Visit(DiscardOp) {
    // Do nothing.
  }

  void Visit(StoreComposeOp Op) {
    // Note the C++ function does (Key, Rhs, Lhs) for some reason.
    OS << "auto ";
    OS << SetLocalVarName(Op.getResult(), "result_");
    OS << " = ::nbdl::store_compose(";
    if (!Op.hasUnitKey()) {
      WriteExpr(Op.getKey());
      OS << ",";
    }
    WriteExpr(Op.getRhs());
    OS << ",";
    WriteExpr(Op.getLhs());
    OS << ");\n";
  }

  void Visit(NoOp Op) {
    OS << "(void)";
    WriteExpr(Op.getArg());
    OS << ";\n";
  }

  void Visit(MatchOp Op) {
    OS << "::nbdl::match(";
    WriteExpr(Op.getStore());
    if (!Op.hasUnitKey()) {
      OS << ", ";
      WriteExpr(Op.getKey());
    }
    OS << ", ";

    auto& Ops = Op.getOverloads().front().getOperations();
    if (Ops.size() == 1) {
      Visit(const_cast<mlir::Operation*>(&Ops.front()));
    } else {
      OS << "boost::hana::overload(";
      llvm::interleave(Ops, OS,
          [&](mlir::Operation const& OverloadOp) {
            Visit(const_cast<mlir::Operation*>(&OverloadOp));
          }, ",\n");
      OS << ")";
    }

    OS << ");\n";
  }

  void Visit(OverloadOp Op) {
    mlir::Region& Body = Op.getBody();
    if (Body.empty())
      return;
    llvm::StringRef TypeStr = Op.getType();
    OS << "[&]";
    // Write parameters.
    OS << "([[maybe_unused]] ";
    mlir::BlockArgument& Arg = Body.getArguments().front();
    if (!TypeStr.empty())
      OS << "::nbdl::SameAs<" << TypeStr << "> ";
    OS << "auto&& "
       << SetLocalVarName(Arg, "arg_");
    OS << ')';
    OS << "{\n";
    VisitRegion(Body);
    OS << '}';
  }

  void Visit(MatchIfOp Op) {
    mlir::Region& Then = Op.getThenRegion();
    mlir::Region& Else = Op.getElseRegion();
    OS << "if (";
    WriteExpr(Op.getPred());
    OS << '(';
    WriteExpr(Op.getInput());
    OS << ")) {\n";
    VisitRegion(Then);

    // Check if the the else region is a single MatchIfOp
    // for pretty chaining.
    OS << "} else ";
    if (auto ChainedIfOp = dyn_cast<MatchIfOp>(Else.front().front())) {
      Visit(ChainedIfOp);
    } else {
      OS << "{\n";
      VisitRegion(Op.getElseRegion());
      OS << "}\n";
    }
  }

  void Visit(MemberNameOp) {
    // Member name is meaningless without the parent object
    // so we print it in GetOp.
    // We could implement in MatchOp, but it is a very
    // unlikely use case.
  }
};

class ContextWriter : public NbdlSpecWriter<ContextWriter> {
public:
  using NbdlSpecWriter<ContextWriter>::NbdlSpecWriter;

  void VisitContext(ContextOp Op) {
    SetLoc(Op.getLoc());

    llvm::StringRef ClassName = Op.getName();
    llvm::StringRef ImplName = Op.getImplName();

    OS << "class " << ClassName << " : nbdl::context_alias<";
    OS << ImplName;
    OS << ", /*is_moveable=*/false> {\n"
          "using Base = nbdl::context_alias<";
    OS << ImplName;
    OS << ">;\n using Base::Base;\n";
    OS << "};\n";
    Flush();
  }
};

class DefineStoreWriter : public NbdlSpecWriter<DefineStoreWriter> {
public:
  // Record the values for each member in order.
  llvm::SmallVector<mlir::Value, 8> Members;

  using NbdlSpecWriter<DefineStoreWriter>::NbdlSpecWriter;

  void VisitDefineStore(DefineStoreOp Op) {
    // Skip externally defined stores.
    if (Op.isExternal())
      return;

    SetLoc(Op.getLoc());
    ValueMapScope Scope(ValueMap);

    // Set the arg names first.
    for (mlir::BlockArgument BlockArg : Op.getBody().getArguments())
      SetLocalVarName(BlockArg, "arg_");

    auto ContOp = getContOp(Op);
    llvm::TypeSwitch<mlir::Operation*>(ContOp.getArg().getDefiningOp())
      .Case<UnitOp>([&, this](auto) {
          this->CreateTag(Op.getName());
        })
      .Case<StoreOp, VariantOp>([&, this](auto ResultOp) {
          this->CreateStrongAlias(Op.getName(), ResultOp.getResult());
        })
      .Case<StoreComposeOp>([&, this](auto) {
          this->CreateClass(Op);
        });
    Flush();
  }

  void CreateTag(llvm::StringRef Name) {
    OS << "struct " << Name << " { };\n";
  }

  void CreateStrongAlias(llvm::StringRef Name, mlir::Value V) {
    OS << "class " << Name << " : public nbdl::strong_alias<";
    VisitType(V);
    OS << "> {\n"
          "public:\n"
          "using Base = nbdl::strong_alias<";
    VisitType(V);
    OS << ">;\n";
    OS << "using Base::Base;\n";
    OS << "using Base::operator=;\n";
    OS << "};\n";
  }

  void CreateClass(DefineStoreOp Op) {
    llvm::StringRef Name = Op.getName();
    OS << "class " << Name << " {\n";
    OS << "public:\n";
    WriteMemberDecls(Op);
    if (!Op.getBody().getArguments().empty())
      OS << Name << "() = default;\n";
    WriteConstructor(Op);
    OS << "};\n";
    Flush();
  }

  nbdl_spec::ContOp getContOp(DefineStoreOp Op) {
    mlir::Operation* Terminator = Op.getBody().front().getTerminator();
    auto ContOp = dyn_cast<nbdl_spec::ContOp>(Terminator);
    if (!ContOp)
      SetError("expecting nbdl.cont as terminator", Op);
    return ContOp;
  }

  void WriteMemberDecls(DefineStoreOp Op) {
    // Get the ContOp and work backwards
    // saving the member names as we go.
    auto ContOp = getContOp(Op);
    if (!ContOp)
      return;

    WriteMemberDeclRec(ContOp.getArg());
  }

  void WriteMemberDeclRec(mlir::Value Val) {
    if (CheckError())
      return;

    if (auto SCO = Val.getDefiningOp<nbdl_spec::StoreComposeOp>()) {
      WriteMemberDeclRec(SCO);
    } else if (auto EO = Val.getDefiningOp<nbdl_spec::UnitOp>();
               EO || isa<mlir::BlockArgument>(Val)) {
      return;
    } else {
      mlir::Operation* Irr = Val.getDefiningOp();
      SetError("unhandled operation: (WriteMemberDecl) {}", Irr);
    }
  }

  void WriteMemberDeclRec(StoreComposeOp Op) {
    WriteMemberDeclRec(Op.getRhs());

    heavy::SourceLocation PrevLoc = CurLoc;
    SetLoc(Op.getLoc());

    mlir::Value Key = Op.getKey();  // Could be mlir::Value().
    mlir::Value Lhs = Op.getLhs();
    llvm::StringRef Name;

    if (Key && Key.getDefiningOp<nbdl_spec::MemberNameOp>()) {
      auto MemberNameOp = Key.getDefiningOp<nbdl_spec::MemberNameOp>();
      // It would be more consistent with our definition of StoreCompose
      // to support shadowing here, but since it is more work to check
      // and very suboptimal for the C++ compiler
      // and very likely to be a result of programming error,
      // we allow it to slip by and the C++ compiler will present the
      // user with an error.
      Name = SetLocalVal(Lhs, MemberNameOp.getName());
    } else {
      Name = SetLocalVarName(Lhs);
    }

    Members.push_back(Lhs);
    VisitType(Lhs);
    OS << ' ' << Name << ";\n";

    Flush();
    CurLoc = PrevLoc;
  }

  void WriteConstructor(DefineStoreOp Op) {
    auto ContOp = getContOp(Op);
    if (!ContOp)
      return;

    OS << "explicit " << Op.getName();
    OS << '(';
    llvm::interleaveComma(Op.getBody().getArguments(), OS,
        [&](mlir::BlockArgument const& Arg) {
          OS << "auto&& ";
          PrintLocalVal(Arg);
        });
    OS << ") \n: ";

    llvm::interleaveComma(Members, OS,
        [&](mlir::Value M) {
          PrintLocalVal(M);
          OS << '(';
          WriteInitArgs(M);
          OS << ')';
        });
    OS << "\n{ }\n";
  }

  void WriteInitArgs(mlir::Value Member) {
    if (auto StoreOp = Member.getDefiningOp<nbdl_spec::StoreOp>()) {
      llvm::interleaveComma(StoreOp.getArgs(), OS,
        [&](mlir::Value Arg) {
          // Check for later use or possibly later use
          // (due to unspecified order of evaluation)
          if (Arg.hasOneUse() ||
              (llvm::count(StoreOp.getArgs(), Arg) == 1 &&
                !findLaterUse(Arg, StoreOp)))
            WriteLastUseForwardedExpr(Arg);
          else
            WriteExpr(Arg);  // Not forwarded
        });
    } else {
      SetErrorV("unsupported operation (WriteInitArgs)", Member);
    }
  }

  // Find any use of Arg in subsequent stores composed of Store
  // (not including Store itself.)
  StoreOp findLaterUse(mlir::Value Arg, mlir::Value Store) {
    assert(Store.hasOneUse() && "store result should be used exactly once");
    mlir::OpOperand StoreUse = *Store.getUsers().begin();

    return llvm::TypeSwitch<mlir::Operation*, StoreOp>(StoreUse.getOwner())
    .Case<StoreOp>([&](StoreOp S) {
        return llvm::is_contained(S.getArgs(), Arg) ? S : StoreOp();
      })
    .Case<StoreComposeOp>([&](StoreComposeOp SC) {
        // Check that Arg is not used in LHS.
        if (SC.getLhs() != Store)
          if (StoreOp Result = findUse(Arg, SC.getLhs()))
            return Result;
        return findLaterUse(Arg, SC.getResult());
      })
    .Case<ContOp>([](auto) { return StoreOp(); })
    .Default([](auto) {
        llvm_unreachable("unexpected use of store");
        return StoreOp();
      });
  }

  // Find any use of Arg in a Store.
  StoreOp findUse(mlir::Value Arg, mlir::Value Store) {
    return llvm::TypeSwitch<mlir::Operation*, StoreOp>(Store.getDefiningOp())
    .Case<StoreOp>([&](StoreOp S) {
        return llvm::is_contained(S.getArgs(), Arg) ? S : StoreOp();
      })
    .Case<StoreComposeOp>([&](StoreComposeOp SC) {
        // Check both sides.
        if (StoreOp Result = findUse(Arg, SC.getLhs()))
          return Result;
        else
          return findUse(Arg, SC.getRhs());
      })
    .Default([](auto) {
        llvm_unreachable("unexpected result for store");
        return StoreOp();
      });
  }
};

}  // end namespace

namespace nbdl_spec {
std::tuple<std::string, heavy::SourceLocationEncoding*, mlir::Operation*>
translate_cpp(heavy::LexerWriterFnRef LexerWriter, mlir::Operation* Op) {
  if (auto FuncOp = dyn_cast<mlir::func::FuncOp>(Op)) {
    FuncWriter Writer(LexerWriter);
    Writer.Visit(Op);
    return std::make_tuple(std::move(Writer.ErrMsg),
                           Writer.ErrLoc, Writer.Irritant);
  } else if (auto DefineStoreOp = dyn_cast<nbdl_spec::DefineStoreOp>(Op)) {
    DefineStoreWriter Writer(LexerWriter);
    Writer.VisitDefineStore(DefineStoreOp);
    return std::make_tuple(std::move(Writer.ErrMsg),
                           Writer.ErrLoc, Writer.Irritant);
  } else if (auto ContextOp = dyn_cast<nbdl_spec::ContextOp>(Op)) {
    ContextWriter Writer(LexerWriter);
    Writer.VisitContext(ContextOp);
    return std::make_tuple(std::move(Writer.ErrMsg),
                           Writer.ErrLoc, Writer.Irritant);
  } else {
    return std::make_tuple(std::string("unhandled operation"),
                static_cast<heavy::SourceLocationEncoding*>(nullptr), Op);
  }

}
}
