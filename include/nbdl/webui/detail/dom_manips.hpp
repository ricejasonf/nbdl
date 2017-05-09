//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_WEBUI_DETAIL_DOM_MANIPS_HPP
#define NBDL_WEBUI_DETAIL_DOM_MANIPS_HPP

#include <nbdl/catch.hpp>
#include <nbdl/detail/string_concat.hpp>
#include <nbdl/fwd/webui/detail/dom_manips.hpp>
#include <nbdl/params_promise.hpp>
#include <nbdl/path_promise.hpp>
#include <nbdl/run_sync.hpp>
#include <nbdl/tap.hpp>
#include <nbdl/ui_spec.hpp>
#include <nbdl/webui/html.hpp>
#include <nbdl/webui/renderer.hpp>

#include <emscripten/val.h>
#include <utility>

namespace nbdl::webui::detail
{
  template <typename Store>
  struct make_nested_renderer_impl_type_from_pair_fn
  {
    template <typename Key, typename FnList>
    constexpr auto operator()(mpdef::pair<Key, FnList>) const
    {
      return hana::type_c<renderer_impl<std::reference_wrapper<Store>, FnList, hana::true_>>;
    }
  };

  template <typename HtmlTagName>
  struct action_fn<begin, html::tag::element_t, HtmlTagName>
  {
    template <typename Resolver, typename ParentElement>
    void operator()(Resolver& resolve, ParentElement&& p) const
    {
      auto el = emscripten::val::global("document").template
        call<emscripten::val>("createElement", emscripten::val(hana::to<char const*>(HtmlTagName{})));
      std::forward<ParentElement>(p).template
        call<void>("appendChild", el);
      resolve(std::move(el));
    }
  };

  template <typename HtmlTagName>
  struct action_fn<end, html::tag::element_t, HtmlTagName>
  {
    template <typename Resolver, typename ParentElement>
    void operator()(Resolver& resolve, ParentElement&& p) const
    {
      resolve(std::forward<ParentElement>(p)["parentNode"]);
    }
  };

  // For one string constant
  template <typename AttributeName, char ...Cs>
  struct action_fn<html::tag::attribute_t, AttributeName, mpdef::list<hana::string<Cs...>>>
  {
    template <typename Resolver, typename ParentElement>
    void operator()(Resolver& resolve, ParentElement&& p) const
    {
      p.template call<void>(
        "setAttribute"
      , emscripten::val(hana::to<char const*>(AttributeName{}))
      , emscripten::val(hana::to<char const*>(hana::string<Cs...>{}))
      );
      resolve(std::forward<ParentElement>(p));
    }
  };

  template <typename AttributeName, typename StringParams>
  struct action_fn<html::tag::attribute_t, AttributeName, StringParams>
  {
    action_fn() = delete;
  };

  template <typename Store, typename AttributeName, typename StringParams>
  struct mut_action_fn<html::tag::attribute_t, Store, AttributeName, StringParams>
  {
    Store const& store;
    emscripten::val el;

    mut_action_fn(Store const& s)
      : store(s)
      , el(emscripten::val::undefined())
    { }

    template <typename Resolver, typename ParentElement>
    void operator()(Resolver& resolve, ParentElement&& p) const
    {
      nbdl::run_sync(
        nbdl::pipe(
          nbdl::params_promise(StringParams{})
        , nbdl::detail::string_concat
        , [&](auto const& text_value)
          {
            p.template call<void>(
              "setAttribute"
            , emscripten::val(hana::to<char const*>(AttributeName{}))
            , emscripten::val(text_value)
            );
            resolve(std::forward<ParentElement>(p));
          }
        )
      , store);
    }
  };

  template <typename String>
  struct action_fn<html::tag::text_node_t, String>
  {
    template <typename Resolver, typename ParentElement>
    void operator()(Resolver& resolve, ParentElement&& p) const
    {
      auto el = emscripten::val::global("document").template
        call<emscripten::val>("createTextNode", emscripten::val(hana::to<char const*>(String{})));
      p.template call<void>("appendChild", el);
      resolve(std::forward<ParentElement>(p));
    }
  };

  template <typename ...PathNodes>
  struct action_fn<html::tag::text_node_t, ui_spec::path_t<PathNodes...>>
  {
    action_fn() = delete;
  };

  template <typename Store, typename ...PathNodes>
  struct mut_action_fn<html::tag::text_node_t, Store, ui_spec::path_t<PathNodes...>>
  {
    Store const& store;
    emscripten::val el;
    emscripten::val parent_el;

    mut_action_fn(Store const& s)
      : store(s)
      , el(emscripten::val::undefined())
      , parent_el(emscripten::val::undefined())
    { }

    template <typename Resolver, typename ParentElement>
    void operator()(Resolver& resolve, ParentElement&& p)
    {
      nbdl::run_sync(
        nbdl::pipe(
          nbdl::path_promise(ui_spec::path_t<PathNodes...>{})
        , [&](auto const& value)
          {
            el = emscripten::val::global("document").template
              call<emscripten::val>("createTextNode", emscripten::val(value));
            p.template call<void>("appendChild", el);
            parent_el = p;
            resolve(std::forward<ParentElement>(p));
          }
        )
      , store
      );
    }

    void update()
    {
      nbdl::run_sync(
        nbdl::pipe(
          nbdl::path_promise(ui_spec::path_t<PathNodes...>{})
        , [&](auto const& value)
          {
            auto new_el = emscripten::val::global("document").template
              call<emscripten::val>("createTextNode", emscripten::val(value));
            parent_el.template call<void>("replaceChild", new_el, el);
            el = new_el;
          }
        )
      , store
      );
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
        , make_nested_renderer_impl_type_from_pair_fn<Store>{}
        )
      , hana::template_<hana::tuple>
      )
    )::type;

    Store& store;
    emscripten::val parent_el;
    emscripten::val container_el;
    int branch_id;
    Renderers renderers;

    mut_action_fn(Store& s)
      : store(s)
      , parent_el(emscripten::val::undefined())
      , container_el(emscripten::val::undefined())
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
            if (branch_id != -1)
            {
              container_el.template call<void>("removeChild", container_el["firstChild"]);
            }
            branch_id = hana::value(index);
            renderers[index].render(container_el);
          }
        })
      , nbdl::catch_(nbdl::noop)
      ), store);
    }

    template <typename Resolver, typename ParentElement>
    void operator()(Resolver& resolve, ParentElement&& p)
    {
      parent_el = p;
      container_el = emscripten::val::global("document").template
        call<emscripten::val>("createElement", emscripten::val("span"));
      update();
      parent_el.template call<void>("appendChild", container_el);
      resolve(std::forward<ParentElement>(p));
    }
  };

  template <typename String>
  struct action_fn<html::tag::unsafe_set_inner_html_t, String>
  {
    template <typename Resolver, typename ParentElement>
    void operator()(Resolver& resolve, ParentElement&& p) const
    {
      p.set("innerHTML", emscripten::val(hana::to<char const*>(String{})));
      resolve(std::forward<ParentElement>(p));
    }
  };
}
#endif
