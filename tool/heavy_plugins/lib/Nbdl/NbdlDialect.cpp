// Copyright Jason Rice 2025
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// Include all of the generated header/source files for the nbdl mlir dialect.
// 
#include <nbdl_spec/NbdlDialect.h>
#include <mlir/IR/DialectImplementation.h>
#include <mlir/IR/OpImplementation.h>
#include <llvm/ADT/TypeSwitch.h>

// Include generated source files.
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"

#include "nbdl_spec/NbdlDialect.cpp.inc"

#define GET_TYPEDEF_CLASSES
#include "nbdl_spec/NbdlTypes.cpp.inc"
#define GET_ATTRDEF_CLASSES
#include "nbdl_spec/NbdlAttrs.cpp.inc"
#define GET_OP_CLASSES
#include "nbdl_spec/NbdlOps.cpp.inc"

void nbdl_spec::NbdlDialect::initialize() {
  addTypes<
#define GET_TYPEDEF_LIST
#include "nbdl_spec/NbdlTypes.cpp.inc"
    >();
  addAttributes<
#define GET_ATTRDEF_LIST
#include "nbdl_spec/NbdlAttrs.cpp.inc"
      >();
  addOperations<
#define GET_OP_LIST
#include "nbdl_spec/NbdlOps.cpp.inc"
      >();

}
#pragma clang diagnostic pop

void nbdl_spec::ScopeOp::build(::mlir::OpBuilder&,
                              ::mlir::OperationState& odsState,
                              std::unique_ptr<::mlir::Region> body) {
  odsState.addRegion(std::move(body));
}
