//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_JS_HPP
#define NBDL_JS_HPP

#include <nbdl/detail/js_val.hpp>
#include <nbdl/fwd/js.hpp>
#include <nbdl/macros/NBDL_JS_PROMISE.hpp>
#include <nbdl/webui/detail/event_receiver.hpp>

#include <boost/hana/core/when.hpp>
#include <boost/hana/equal.hpp>
#include <emscripten.h>
#include <type_traits>

//
// NBDL_JS_TRANSFORM - transforms a nbdl::js::val
//
#define NBDL_JS_TRANSFORM(val, F) EM_ASM_({ \
  Module.NBDL_DETAIL_JS_SET($0, F(Module.NBDL_DETAIL_JS_GET($0))); \
  } , nbdl::js::get_handle(val))

namespace nbdl::js
{
  inline void init()
  {
    nbdl::detail::js_val::init_registry();
  }

  //
  // is_js_val
  //

  template <>
  constexpr bool is_js_val_impl<val> = true;

  template <>
  constexpr bool is_js_val_impl<::nbdl::detail::js_val> = true;

  template <>
  constexpr bool is_js_val_impl<callback_t> = true;

  //
  // val
  //

  template <typename T>
  void val::copy(T const& other)
  {
    EM_ASM_(
      { Module.NBDL_DETAIL_JS_SET($0, Module.NBDL_DETAIL_JS_GET($1)); }
    , handle()
    , nbdl::js::get_handle(other)
    );
  }

  //
  // callback
  //

  namespace detail
  {
    template <typename Fn>
    struct callback_event_receiver
    {
      Fn fn;
      std::unique_ptr<webui::detail::event_receiver_vals> vals;

      void receive_event()
      {
        fn(vals->event_data);
      }
    };
  }

  inline int callback_t::handle() const
  {
    return receiver->virtual_(webui::detail::handler_s)(*receiver).handle();
  }

  template <typename Fn>
  callback_t callback_fn::operator()(Fn&& fn) const
  {
    auto receiver = webui::detail::make_event_receiver(
      detail::callback_event_receiver<std::decay_t<Fn>>{
        std::forward<Fn>(fn)
      , std::unique_ptr<webui::detail::event_receiver_vals>(new webui::detail::event_receiver_vals())
      }
    );

    return callback_t{int{}, std::move(receiver)};
  }

  template <typename T>
  auto get_handle_fn::operator()(T const& v) const
    -> std::enable_if_t<is_js_val<T>, int>
  { return v.handle(); }

  namespace detail
  {
    // deprecated
    constexpr get_handle_fn get_handle{};
  }
}

namespace boost::hana
{
  // Comparable

  template <typename T, typename U>
  struct equal_impl<T, U, when<nbdl::js::is_js_val<T> and nbdl::js::is_js_val<U>>>
  {
    template <typename X, typename Y>
    static bool apply(X const& x, Y const& y)
    {
      return EM_ASM_INT(
        { return Module.NBDL_DETAIL_JS_GET($0) === Module.NBDL_DETAIL_JS_GET($1); }
      , nbdl::js::get_handle(x)
      , nbdl::js::get_handle(y)
      );
    }
  };
}

#endif
