// Nbdl.td - MLIR Dialect Definitions for Nbdl
//
// Copyright Jason Rice 2025
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_DIALECT_TD
#define NBDL_DIALECT_TD

include "mlir/IR/AttrTypeBase.td"
include "mlir/IR/OpBase.td"
include "mlir/IR/SymbolInterfaces.td"

def Nbdl_Dialect : Dialect {
  let name = "nbdl";
  let cppNamespace = "nbdl_spec";
  let useDefaultTypePrinterParser = 1;
}

def Nbdl_Empty : TypeDef<Nbdl_Dialect, "Empty", []> {
  let mnemonic = "empty";
  let description = [{
    We define empty_type as a type whose with no
    state either compile-time or run-time.

    This serves as the identity element for sum types.
  }];
}

def Nbdl_Unit : TypeDef<Nbdl_Dialect, "Unit", []> {
  let mnemonic = "unit";
  let description = [{
    We define unit_type to have a single possible
    compile-time and run-time state.

    This serves as the identity element for product types.
  }];
}

// We keep cpp_typename around for when
// we want to go back to c++.
// DEPRECATED
class Nbdl_TypeBase<string name, string type_mnemonic,
                    list<Trait> traits = []>
  : TypeDef<Nbdl_Dialect, name, traits> {
  let mnemonic = type_mnemonic;
  let parameters = (ins StringRefParameter<>:$cpp_typename);
  let assemblyFormat = "`<` $cpp_typename `>`";
}

// DEPRECATED (I think)
def Nbdl_Opaque : TypeDef<Nbdl_Dialect, "Opaque", []> {
  let mnemonic = "opaque";
  let description = [{
    Represent an placeholder C++ type (typically `auto&&`).
    Values of this type imply an immediate construction.
    That is, it represents a run-time value in the context
    where it appears (e.g. an argument to a function).
  }];
}

def Nbdl_Store : TypeDef<Nbdl_Dialect, "Store", []> {
  let mnemonic = "store";
  let description = [{
    An Object of store type represents a member of our state graph.
    Since a store object can exist as an inactive alternative
    of a variant, it is not necessarily have an immediate
    construction.  Hence, we require it to be semiregular.
    (FIXME or just default constructible?)
  }];
}

def Nbdl_Tag : TypeDef<Nbdl_Dialect, "Tag", []> {
  let mnemonic = "tag";
  let description = [{
    An empty type that only carries information
    at compile-time is called a tag type.
  }];
}

def Nbdl_Variant : TypeDef<Nbdl_Dialect, "Variant", []> {
  let mnemonic = "variant";
  let description = [{
    A type satisfying the nbdl::Variant concept
  }];

/*
  // TODO Eventually it would be nice to specify the alternatives
  //      Also some parameterization for knowing the key => 
  let parameters = (ins Variadic<AnyTypeOf<[Nbdl_Store, Nbdl_Tag,
                                            Nbdl_Unit>:$alts);
  let assemblyFormat = "`<`$alts`>`";
*/
}

def Nbdl_MemberName : TypeDef<Nbdl_Dialect, "member_name"> {
  let mnemonic = "member_name";
  let description = [{
    Represent a string that is a valid C++ identifier.
    The intended primary use case is to specify a member of a Struct.
  }];
}

// TODO Eventually add more specific constraints like variant<...>.
def Nbdl_Type : AnyTypeOf<[Nbdl_Store,
                           Nbdl_Unit,
                           Nbdl_Tag]>;

// TypePlus is for keys callable objects which allow member names
// used for accessing members or calling member functions.
def Nbdl_TypePlus : AnyTypeOf<[Nbdl_Store,
                               Nbdl_Unit,
                               Nbdl_Tag,
                               Nbdl_MemberName]>;

// Implicitly default to the unit type for keys.
def Nbdl_Key : Optional<Nbdl_TypePlus>;

class Nbdl_Op<string mnemonic, list<Trait> traits = []> :
    Op<Nbdl_Dialect, mnemonic, traits>;

def Nbdl_LiteralOp : Nbdl_Op<"literal", []> {
  let description = [{
    Represent a literal such as an int or a string
    that is liftable to some compile-time object
    in C++. Only integers and strings are supported.
  }];

  let arguments = (ins AnyAttrOf<[I32Attr, StrAttr]>:$value);
  let results   = (outs Nbdl_Type:$result);
}

def Nbdl_GetOp : Nbdl_Op<"get", []> {
  let summary = "get";
  let description = [{
  }];
  let arguments = (ins Nbdl_Type:$state,
                       Nbdl_Key:$key);
  let results   = (outs Nbdl_Type:$result);
  let extraClassDeclaration = [{
    bool hasUnitKey() {
      return !getKey() ||
        ::llvm::isa<::nbdl_spec::UnitType>(getKey().getType());
    }
  }];
}

def Nbdl_MatchOp : Nbdl_Op<"match", [Terminator, NoTerminator]> {
  let summary = "match";
  let description = [{
    Given a nbdl::Store and, optionally, a key,
    `match` visits an element contained within that Store using a
    continuation for each of the possible specified overloads.

    Each overload is checked linearly until a matching overload
    is found.  It is an error if there exists an alternative for
    which there exists no matching overload.

    For an example, a `std::unordered_map<int, std::string>` is a store
    that can provide access to a contained element with a key `5`. If the
    element does not exist some `not_found` alternative is matched.
    Alternatively, a `std::variant<Foo, Bar>` is a store that
    also provides access to its contained element without a key
    (ie it is just unwrapped). This requires a visitor to be overloaded
    with every possible alternative.
  }];

  let arguments = (ins Nbdl_Type:$store,
                       Nbdl_Key:$key);
  let results = (outs);
  let regions = (region SizedRegion<1>:$overloads);
  let extraClassDeclaration = [{
    bool hasUnitKey() {
      return !getKey() ||
        ::llvm::isa<::nbdl_spec::UnitType>(getKey().getType());
    }
  }];
}

def Nbdl_OverloadOp : Nbdl_Op<"overload", []> {
  let description = [{
    Visit the body when the input matches a specific typename
    not including any qualifiers or when the type is an empty string.
    If the body is not empty, it takes an argument that is the matched
    object. An empty body is a noop.
  }];
  let arguments = (ins StrAttr:$type);
  let results = (outs);
  let regions = (region AnyRegion:$body);
}

def Nbdl_NoOp : Nbdl_Op<"noop", [Terminator]> {
  let description = [{
    NoOp visits a store but does nothing except mark it as used.
  }];
  let arguments = (ins Nbdl_Type:$arg);
  let results = (outs);
}

def Nbdl_MatchIfOp : Nbdl_Op<"match_if", [Terminator]> {
  let description = [{
    Apply a predicate to an object and branch on the result
    like an if/else statement.
  }];

  let arguments = (ins Nbdl_Type:$input, Nbdl_Type:$pred);
  let regions   = (region SizedRegion<1>:$thenRegion,
                          SizedRegion<1>:$elseRegion);
}

def Nbdl_ConstexprOp : Nbdl_Op<"constexpr", []> {
  let description = [{
    The result of a constant expression in C++.
  }];
  let arguments = (ins StrAttr:$expr);
  let results = (outs Nbdl_Type:$result);
}

def Nbdl_MemberNameOp : Nbdl_Op<"member_name", []> {
  let description = [{
    Denote a member name as a value.
  }];

  let arguments = (ins StrAttr:$name);
  let results = (outs Nbdl_MemberName:$result);
}

def Nbdl_StoreComposeOp : Nbdl_Op<"store_compose", []> {
  let description = [{
    Add a value $lhs as a member to another store $rhs using a key $key
    to access the member. The type $key will shadow any key of the same
    type in $rhs (ie values are not considered.)

    (Writing logic to shadow a subset of key
     values is better implemented in C++.)
  }];

  let arguments = (ins
    Nbdl_Key:$key,
    Nbdl_Type:$lhs,
    Nbdl_Type:$rhs);
  let results = (outs Nbdl_Type:$result);
  let extraClassDeclaration = [{
    bool hasUnitKey() {
      return !getKey() ||
        ::llvm::isa<::nbdl_spec::UnitType>(getKey().getType());
    }
  }];
}

def Nbdl_VariantOp : Nbdl_Op<"variant", []> {
  let description = [{
    Create a sum type.
  }];
  let arguments = (ins Variadic<Nbdl_Type>:$args);
  let results = (outs Nbdl_Type:$result);
}

def Nbdl_DefineStoreOp : Nbdl_Op<"define_store", [Symbol, IsolatedFromAbove]> {
  let description = [{
    Define a store object, and expose its interface (type) to the user.

    If no region exists, then the operation is considered external
    and should refer to an existing C++ type that satisfies nbdl::Store.
    Otherwise, a new C++ struct type is created. The symbol should be a
    fully namespace qualified C++ type name.

    Use ContOp to terminate and finalize the object.
    If ContOp receives the body argument directly then the
    created type is a Tag.
  }];

  let regions = (region AnyRegion:$body);
  let arguments = (ins StrAttr:$sym_name);
  let results = (outs);
  let extraClassDeclaration = [{
    bool isExternal() { return getBody().empty(); }

    // Get the fully qualified name.
    // This is needed for defining template specializations.
    llvm::StringRef getFullName() { return getSymName(); }

    // Get the unqualified name.
    llvm::StringRef getName() {
      llvm::StringRef SymName = getSymName();
      return SymName.take_back(SymName.rfind(':'));
    }
  }];
}

def Nbdl_ContextOp : Nbdl_Op<"context", [Symbol]> {
  let description = [{
    Encapsulate a store object to serve as a root node in a state tree.

    Nested objects are still accessible via `match` and `get`,
    but the context wrapper supports the convention that the root
    node cannot be a non-root member of a path.

    An instance can be created with nbdl.store.
  }];

  let arguments = (ins StrAttr:$sym_name, FlatSymbolRefAttr:$implName);
  let results = (outs);

  let extraClassDeclaration = [{
    // Get the fully qualified name.
    // This is needed for defining template specializations.
    llvm::StringRef getFullName() { return getSymName(); }

    // Get the unqualified name.
    llvm::StringRef getName() {
      llvm::StringRef SymName = getSymName();
      return SymName.take_back(SymName.rfind(':'));
    }
  }];
}

def Nbdl_ContOp : Nbdl_Op<"cont", [Terminator]> {
  let description = [{
    The terminator we need.
  }];

  let arguments = (ins Nbdl_Type:$arg);
  let results = (outs);
}

def Nbdl_StoreOp : Nbdl_Op<"store", []> {
  let description = [{
    Construct a store object. The name should be a
    symbol that refers to a CreateStoreOp which can
    be external.
  }];

  let arguments = (ins
    FlatSymbolRefAttr:$name,
    Variadic<AnyType>:$args);
  let results = (outs Nbdl_Type:$result);
}

def Nbdl_EmptyOp : Nbdl_Op<"empty", []> {
  let description = [{
    An operation that produces an object of the empty type
    which is just absurd.
  }];

  let arguments = (ins);
  let results = (outs Nbdl_Empty:$result);
}

def Nbdl_UnitOp : Nbdl_Op<"unit", []> {
  let description = [{
    An operation that produces an object of the unit type.
  }];

  let arguments = (ins);
  let results = (outs Nbdl_Unit:$result);
}

def Nbdl_DiscardOp : Nbdl_Op<"discard", [Terminator]> {
  let description = [{
    Discard the result of a nbdl.visit call.

    Note:
    This is really only used to terminate the block
    and is the most command case for nbdl.visit.
    I don't know that we will ever want to "return"
    a value.
  }];

  let arguments = (ins Nbdl_Type:$arg);
  let results   = (outs);
}

def Nbdl_VisitOp : Nbdl_Op<"visit", []> {
  let description = [{
    Call a function with resolved stores as arguments
    returning a result. This is analogous to std::visit
    except only one function is generated.
  }];
  let arguments = (ins Nbdl_TypePlus:$fn,
                       Variadic<Nbdl_Type>:$args);
  let results = (outs Nbdl_Type:$result);
}

def Nbdl_ConsumerOp : Nbdl_Op<"consumer", []> {
  let description = [{
    A consumer is an object provided with an object
    to match a set of objects in a store.
    The $name should be a template that takes the
    type of $matchFn yielding the type to construct.
  }];

  let arguments = (ins
    FlatSymbolRefAttr:$name,
    AnyType:$matchFn);
  let results = (outs Nbdl_Type:$result);
}

def Nbdl_ConstOp : Nbdl_Op<"const", []> {
  let description = [{
    Cast an object to const likely using std::as_const.

    Notes:
    We don't modify the type in IR land but rely on C++
    to manage underlying types. This is analogous to an
    immutable borrow I think.
  }];

  let arguments = (ins Nbdl_Type:$arg);
  let results = (outs Nbdl_Type:$result);
}

def Nbdl_ScopeOp : Nbdl_Op<"scope", []> {
  let description = [{
    Terminating operations in a function may mutate a store
    thus invalidating any matched store with that object as
    a node in its root path.  We can nest intermediate operations
    in a nbdl.scope region to prevent subsequent uses of such
    objects.
  }];

  let arguments = (ins);
  let results = (outs);
  let regions = (region SizedRegion<1>:$body);

  let builders = [
    OpBuilder<(ins "std::unique_ptr<::mlir::Region>":$body)>
  ];
}

#endif
