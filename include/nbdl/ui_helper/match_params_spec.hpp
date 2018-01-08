//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_UI_HELPER_MATCH_PARAMS_SPEC_HPP
#define NBDL_UI_HELPER_MATCH_PARAMS_SPEC_HPP

#include <nbdl/detail/default_constructible_lambda.hpp>
#include <nbdl/fwd/ui_helper/match_params_spec.hpp>
#include <nbdl/ui_helper.hpp>
#include <nbdl/ui_helper/match_branch.hpp>
#include <nbdl/ui_helper/match_path_spec.hpp>
#include <nbdl/ui_spec.hpp>

#include <boost/hana/core/is_a.hpp>
#include <boost/hana/core/tag_of.hpp>
#include <boost/hana/append.hpp>
#include <boost/hana/basic_tuple.hpp>
#include <boost/hana/concat.hpp>
#include <boost/hana/fuse.hpp>
#include <boost/mp11/list.hpp>
#include <functional>
#include <utility>

namespace nbdl::ui_helper::detail
{
  template <typename T>
  struct make_param_action_promise;

  template <typename ImplTag>
  struct match_param_action;

  template <>
  struct match_param_action<ui_spec::apply_tag>
  {
    template <typename Store, typename ApplyFn, typename ParamsSpec, typename Params, typename Fn>
    static auto apply(Store store
                    , ui_helper::param_action_t<ui_spec::apply_t<ApplyFn, ParamsSpec>>
                    , Params const& params
                    , Fn fn)
    {
      match_params_spec(store, ParamsSpec{}, [&](auto const& ...results)
      {
        constexpr auto apply = ::nbdl::detail::default_constructible_lambda<ApplyFn>{};
        auto result = apply(results...);
        fn(hana::append(params, std::cref(result)));
      });
    }
  };

  template <>
  struct match_param_action<ui_spec::branch_spec_tag>
  {
    template <typename Store, typename BranchSpec, typename Params, typename Fn>
    static auto apply(Store store
                    , ui_helper::param_action_t<BranchSpec>
                    , Params const& params
                    , Fn fn)
    {
      nbdl::ui_helper::match_branch<BranchSpec>(store, [&](auto, auto branch)
      {
        match_params_spec(store, branch, [&](auto const& ...results)
        {
          fn(hana::concat(params, hana::make_basic_tuple(results...)));
        });
      });
    }
  };

  template <typename Tag>
  struct match_param_spec_impl
  {
    template <typename Store, typename NotParamSpec, typename Params, typename Fn>
    static void apply(Store, NotParamSpec, Params const& params, Fn fn)
    {
      fn(hana::append(params, NotParamSpec{}));
    }
  };

  template <>
  struct match_param_spec_impl<ui_spec::path_tag>
  {
    template <typename Store, typename ParamSpec, typename Params, typename Fn>
    static void apply(Store store, ParamSpec, Params const& params, Fn fn)
    {
      nbdl::ui_helper::match_path_spec(store, ParamSpec{}, [&](auto const& result)
      {
        fn(hana::append(params, std::cref(result)));
      });
    }
  };

  template <>
  struct match_param_spec_impl<param_action_tag>
  {
    template <typename Store, typename ParamSpec, typename Params, typename Fn>
    static void apply(Store store, ParamSpec, Params const& params, Fn fn)
    {
      using Tag = hana::tag_of_t<mp_first<ParamSpec>>;
      match_param_action<Tag>::apply(store, ParamSpec{}, params, fn);
    }
  };
}

namespace nbdl::ui_helper
{
  namespace hana = boost::hana;
  using namespace boost::mp11;

  template <typename Store, typename ParamsSpec, typename Fn>
  void match_params_spec_fn::operator()(Store store, ParamsSpec, Fn&& fn) const
  {
    using FlatParamsSpec = mp_apply<
      mp_append
    , decltype(hana::transform(ParamsSpec{}, flatten_param_node))
    >;

    auto done = hana::fuse([&](auto ...x) { std::forward<Fn>(fn)(std::ref(x).get()...); });
    auto reducer = [store](auto spec, auto next)
    {
      return [=](auto const& params)
      {
        using Tag = hana::tag_of_t<decltype(spec)>;
        detail::match_param_spec_impl<Tag>::apply(store, decltype(spec){}, params, next);
      };
    };

    hana::fold_right(FlatParamsSpec{}, std::ref(done), reducer)(hana::make_basic_tuple());
  }
}

#endif
