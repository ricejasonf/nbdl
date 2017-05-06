//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_UI_VIEW_RENDERER_HPP
#define NBDL_UI_VIEW_RENDERER_HPP

#include <nbdl/fwd/ui_view/renderer.hpp>
#include <nbdl/make_state_consumer.hpp>
#include <nbdl/notify_state_change.hpp>
#include <nbdl/promise.hpp>
#include <nbdl/ui_view/detail/flatten_spec.hpp>
#include <nbdl/ui_view/detail/manips.hpp>

#include <functional>
#include <utility>

namespace nbdl
{
  // TODO This is an exact duplication with webui::renderer.
  namespace ui_view
  {
    namespace detail
    {
      template <typename RenderAction>
      struct can_update : std::false_type { };

      template <typename ...Params>
      struct can_update<mut_action_fn<Params...>&>
        : std::true_type
      { };


      template <typename ...>
      struct construct_render_node;

      template <typename Tag, typename ...Params>
      struct construct_render_node<action_fn<Tag, Params...>>
      {
        template <typename Store>
        static auto apply(Store& store)
        {
          if constexpr(std::is_default_constructible<action_fn<Tag, Params...>>::value)
          {
            return action_fn<Tag, Params...>{};
          }
          else
          {
            return mut_action_fn<Tag, Store, Params...>(store);
          }
        }
      };
    }

    template <typename RenderSpec>
    struct renderer { };

    template <typename ...>
    struct renderer_impl;

    template <typename Store, typename ...Node>
    struct renderer_impl<Store, mpdef::list<Node...>>
    {
      using RenderPipe = decltype(
        nbdl::pipe(detail::construct_render_node<Node>::apply(std::declval<Store&>())...)
      );

      Store store;
      RenderPipe render_pipe;

      renderer_impl(Store s)
        : store(std::move(s))
        , render_pipe(nbdl::pipe(detail::construct_render_node<Node>::apply(store)...))
      { }

      template <typename Parent>
      void render(Parent&& parent)
      {
        nbdl::run_sync(
          hana::transform(render_pipe, [](auto& x) { return nbdl::promise(std::ref(x)); })
        , std::forward<Parent>(parent)
        );
      }

      void update()
      {
        hana::for_each(render_pipe, [&](auto& x)
        {
          if constexpr(detail::can_update<decltype(x)>::value)
          {
            x.update();
          }
        });
      }
    };

    template <typename Store, typename RenderSpec>
    auto make_renderer(Store&& store, RenderSpec)
    {
      using FnList = decltype(ui_view::detail::flatten_spec(RenderSpec{}));
      return ui_view::renderer_impl<Store, FnList>(std::forward<Store>(store));
    }
  }

  template <typename RenderSpec>
  struct make_state_consumer_impl<ui_view::renderer<RenderSpec>>
  {
    template <typename Store>
    static constexpr auto apply(Store&& store)
    {
      auto r = ui_view::make_renderer<RenderSpec>();
      r.render(std::forward<Store>(store));
      return std::move(r);
    }
  };

  template <typename RenderSpec>
  struct notify_state_change_impl<ui_view::renderer<RenderSpec>>
  {
    template <typename Consumer, typename Path>
    static constexpr auto apply(Consumer&, Path&&)
    {
      // TODO something!
    }
  };
}
#endif

