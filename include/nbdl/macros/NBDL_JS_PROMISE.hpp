//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_JS_PROMISE

#include <nbdl/hold.hpp>
#include <nbdl/js.hpp>
#include <nbdl/macros/NBDL_DETAIL_VFUNC.hpp>
#include <nbdl/promise.hpp>

#include <boost/hana/tuple.hpp>
#include <array>
#include <utility>

namespace nbdl::js::detail
{
  constexpr auto make_promise = [](auto js_routine)
  {
    return nbdl::promise([js_routine](auto& resolver, auto const& input_vals)
    {
      // Really weird lifetime management stuffs here
      // pair(resolve, reject)
      std::pair<callback_t, callback_t>* delete_me{};

      auto resolve = callback([&resolver, delete_me](auto const& result)
      {
        delete delete_me;
        resolver.resolve(result);
      });

      auto reject  = callback([&resolver, delete_me](auto const& result)
      {
        delete delete_me;
        resolver.reject(result);
      });

      delete_me = new std::pair<callback_t, callback_t>(std::move(resolve), std::move(reject));

      js_routine(delete_me->first, delete_me->second, input_vals);
    });
  };

  constexpr auto collect_js_vals = [](auto const& ...vals)
  {
    static_assert(sizeof...(vals) > 0);
    return std::array<val, sizeof...(vals)>{{val{vals}...}};
  };

  constexpr auto collect_js_vals_reverse = [](auto const& ...inputs)
  {
    static_assert(sizeof...(inputs) > 0);
    return hana::unpack(hana::reverse(hana::make_tuple(val{inputs}...)), [](auto const& ...vals)
    {
      return std::array<val, sizeof...(vals)>{{vals...}};
    });
  };
}

//
// NBDL_DETAIL_JS_PROMISE_COLLECT_ARGS
//
#define NBDL_DETAIL_JS_PROMISE_COLLECT_ARG1(NAME) NAME 
#define NBDL_DETAIL_JS_PROMISE_COLLECT_ARGS(...) \
  NBDL_DETAIL_VFUNC(NBDL_DETAIL_JS_PROMISE_COLLECT_ARG, __VA_ARGS__)
#define NBDL_DETAIL_JS_PROMISE_COLLECT_ARG9(NAME, ...) \
  NBDL_DETAIL_JS_PROMISE_COLLECT_ARG1(NAME), NBDL_DETAIL_JS_PROMISE_COLLECT_ARG8(__VA_ARGS__)
#define NBDL_DETAIL_JS_PROMISE_COLLECT_ARG8(NAME, ...) \
  NBDL_DETAIL_JS_PROMISE_COLLECT_ARG1(NAME), NBDL_DETAIL_JS_PROMISE_COLLECT_ARG7(__VA_ARGS__)
#define NBDL_DETAIL_JS_PROMISE_COLLECT_ARG7(NAME, ...) \
  NBDL_DETAIL_JS_PROMISE_COLLECT_ARG1(NAME), NBDL_DETAIL_JS_PROMISE_COLLECT_ARG6(__VA_ARGS__)
#define NBDL_DETAIL_JS_PROMISE_COLLECT_ARG6(NAME, ...) \
  NBDL_DETAIL_JS_PROMISE_COLLECT_ARG1(NAME), NBDL_DETAIL_JS_PROMISE_COLLECT_ARG5(__VA_ARGS__)
#define NBDL_DETAIL_JS_PROMISE_COLLECT_ARG5(NAME, ...) \
  NBDL_DETAIL_JS_PROMISE_COLLECT_ARG1(NAME), NBDL_DETAIL_JS_PROMISE_COLLECT_ARG4(__VA_ARGS__)
#define NBDL_DETAIL_JS_PROMISE_COLLECT_ARG4(NAME, ...) \
  NBDL_DETAIL_JS_PROMISE_COLLECT_ARG1(NAME), NBDL_DETAIL_JS_PROMISE_COLLECT_ARG3(__VA_ARGS__)
#define NBDL_DETAIL_JS_PROMISE_COLLECT_ARG3(NAME, ...) \
  NBDL_DETAIL_JS_PROMISE_COLLECT_ARG1(NAME), NBDL_DETAIL_JS_PROMISE_COLLECT_ARG2(__VA_ARGS__)
#define NBDL_DETAIL_JS_PROMISE_COLLECT_ARG2(NAME, NAME2) \
  NBDL_DETAIL_JS_PROMISE_COLLECT_ARG1(NAME), NBDL_DETAIL_JS_PROMISE_COLLECT_ARG1(NAME2)

//
// NBDL_DETAIL_JS_PROMISE_ARG_HANDLES
//
#define NBDL_DETAIL_JS_PROMISE_ARG_HANDLE1(NAME) ::nbdl::js::detail::get_handle(input_vals[0])
#define NBDL_DETAIL_JS_PROMISE_ARG_HANDLES(...) \
  NBDL_DETAIL_VFUNC(NBDL_DETAIL_JS_PROMISE_ARG_HANDLE, __VA_ARGS__)
#define NBDL_DETAIL_JS_PROMISE_ARG_HANDLE9(NAME, ...) \
  ::nbdl::js::detail::get_handle(input_vals[8]), NBDL_DETAIL_JS_PROMISE_ARG_HANDLE8(__VA_ARGS__)
#define NBDL_DETAIL_JS_PROMISE_ARG_HANDLE8(NAME, ...) \
  ::nbdl::js::detail::get_handle(input_vals[7]), NBDL_DETAIL_JS_PROMISE_ARG_HANDLE7(__VA_ARGS__)
#define NBDL_DETAIL_JS_PROMISE_ARG_HANDLE7(NAME, ...) \
  ::nbdl::js::detail::get_handle(input_vals[6]), NBDL_DETAIL_JS_PROMISE_ARG_HANDLE6(__VA_ARGS__)
#define NBDL_DETAIL_JS_PROMISE_ARG_HANDLE6(NAME, ...) \
  ::nbdl::js::detail::get_handle(input_vals[5]), NBDL_DETAIL_JS_PROMISE_ARG_HANDLE5(__VA_ARGS__)
#define NBDL_DETAIL_JS_PROMISE_ARG_HANDLE5(NAME, ...) \
  ::nbdl::js::detail::get_handle(input_vals[4]), NBDL_DETAIL_JS_PROMISE_ARG_HANDLE4(__VA_ARGS__)
#define NBDL_DETAIL_JS_PROMISE_ARG_HANDLE4(NAME, ...) \
  ::nbdl::js::detail::get_handle(input_vals[3]), NBDL_DETAIL_JS_PROMISE_ARG_HANDLE3(__VA_ARGS__)
#define NBDL_DETAIL_JS_PROMISE_ARG_HANDLE3(NAME, ...) \
  ::nbdl::js::detail::get_handle(input_vals[2]), NBDL_DETAIL_JS_PROMISE_ARG_HANDLE2(__VA_ARGS__)
#define NBDL_DETAIL_JS_PROMISE_ARG_HANDLE2(NAME, NAME2) \
  ::nbdl::js::detail::get_handle(input_vals[1]), NBDL_DETAIL_JS_PROMISE_ARG_HANDLE1(NAME2)

//
// NBDL_DETAIL_JS_PROMISE_ROUTINE_INIT_CAPTURES
//
#define NBDL_DETAIL_JS_PROMISE_INIT_CAPTURES(...) \
  NBDL_DETAIL_VFUNC(NBDL_DETAIL_JS_PROMISE_INIT_CAPTURE, __VA_ARGS__)
#define NBDL_DETAIL_JS_PROMISE_INIT_CAPTURE8(NAME, ...) \
  var NAME = Module.NBDL_DETAIL_JS_GET($9); NBDL_DETAIL_JS_PROMISE_INIT_CAPTURE7(__VA_ARGS__)
#define NBDL_DETAIL_JS_PROMISE_INIT_CAPTURE7(NAME, ...) \
  var NAME = Module.NBDL_DETAIL_JS_GET($8); NBDL_DETAIL_JS_PROMISE_INIT_CAPTURE6(__VA_ARGS__)
#define NBDL_DETAIL_JS_PROMISE_INIT_CAPTURE6(NAME, ...) \
  var NAME = Module.NBDL_DETAIL_JS_GET($7); NBDL_DETAIL_JS_PROMISE_INIT_CAPTURE5(__VA_ARGS__)
#define NBDL_DETAIL_JS_PROMISE_INIT_CAPTURE5(NAME, ...) \
  var NAME = Module.NBDL_DETAIL_JS_GET($6); NBDL_DETAIL_JS_PROMISE_INIT_CAPTURE4(__VA_ARGS__)
#define NBDL_DETAIL_JS_PROMISE_INIT_CAPTURE4(NAME, ...) \
  var NAME = Module.NBDL_DETAIL_JS_GET($5); NBDL_DETAIL_JS_PROMISE_INIT_CAPTURE3(__VA_ARGS__)
#define NBDL_DETAIL_JS_PROMISE_INIT_CAPTURE3(NAME, ...) \
  var NAME = Module.NBDL_DETAIL_JS_GET($4); NBDL_DETAIL_JS_PROMISE_INIT_CAPTURE2(__VA_ARGS__)
#define NBDL_DETAIL_JS_PROMISE_INIT_CAPTURE2(NAME, ...) \
  var NAME = Module.NBDL_DETAIL_JS_GET($3); NBDL_DETAIL_JS_PROMISE_INIT_CAPTURE1(__VA_ARGS__)
#define NBDL_DETAIL_JS_PROMISE_INIT_CAPTURE1(NAME, ...) \
  var NAME = Module.NBDL_DETAIL_JS_GET($2);

//
// NBDL_DETAIL_JS_PROMISE_ROUTINE
//

#define NBDL_DETAIL_JS_PROMISE_ROUTINE(code) \
  [](auto const& resolve, auto const& reject, auto const&) { \
    EM_ASM_({ \
      (function() { (code)(Module.NBDL_DETAIL_JS_GET($0), Module.NBDL_DETAIL_JS_GET($1)); })(); \
    }, ::nbdl::js::detail::get_handle(resolve), ::nbdl::js::detail::get_handle(reject)); \
  }

#define NBDL_DETAIL_JS_PROMISE_ROUTINE_WITH_CAPTURE_IMPL(code, captures, ...) \
  [](auto const& resolve, auto const& reject, auto const& input_vals) { \
    EM_ASM_({ \
      (function() { captures \
        (code)(Module.NBDL_DETAIL_JS_GET($0), Module.NBDL_DETAIL_JS_GET($1)); })(); \
    }, ::nbdl::js::detail::get_handle(resolve), ::nbdl::js::detail::get_handle(reject), \
      NBDL_DETAIL_JS_PROMISE_ARG_HANDLES(__VA_ARGS__)); \
  }

#define NBDL_DETAIL_JS_PROMISE_ROUTINE_WITH_CAPTURE(code, ...) \
  NBDL_DETAIL_JS_PROMISE_ROUTINE_WITH_CAPTURE_IMPL( \
    code \
  , NBDL_DETAIL_JS_PROMISE_INIT_CAPTURES(__VA_ARGS__), __VA_ARGS__ \
  )

#define NBDL_DETAIL_JS_PROMISE_ROUTINE_C_FUNC(fn, ...) \
  [](auto const& resolve, auto const& reject, auto const& input_vals) { \
    fn( \
      ::nbdl::js::detail::get_handle(resolve) \
    , ::nbdl::js::detail::get_handle(reject) \
    , NBDL_DETAIL_JS_PROMISE_ARG_HANDLES(__VA_ARGS__)); \
  }

//
// NBDL_JS_PROMISE
//
#define NBDL_JS_PROMISE(code, ...) \
  ::boost::hana::make_tuple( \
    ::nbdl::hold(std::array<int, 0>{}) \
  , ::nbdl::js::detail::make_promise(NBDL_DETAIL_JS_PROMISE_ROUTINE(code)) \
  )

//
// NBDL_JS_PROMISE_WITH_CAPTURE - captures up to 8 values
//
#define NBDL_JS_PROMISE_WITH_CAPTURE(code, ...) \
  ::boost::hana::make_tuple( \
    ::nbdl::hold(::nbdl::js::detail::collect_js_vals(NBDL_DETAIL_JS_PROMISE_COLLECT_ARGS(__VA_ARGS__))) \
  , ::nbdl::js::detail::make_promise(NBDL_DETAIL_JS_PROMISE_ROUTINE_WITH_CAPTURE(code, __VA_ARGS__)) \
  )

//
// NBDL_JS_PROMISE_C_FUNC - captures up to 8 input values
//                          signature of c function shall be:
//                          fn(int h_resolve, int h_reject, int h_captures...);
//
#define NBDL_JS_PROMISE_C_FUNC(fn, ...) \
  ::boost::hana::make_tuple( \
    ::nbdl::hold(::nbdl::js::detail::collect_js_vals_reverse( \
        NBDL_DETAIL_JS_PROMISE_COLLECT_ARGS(__VA_ARGS__))) \
  , ::nbdl::js::detail::make_promise(NBDL_DETAIL_JS_PROMISE_ROUTINE_C_FUNC(fn, __VA_ARGS__)) \
  )

#endif
