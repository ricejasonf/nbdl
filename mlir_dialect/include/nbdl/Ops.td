// Ops.td - MLIR Dialect Definitions for Nbdl
//
// Copyright Jason Rice 2024
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_DIALECT_OPS_TD
#define NBDL_DIALECT_OPS_TD

include "mlir/IR/OpBase.td"
include "mlir/IR/RegionKindInterface.td"
include "mlir/IR/SymbolInterfaces.td"
include "mlir/Interfaces/ControlFlowInterfaces.td"
include "mlir/Interfaces/SideEffectInterfaces.td"

def Nbdl_Dialect : Dialect {
  let name = "nbdl";
}

class Nbdl_Op<string mnemonic, list<Trait> traits = []> :
    Op<Nbdl_Dialect, mnemonic, traits>;

class Nbdl_TypeBase<string cpp_typename>
  : Type<CPred<"mlir::isa<::nbdl::Nbdl_TypeBase>($_self)">,
               "cpp_typename", "::nbdl::Nbdl_TypeBase">;

class Nbdl_Type<"opaque_type"> : Nbdl:Type<"opaque_type"> {
  let summary = "Nbdl opaque C++ type specifier";
  let description = [{
    This is type represents a C++ typename or a placeholder type
    which permits the use of a type-constraint (ie a concept).
    The `cpp_typename` should map directly to a C++ type specifier
    preferably with fully qualified namespaces.
  }];

  let parameters = (ins StringRefParameter<>:$cpp_typename)
};

def Nbdl_TypeAttr : TypeAttrBase<"::nbdl::ops::Type",
                                       "a nbdl type attribute">;
def Nbdl_TypeArrayAttr : TypedArrayAttrBase<Nbdl_TypeAttr,
                                                  "nbdl type array attribute">;

class Nbdl_Op<string mnemonic, list<Trait> traits = []> :
    Op<Nbdl_Dialect, mnemonic, traits>;

def Nbdl_GetOp : Nbdl_Op<"get", []> {
  let summary = "get";
  let description = [{
  }];
  let arguments = (ins Nbdl_Type:$store,
                       Optional<Nbdl_Type>:$key)
  let results   = (outs Nbdl_Type:$result);
};

def Nbdl_MatchOp : Nbdl_Op<"match", [Terminator]> {
  let summary = "match";
  let description = [{
    Given a nbdl::Store and, optionally, a key, Match visits an element
    contained within that Store using a continuation for each of
    the possible typed alternatives.

    Stores match values of different types, so a region is
    used to provide the continuation for each possible
    alternative which is checked linearly.

    It is an error for the matched object to not have a matching
    alternative.

    For an example, a `std::unordered_map<int, std::string>` is a store
    that can provide access to a contained element with a key `5`. If the
    element does not exist a `not_found` alternative is matched.
    Alternatively, a `std::variant<Foo, Bar>` is a store that
    provides access to its contained element without a key
    (ie it is just unwrapped).
  }];
  let arguments = (ins Nbdl_Type:$store,
                       Optional<Nbdl_Type>:$key,
                       Nbdl_TypeArrayAttr:$alternative);
  let results = (outs);
  let regions = (region VariadicRegion<SizedRegion<1>>:$matchRegions)
};

#endif
