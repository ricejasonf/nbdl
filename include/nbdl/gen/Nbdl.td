// Ops.td - MLIR Dialect Definitions for Nbdl
//
// Copyright Jason Rice 2024
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_DIALECT_OPS_TD
#define NBDL_DIALECT_OPS_TD

include "mlir/IR/AttrTypeBase.td"
include "mlir/IR/OpBase.td"

def Nbdl_Dialect : Dialect {
  let name = "nbdl::gen";
}

class Nbdl_TypeBase<string name, string type_mnemonic,
                    list<Trait> traits = []>
  : TypeDef<Nbdl_Dialect, name, traits> {
  let mnemonic = type_mnemonic;
}

def Nbdl_Type : Nbdl_TypeBase<"Opaque", "opaque_type"> {
  let summary = "Nbdl opaque C++ type specifier";
  let description = [{
    This is type represents a C++ typename or a placeholder type
    which permits the use of a type-constraint (ie a concept).
    The `cpp_typename` should map directly to a C++ type specifier
    preferably with fully qualified namespaces.
  }];

  let parameters = (ins StringRefParameter<>:$cpp_typename);
  let assemblyFormat = "`<` $cpp_typename `>`";
}

// Keys are optional, but we are using variadic arguments
// so we resort to using this sum type.
def Nbdl_Key : AnyTypeOf<[Nbdl_Type, NoneType]>;

def Nbdl_TypeAttr : TypeAttrBase<"::nbdl::gen::Type",
                                 "a nbdl type attribute">;
def Nbdl_TypeArrayAttr : TypedArrayAttrBase<Nbdl_TypeAttr,
                                            "nbdl type array attribute">;

class Nbdl_Op<string mnemonic, list<Trait> traits = []> :
    Op<Nbdl_Dialect, mnemonic, traits>;

def Nbdl_TagOp : Nbdl_Op<"tag", []> {
  let summary = "tag";
  let description = [{
    Tag represents an instance of an empty, default
    constructible type that is used for tag
    dispatch. This is intended primarily for use as
    a key and allows avoiding capture of stateless
    values.
  }];

  let arguments = (Nbdl_TypeAttr:$input_type);
  let results   = (outs Nbdl_Type:$result);
}

def Nbdl_GetOp : Nbdl_Op<"get", []> {
  let summary = "get";
  let description = [{
  }];
  let arguments = (ins Nbdl_Type:$store,
                       Nbdl_Key:$key);
  let results   = (outs Nbdl_Type:$result);
}

def Nbdl_MatchOp : Nbdl_Op<"match", [Terminator]> {
  let summary = "match";
  let description = [{
    Given a continuation function, a nbdl::Store and, optionally, a key,
    `match` visits an element contained within that Store using a
    continuation for each of the possible typed alternatives.

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

    Each region receives fn, store, and the captures as their
    arguments.
  }];
  let arguments = (ins Nbdl_TypeArrayAttr:$alternative,
                       Nbdl_Type:$fn,
                       Nbdl_Type:$store,
                       Nbdl_Key:$key,
                       Variadic<Nbdl_Type>:$captures);
  let results = (outs);
  let regions = (region VariadicRegion<SizedRegion<1>>:$matchRegions);
}

def Nbdl_ContOp : Nbdl_Op<"cont", [Terminator]> {
  let summary = "cont";
  let description = [{
    ContOp represents the call to a generic continuation object
    passing the resolved object and any additional captures
    as arguments.
  }];
  let arguments = (ins Nbdl_TypeAttr:$fn,
                       Nbdl_Type:$arg,
                       Variadic<Nbdl_Type>:$restArgs);
  let results = (outs);
}

#endif
