#ifndef NBDL_SPEC_NBDL_DIALECT_H
#define NBDL_SPEC_NBDL_DIALECT_H

#include <mlir/Dialect/Func/IR/FuncOps.h>
#include <mlir/IR/BuiltinOps.h>
#include <mlir/IR/Dialect.h>
#include <mlir/IR/OpDefinition.h>
#include <mlir/Interfaces/ControlFlowInterfaces.h>
#include <mlir/Interfaces/SideEffectInterfaces.h>

namespace nbdl_spec {
using mlir::func::CallOp;
using mlir::func::FuncOp;
using mlir::func::ReturnOp;
using mlir::StringAttr;
}

// Include the generated header files
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"

#include "nbdl_spec/NbdlDialect.h.inc"

#define GET_TYPEDEF_CLASSES
#include "nbdl_spec/NbdlTypes.h.inc"

#define GET_ATTRDEF_CLASSES
#include "nbdl_spec/NbdlAttrs.h.inc"

#define GET_OP_CLASSES
#include "nbdl_spec/NbdlOps.h.inc"
#pragma clang diagnostic pop

#endif
