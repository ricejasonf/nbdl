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
#include <nbdl/notify_state_change.hpp>
#include <nbdl/webui/detail/dom_manips.hpp>
#include <nbdl/webui/detail/flatten_spec.hpp>

#include <boost/hana/integral_constant.hpp>
#include <boost/hana/type.hpp>
#include <boost/hana/tuple.hpp>
#include <emscripten/val.h>
#include <utility>

namespace nbdl
{
  namespace hana = boost::hana;

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

      constexpr auto destroy_render_node = [](auto&& node)
        -> decltype(node.destroy())
      { node.destroy(); };

      constexpr auto can_destroy_render_node = hana::is_valid(destroy_render_node);

      template <typename ...>
      struct construct_render_node;

      template <typename Tag, typename ...Params>
      struct construct_render_node<action_fn<Tag, Params...>>
      {
        template <typename Store>
        static auto apply(Store store)
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

      template <typename Store>
      struct construct_pipe_helper_fn
      {
        Store store;

        template <typename ...Node>
        auto operator()(Node ...) const
        {
          return hana::make_tuple(detail::construct_render_node<Node>::apply(store)...);
        }
      };
    }


    template <typename Store, typename Spec, typename IsSpecFlat = hana::false_>
    struct renderer_impl
    {
      static constexpr auto get_fn_list()
      {
        if constexpr(IsSpecFlat{})
        {
          return Spec{};
        }
        else
        {
          return webui::detail::flatten_spec(Spec{});
        }
      }

      Store store;
      bool is_rendered;
      bool is_destroyed;

      using FnList = decltype(get_fn_list());
      using RenderPipe = decltype(hana::unpack(
        FnList{}
      , detail::construct_pipe_helper_fn<Store>{store}
      ));

      RenderPipe render_pipe;

      renderer_impl(Store s)
        : store(s)
        , is_rendered(false)
        , is_destroyed(false)
        , render_pipe(hana::unpack(
            FnList{}
          , detail::construct_pipe_helper_fn<Store>{store}
          ))
      { }

      template <typename Parent>
      void render(Parent&& parent)
      {
        is_rendered = true;
        emscripten::val current = std::forward<Parent&&>(parent);
        hana::for_each(render_pipe, [&](auto& fn)
        {
          current = fn(current);
        });
      }

      void update()
      {
        if (!is_rendered or is_destroyed)
        {
          return;
        }

        hana::for_each(render_pipe, [&](auto& x)
        {
          if constexpr(detail::can_update<decltype(x)>::value)
          {
            x.update();
          }
        });
      }

      void destroy()
      {
        if (!is_rendered or is_destroyed)
        {
          return;
        }
        is_destroyed = true;

        hana::for_each(render_pipe, [&](auto& x)
        {
          if constexpr(decltype(detail::can_destroy_render_node(x))::value)
          {
            detail::destroy_render_node(x);
          }
        });
      }
    };
  }

  template <typename RenderSpec>
  struct make_state_consumer_impl<webui::renderer<RenderSpec>>
  {
    template <typename Store, typename RootElement>
    static constexpr auto apply(Store&& store, RootElement&& root)
    {
      auto r = webui::renderer_impl<std::decay_t<Store>, RenderSpec>(std::forward<Store>(store));
      r.render(std::forward<RootElement>(root));
      return std::move(r);
    }
  };

  template <typename RenderSpec>
  struct notify_state_change_impl<webui::renderer<RenderSpec>>
  {
    template <typename Consumer, typename Path>
    static constexpr auto apply(Consumer& c, Path&&)
    {
      c.update();
    }
  };
}

namespace boost::hana
{
  template <typename Store, typename Spec>
  struct tag_of<nbdl::webui::renderer_impl<Store, Spec>>
  {
    using type = nbdl::webui::renderer<Spec>;
  };
}
#endif
