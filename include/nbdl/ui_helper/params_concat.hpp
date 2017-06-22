//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_UI_HELPER_PARAMS_CONCAT_HPP
#define NBDL_UI_HELPER_PARAMS_CONCAT_HPP

#include <nbdl/detail/string_concat.hpp>
#include <nbdl/ui_helper/path.hpp>
#include <nbdl/ui_spec.hpp>

#include <boost/hana/core/is_a.hpp>
#include <experimental/string_view>
#include <functional>
#include <utility>

namespace nbdl::ui_helper
{
  namespace hana = boost::hana;
  using string_view = std::experimental::string_view;

  struct params_concat_fn
  {
    template <typename ...Params, typename Store>
    std::string operator()(mpdef::list<Params...>, Store const&) const;
  };

  constexpr params_concat_fn params_concat{};

  namespace detail
  {
    template <typename ParamSpec>
    constexpr auto param_to_string_view = [](auto const& /* store */)
      -> string_view
    {
      return nbdl::detail::to_string_view(ParamSpec{});
    };

    template <typename ...PathNodes>
    constexpr auto param_to_string_view<nbdl::ui_spec::path_t<PathNodes...>> = [](auto const& store)
     -> string_view
    {
      // If the path does not resolve to the Param then the result
      // will be an empty string_view which, theoretically, should
      // never happen anyways.
      string_view result;

      path(nbdl::ui_spec::path_t<PathNodes...>{}, store, [&](auto const& param)
      {
        result = nbdl::detail::to_string_view(param);
      });

      return result;
    };
  }

  template <typename ...Params, typename Store>
  std::string params_concat_fn::operator()(mpdef::list<Params...>, Store const& store) const
  {
    return nbdl::detail::string_concat.with_views(std::array<string_view, sizeof...(Params)>{{
      detail::param_to_string_view<Params>(store)...
    }});
  }
}

#endif
