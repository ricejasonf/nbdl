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
#include <nbdl/ui_helper/path.hpp>
#include <nbdl/ui_spec.hpp>

#include <boost/hana/at.hpp>
#include <boost/hana/find_if.hpp>
#include <boost/hana/front.hpp>
#include <boost/hana/or.hpp>
#include <boost/hana/second.hpp>
#include <boost/hana/transform.hpp>
#include <boost/hana/type.hpp>
#include <functional>
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

    template <typename ...PathNodes>
    constexpr auto param_to_string_view<nbdl::ui_spec::path_t<PathNodes...>> = [](auto const& store)
     -> string_concat_view
    {
      // If the path does not resolve to the Param then the result
      // will be an empty string_concat_view which, theoretically, should
      // never happen anyways.
      string_concat_view result;

      path(nbdl::ui_spec::path_t<PathNodes...>{}, store, [&](auto const& param)
      {
        result = nbdl::detail::to_string_concat_view(param);
      });

      return result;
    };

    // param_action match

    template <typename T>
    struct match_branch_pred_fn
    {
      template <typename U, typename Spec>
      constexpr auto operator()(mpdef::pair<U, Spec>) const
      {
        return hana::or_(
          hana::type_c<T>    == U{}
        , hana::type_c<void> == U{}
        );
      }
    };

    template <typename PathSpec, typename Branches>
    constexpr auto param_to_string_view<param_action_t<
      ui_spec::match_tag, PathSpec, Branches>> = [](auto const& store) -> string_concat_view
    {
      string_concat_view result;

      path(PathSpec{}, store, [&](auto const& value)
      {
        using T = typename decltype(hana::typeid_(value))::type;
        constexpr auto params_pair = hana::find_if(Branches{}, match_branch_pred_fn<T>{});
        static_assert(
          hana::is_just(params_pair)
        , "`ui_spec::match` has no matching branch"
        );

        result = params_concat(hana::second(*params_pair), store);
      });

      return result;
    };

    // param_action match_if

    template <typename PathSpec, typename Branches>
    constexpr auto param_to_string_view<param_action_t<
      ui_spec::match_if_tag, PathSpec, Branches>> = [](auto const& store) -> string_concat_view
    {
      string_concat_view result;

      path(PathSpec{}, store, [&](auto const& param_at_path)
      {
        auto pipe = nbdl::detail::promise_join(
          nbdl::detail::match_if(hana::transform(Branches{}, hana::first))
        , nbdl::detail::promise_join(
            nbdl::tap([&](auto index)
            {
              using Param = typename decltype(
                hana::typeid_(hana::second(hana::at(Branches{}, index)))
              )::type;
              result = params_concat(Param{}, store);
            })
          , nbdl::detail::promise_end_sync{}
          )
        );
        pipe.resolve(param_at_path);
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
