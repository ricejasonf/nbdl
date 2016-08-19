//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_WEBUI_RENDERER_HPP
#define NBDL_WEBUI_RENDERER_HPP

#include <nbdl/fwd/webui/renderer.hpp>
#include <emscripten/val.h>

#include<utility>

namespace nbdl
{
  template <typename RenderImpl, typename RenderSpec>
  struct make_state_consumer_impl<webui::renderer<RenderImpl, RenderSpec>>
  {
    template <typename PushApi>
    static constexpr auto apply(PushApi&& p)
    {
      return webui::renderer_impl<PushApi, RenderSpec>(std::forward<PushApi>(p));
    }
  };

  namespace webui
  {
    template <typename PushApi, typename RenderImpl, typename RenderSpec>
    class renderer_impl
    {
      using RenderFns = decltype(detail::flatten_spec(nbdl::make_def(hana::type_c<RenderSpec>)));
      // State is the return value of each element in RenderFns
      using State = decltype(
        hana::remove_if(hana::transform(RenderActions{}, hana::typeid_), hana::traits::is_empty)
      );
      // TODO create a map of paths to indices to call the render fn
      // It should get a run of functions up to the corresponding 'end'
      // function.
      // Really need traits for begin/end

      PushApi push_api;

      void render()
      {
        // nodes are always pairs (tag, stuff)
        hana::unpack(RenderSpec{}, [](auto const& node)
        {
          if constexpr(decltype(hana::equal(hana::first(node), tag::element))::value)
          {
            constexpr auto child_nodes = hana::second(node);
            hana::flatten(
              mpdef::make_list(detail::start_element(hana::at_c<0>(child_nodes))),
              hana::unpack(
                hana::filter(child_nodes, hana::compose(hana::equal.to(tag::attribute), hana::first)),
                mpdef::make_list ^hana::on^ hana::attribute
              ),
              hana::unpack(mpdef::find_by_tag(node, tag::children), this_function)
            );
          }
        });
      }

      public:

      renderer_impl(PushApi p)
        : push_api(std::move(p))
      {
        render();
      }
    }; 
  }
}
#endif
