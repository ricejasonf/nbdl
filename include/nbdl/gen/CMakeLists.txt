# FIXME Fix the toolchain file to do this.
include_directories("/opt/sysroot/include")

add_mlir_dialect(Nbdl nbdl::gen)
#set(LLVM_TARGET_DEFINITIONS Ops.td)
#mlir_tablegen(Ops.h.inc -gen-op-decls)
#mlir_tablegen(Ops.cpp.inc -gen-op-defs)
#add_public_tablegen_target(NbdlOpsIncGen)
