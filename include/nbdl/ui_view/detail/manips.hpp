//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_UI_VIEW_DETAIL_MANIPS_HPP
#define NBDL_UI_VIEW_DETAIL_MANIPS_HPP

#ifndef __OBJC__
static_assert(false, "UIView manipulations require an Objective-C++ compiler");
#endif

#import <UIKit/UIKit.h>

#include <nbdl/catch.hpp>
#include <nbdl/fwd/ui_view/detail/manips.hpp>
#include <nbdl/path_promise.hpp>
#include <nbdl/promise.hpp>
#include <nbdl/run_sync.hpp>
#include <nbdl/tap.hpp>
#include <nbdl/ui_spec.hpp>
#include <nbdl/ui_view/spec.hpp>
#include <nbdl/ui_view/renderer.hpp>

#include <utility>

namespace nbdl::ui_view::detail
{
  template <typename View, typename InitFn>
  struct action_fn<begin, spec::tag::view_t, View, InitFn>
  {
    View* view;

    action_fn()
      : view(nil)
    { }

    template <typename Resolver, typename Superview>
    void operator()(Resolver& resolve, Superview* super_view) const
    {
      if (view == nil)
      {
        View* view = [View alloc];
        InitFn{}(view);
      }

      [super_view addSubview: view];
      resolve(view);
    }
  };

  template <typename View>
  struct action_fn<end, spec::tag::view_t, View>
  {
    template <typename Resolver>
    void operator()(Resolver& resolve, View* view) const
    {
      resolve(view.super_view);
    }
  };

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

  template <typename ...>
  struct construct_branch_renderers;

  template <typename ...X>
  struct construct_branch_renderers<hana::tuple<X...>>
  {
    template <typename Store>
    static auto apply(Store& store)
    {
      return hana::make_tuple(X(store)...);
    }
  };

  template <typename PathSpec, typename Branches>
  struct action_fn<ui_spec::match_tag, PathSpec, Branches>
  {
    action_fn() = delete;
  };

  //  Branches - list<pair<T, list<action...>>...>
  template <typename Store, typename PathSpec, typename Branches>
  struct mut_action_fn<ui_spec::match_tag, Store, PathSpec, Branches>
  {
    using Renderers = typename decltype(
      hana::unpack(
        hana::transform(
          Branches{}
        , hana::compose(
            hana::partial(hana::template_<renderer_impl>, hana::type_c<Store>)
          , hana::typeid_
          , hana::second
          )
        )
      , hana::template_<hana::tuple>
      )
    )::type;

    Store& store;
    UIView* container_el;
    int branch_id;
    Renderers renderers;

    mut_action_fn(Store& s)
      : store(s)
      , container_el([[UIView alloc] initWithFrame: CGRectMake(0, 0, 100, 100)])
      , branch_id(-1)
      , renderers(construct_branch_renderers<Renderers>::apply(s))
    { }

    void update()
    {
      nbdl::run_sync(nbdl::pipe(
        nbdl::path_promise(PathSpec{})
      , [](auto const& value)
        {
          using T = typename decltype(hana::typeid_(value))::type;
          return *hana::index_if(Branches{}, match_branch_pred_fn<T>{});
        }
      , nbdl::tap([&](auto index)
        {
          if (hana::value(index) != branch_id)
          {
            // Something changed! :D
            for (UIView* view in [container_el subviews])
            {
              [view removeFromSuperview];
            }
            branch_id = hana::value(index);
            renderers[index].render(container_el);
          }
        })
      , nbdl::catch_(nbdl::noop)
      ), store);
    }

    template <typename Resolver, typename Superview>
    void operator()(Resolver& resolve, Superview* p)
    {
      update();
      [p addSubView: container_el];
      resolve(p);
    }
  };
}
#endif
