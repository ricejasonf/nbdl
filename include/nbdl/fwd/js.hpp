//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_FWD_JS_HPP
#define NBDL_FWD_JS_HPP

#include <nbdl/detail/js_val.hpp>
#include <nbdl/webui/detail/event_receiver.hpp>

#include <memory>

namespace nbdl::js
{
  //
  // init - This should be called at the beginning of any emscripten
  //        program using Nbdl.

  inline void init();

  //
  // is_js_val
  //

  template <typename T>
  constexpr bool is_js_val_impl = false;

  template <typename T>
  constexpr bool is_js_val = is_js_val_impl<std::decay_t<T>>;

  //
  // get_handle - Used to get javascript handle
  //

  struct get_handle_fn
  {
    template <typename T>
    auto operator()(T const& v) const
      -> std::enable_if_t<is_js_val<T>, int>;
  };

  constexpr get_handle_fn get_handle{};

  //
  // val - This represents a handle to a value in javascript. The copy
  //       constructor creates new entries in the js hash table lookup
  //       so it is not good to copy it around much.
  //

  struct callback_t;

  struct val : private ::nbdl::detail::js_val
  {
    val() = default;

    val(val const& other)
    {
      copy(other);
    }

    template <typename JsVal>
    val(JsVal const& other, std::enable_if_t<is_js_val<JsVal>, int> = 0)
    { copy(other); }

    val& operator=(val const& other)
    {
      copy(other);
      return *this;
    }

    private:

    friend get_handle_fn;

    template <typename T>
    void copy(T const& other);
  };

  //
  // callback - A unary function that can be called from javascript.
  //            Calling the function after the end of life of this
  //            object will result in undefined behaviour.
  //
  //            Consider this a low-level tool for advanced use cases.
  //

  struct callback_fn;

  struct callback_t
  {
    private:

    friend callback_fn;
    friend get_handle_fn;
    std::unique_ptr<webui::detail::event_receiver> receiver;

    explicit callback_t(int, std::unique_ptr<webui::detail::event_receiver>&& r)
      : receiver(std::move(r))
    { }

    inline int handle() const;
  };

  struct callback_fn
  {
    template <typename Fn>
    callback_t operator()(Fn&& fn) const;
  };

  constexpr callback_fn callback{};
}

#endif
