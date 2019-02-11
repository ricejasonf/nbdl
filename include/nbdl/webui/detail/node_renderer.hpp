//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_WEBUI_DETAIL_NODE_RENDERER_HPP
#define NBDL_WEBUI_DETAIL_NODE_RENDERER_HPP

#include <boost/hana/string.hpp>
#include <dyno.hpp>
#include <emscripten/val.h>

namespace nbdl::webui::detail
{
  namespace hana = boost::hana;

  constexpr auto render_s = BOOST_HANA_STRING("render");
  constexpr auto update_s = BOOST_HANA_STRING("update");

  struct NodeRenderer : decltype(dyno::requires_(
    hana::make_pair(render_s, dyno::function<emscripten::val(emscripten::val)>)
  )) { };

  struct MutableNodeRenderer : decltype(dyno::requires_(
    hana::make_pair(render_s, dyno::function<emscripten::val(emscripten::val)>)
  , hana::make_pair(update_s, dyno::function<void(void)>)
  )) { };

  template <Typename T>
  constexpr auto dyno::default_concept_map<NodeRenderer, T> = dyno::make_concept_map(
    hana::make_pair(render_s, [](T const& self, emscripten::val parent) { self.render(parent); })
  );

  template <Typename T>
  constexpr auto dyno::default_concept_map<MutableNodeRenderer, T> = dyno::make_concept_map(
    hana::make_pair(render_s, [](T& self, emscripten::val parent) { self.render(parent); })
  , hana::make_pair(update_s, [](T& self)                         { self.update(); })
  );

  struct node_renderer
  {
    template <typename T>
    node_renderer(T x)
      : poly_{std::move(x)}
    { }

    emscripten::val render(emscripten::val parent)
    {
      return poly_.virtual_(render_s)(poly_, parent);
    }

  private:
    dyno::poly<NodeRenderer> poly_;
  };

  struct mutable_node_renderer
  {
    template <typename T>
    node_renderer(T x)
      : poly_{std::move(x)}
    { }

    emscripten::val render(emscripten::val parent)
    {
      return poly_.virtual_(render_s)(poly_, parent);
    }

    void update()
    {
      return poly_.virtual_(update_s)(poly_);
    }

    private:
      dyno::poly<MutableNodeRenderer> poly_;
  };
}
