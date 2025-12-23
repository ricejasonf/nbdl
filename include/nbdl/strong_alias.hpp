//
// Copyright Jason Rice 2025
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_STRONG_ALIAS_HPP
#define NBDL_STRONG_ALIAS_HPP

#include <nbdl/concept/extras.hpp>
#include <nbdl/concept/State.hpp>
#include <nbdl/concept/Store.hpp>
#include <nbdl/get.hpp>
#include <nbdl/match.hpp>

namespace nbdl {
template <typename Value, bool is_moveable = true>
class strong_alias {
  Value value;

public:
  using value_type = Value;

  // The defaulted constructors use subsumption to
  // beat the forwarding constructor.
  strong_alias(strong_alias const&) = default;
  strong_alias(strong_alias&) = default;
  strong_alias(strong_alias&&) = default;
  explicit strong_alias(auto&& ... args) :
    value(static_cast<decltype(args)>(args) ...)
  { }

  strong_alias& operator=(this auto& self,
                          NotSameAs<Value> auto&& arg) {
    self.value = static_cast<decltype(arg)>(arg);
    return self;
  }

  auto&& nbdl_get_strong_alias_value(this auto&& self) {
    return static_cast<decltype(self)>(self).value;
  }
};

template <StateAlias Alias>
  requires nbdl::State<typename Alias::value_type>
struct get_impl<Alias> {
  template <typename State>
  static constexpr decltype(auto) apply(State&& s) {
    return (s);
  }

  template <typename State, typename Key>
  static constexpr decltype(auto) apply(State&& s, Key&& k) {
    return nbdl::get(std::forward<State>(s).nbdl_get_strong_alias_value(),
                     std::forward<Key>(k));
  }
};

template <StoreAlias T>
 requires (!nbdl::StateAlias<T>)
struct match_impl<T> {
  template <typename StoreAlias, typename Fn>
    requires detail::HasMatchUnitImpl<typename T::value_type>
  static constexpr void apply(StoreAlias&& s, Fn&& fn) {
    match(std::forward<StoreAlias>(s).nbdl_get_strong_alias_value(),
          std::forward<Fn>(fn));
  }

  template <typename StoreAlias, typename Key, typename Fn>
  static constexpr void apply(StoreAlias&& s, Key&& k, Fn&& fn) {
    match(std::forward<StoreAlias>(s).nbdl_get_strong_alias_value(),
          std::forward<Key>(k),
          std::forward<Fn>(fn));
  }
};

// context_alias

// TODO This should replace the legacy nbdl::context.
template <Store T, bool is_moveable = true>
class context_alias {
  T value;

public:
  // The defaulted constructors use subsumption to
  // beat the forwarding constructor.
  context_alias(context_alias const&) = default;
  context_alias(context_alias&) = default;
  context_alias(context_alias&&) = default;
  explicit context_alias(auto&& ... args) :
    value(static_cast<decltype(args)>(args) ...)
  { }

  auto&& nbdl_get_context_alias_value(this auto&& self) {
    return static_cast<decltype(self)>(self).value; 
  }
};

template <Store T>
class context_alias<T, /*is_moveable=*/false> {
  T value;

public:
  // The deleted constructors use subsumption to
  // beat the forwarding constructor.
  context_alias(context_alias const&) = delete;
  context_alias(context_alias&) = delete;
  context_alias(context_alias&&) = delete;
  explicit context_alias(auto&& ... args) :
    value(static_cast<decltype(args)>(args) ...)
  { }

  auto&& nbdl_get_context_alias_value(this auto&& self) {
    return static_cast<decltype(self)>(self).value; 
  }
};

}  // end namespace nbdl

#endif
