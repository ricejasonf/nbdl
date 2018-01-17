//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_UI_HELPER_PARAMS_CONCAT_HPP
#define NBDL_UI_HELPER_PARAMS_CONCAT_HPP

#include <nbdl/detail/match_if.hpp>
#include <nbdl/detail/string_concat_view.hpp>
#include <nbdl/run_sync.hpp>
#include <nbdl/tap.hpp>
#include <nbdl/ui_helper.hpp>
#include <nbdl/ui_helper/match_branch.hpp>
#include <nbdl/ui_helper/match_path_spec.hpp>
#include <nbdl/ui_spec.hpp>

#include <boost/hana/at.hpp>
#include <boost/hana/find_if.hpp>
#include <boost/hana/front.hpp>
#include <boost/hana/or.hpp>
#include <boost/hana/second.hpp>
#include <boost/hana/transform.hpp>
#include <boost/hana/type.hpp>
#include <functional>
#include <type_traits>
#include <utility>

namespace nbdl::ui_helper
{
  namespace hana = boost::hana;

  struct params_concat_fn
  {
    template <typename ...Params, typename Store>
    ::nbdl::detail::string_concat_view operator()(mpdef::list<Params...>, Store const&) const;
  };

  constexpr params_concat_fn params_concat{};

  namespace detail
  {
    using ::nbdl::detail::string_concat_view;

    template <typename ParamSpec>
    constexpr auto param_to_string_view = [](auto const& /* store */)
      -> string_concat_view
    {
      return nbdl::detail::to_string_concat_view(ParamSpec{});
    };

    template <>
    constexpr auto param_to_string_view<char const*> = [](auto const& value)
    {
      static_assert(std::is_void<decltype(value)>::value, "Raw string literals are prohibited.");
    };

    template <typename ...PathNodes>
    constexpr auto param_to_string_view<nbdl::ui_spec::path_t<PathNodes...>> = [](auto const& store)
     -> string_concat_view
    {
      // If the path does not resolve to the Param then the result
      // will be an empty string_concat_view which, theoretically, should
      // never happen anyways.
      string_concat_view result{};

      match_path_spec(store, nbdl::ui_spec::path_t<PathNodes...>{}, [&](auto const& param)
      {
        result = nbdl::detail::to_string_concat_view(param);
      });

      return result;
    };

    // param_action branch

    template <typename ...T>
    constexpr auto param_to_string_view<param_action_t<ui_spec::branch_spec<T...>>>
      = [](auto const& store) -> string_concat_view
    {
      string_concat_view result;

      match_branch<ui_spec::branch_spec<T...>>(store, [&](auto, auto branch)
      {
        result = params_concat(branch, store);
      });

      return result;
    };
  }

  template <typename ...Params, typename Store>
  ::nbdl::detail::string_concat_view
  params_concat_fn::operator()(mpdef::list<Params...>, Store const& store) const
  {
    return ::nbdl::detail::make_string_concat_view(
      detail::param_to_string_view<Params>(store)...
    );
  }
}

#endif
