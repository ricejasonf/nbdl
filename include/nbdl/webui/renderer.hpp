//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_WEBUI_RENDERER_HPP
#define NBDL_WEBUI_RENDERER_HPP

#include <nbdl/fwd/webui/renderer.hpp>
#include <nbdl/make_state_consumer.hpp>
#include <nbdl/promise.hpp>
#include <nbdl/notify_state_change.hpp>
#include <nbdl/webui/detail/dom_manips.hpp>
#include <nbdl/webui/detail/flatten_spec.hpp>

#include <emscripten/val.h>
#include <utility>

namespace nbdl
{
  namespace webui
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

      template <typename Fn, typename ...Params>
      struct construct_render_node<bind_impl<Fn, Params...>>
      {
        template <typename Store>
        static auto apply(Store& store)
        {
          if constexpr(hana::is_nothing(
            hana::index_if(mpdef::make_list(Params{}...), hana::is_a<path_tag>)
          ))
          {
            return bind_impl<Fn, Params...>{};
          }
          else
          {
            return mut_bind_impl<Fn, Params...>(store);
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

      RenderPipe render_pipe;

      renderer_impl(Store& store)
        : render_pipe(nbdl::pipe(detail::construct_render_node<Node>::apply(store)...))
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
    auto make_renderer(Store& store, RenderSpec)
    {
      using FnList = decltype(webui::detail::flatten_spec(RenderSpec{}));
      return webui::renderer_impl<Store, FnList>(store);
    }
  }

  template <typename RenderSpec>
  struct make_state_consumer_impl<webui::renderer<RenderSpec>>
  {
    template <typename Store>
    static constexpr auto apply(Store&& store)
    {
      auto r = webui::make_renderer<RenderSpec>();
      r.render(std::forward<Store>(store));
      return std::move(r);
    }
  };

  template <typename RenderSpec>
  struct notify_state_change_impl<webui::renderer<RenderSpec>>
  {
    template <typename Consumer, typename Path>
    static constexpr auto apply(Consumer&, Path&&)
    {
      // TODO something!
    }
  };
}
#endif
