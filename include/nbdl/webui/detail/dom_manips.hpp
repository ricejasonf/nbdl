//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_WEBUI_DETAIL_DOM_MANIPS_HPP
#define NBDL_WEBUI_DETAIL_DOM_MANIPS_HPP

#include <mpdef/list.hpp>
#include <nbdl/catch.hpp>
#include <nbdl/detail/default_constructible_lambda.hpp>
#include <nbdl/detail/match_if.hpp>
#include <nbdl/fwd/webui/detail/dom_manips.hpp>
#include <nbdl/store_range.hpp>
#include <nbdl/ui_helper/params_concat.hpp>
#include <nbdl/ui_helper/match_path_spec.hpp>
#include <nbdl/ui_spec.hpp>
#include <nbdl/webui/detail/dom_event_listener.hpp>
#include <nbdl/webui/detail/event_receiver.hpp>
#include <nbdl/webui/html.hpp>
#include <nbdl/webui/renderer.hpp>

#include <boost/hana/basic_tuple.hpp>
#include <boost/hana/core/is_a.hpp>
#include <boost/hana/core/to.hpp>
#include <boost/hana/first.hpp>
#include <boost/hana/index_if.hpp>
#include <boost/hana/or.hpp>
#include <boost/hana/string.hpp>
#include <boost/hana/transform.hpp>
#include <boost/hana/type.hpp>
#include <boost/hana/unpack.hpp>
#include <boost/hana/value.hpp>
#include <boost/mp11/algorithm.hpp>
#include <emscripten/val.h>
#include <utility>
#include <vector>

namespace nbdl::webui::detail
{
  namespace hana = boost::hana;
  using namespace hana::literals;
  using namespace boost::mp11;
  using js_val = nbdl::detail::js_val;

  // converts a Constant or string to js val
  template <typename T>
  emscripten::val to_json_val(T t)
  {
    if constexpr (hana::is_a<hana::string_tag, T>)
    {
      return emscripten::val(hana::to<char const*>(t));
    }
    else
    {
      static_assert(
        not hana::is_a<char const*, T>
      , "String literals not allowed. Please use constexpr alternative like hana::string."
      );
      return emscripten::val(hana::value(t));
    }
  }

  template <typename T>
  emscripten::val to_text_val(T&& t)
  {
    // TODO This should be generic and it should be possible to specialize for user types
    if constexpr (std::is_integral<std::decay_t<T>>::value)
    {
      return emscripten::val(std::to_string(t));
    }
    else
    {
      return emscripten::val(std::string{std::forward<T>(t)});
    }
  }

  //
  // make_nested_renderer_tuple
  //

  template <typename Store, typename List>
  struct make_nested_renderer_tuple_impl;

  template <typename Store, typename ...FnList>
  struct make_nested_renderer_tuple_impl<Store, mpdef::list<FnList...>>
  {
    using type = hana::basic_tuple<renderer_impl<Store, FnList, hana::true_>...>;
  };

  template <typename Store, typename List>
  using make_nested_renderer_tuple = typename make_nested_renderer_tuple_impl<Store, List>::type;

  //
  // action_fn specializations
  //

  template <typename HtmlTagName>
  struct action_fn<begin, html::tag::element_t, HtmlTagName>
  {
    template <typename ParentElement>
    decltype(auto) operator()(ParentElement&& p) const
    {
      auto el = emscripten::val::global("document").template
        call<emscripten::val>("createElement", to_json_val(HtmlTagName{}));
      std::forward<ParentElement>(p).template
        call<void>("appendChild", el);
      return el;
    }
  };

  template <typename HtmlTagName>
  struct action_fn<end, html::tag::element_t, HtmlTagName>
  {
    template <typename ParentElement>
    decltype(auto) operator()(ParentElement&& p) const
    {
      return std::forward<ParentElement>(p)["parentNode"];
    }
  };

  //
  // attribute
  //

  // For one string constant
  template <typename AttributeName, char ...Cs>
  struct action_fn<html::tag::attribute_t, AttributeName, mpdef::list<hana::string<Cs...>>>
  {
    template <typename ParentElement>
    decltype(auto) operator()(ParentElement&& p) const
    {
      p.template call<void>(
        "setAttribute"
      , to_json_val(AttributeName{})
      , to_json_val(hana::string<Cs...>{})
      );
      return std::forward<ParentElement>(p);
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
    Store store;
    emscripten::val el;

    mut_action_fn(Store s)
      : store(s)
      , el(emscripten::val::undefined())
    { }

    template <typename ParentElement>
    decltype(auto) operator()(ParentElement&& p)
    {
      el = p;
      update();
      return std::forward<ParentElement>(p);
    }

    void update()
    {
      std::string text_value = ui_helper::params_concat(StringParams{}, store)
        .to_string();
      el.template call<void>(
        "setAttribute"
      , to_json_val(AttributeName{})
      , emscripten::val(std::move(text_value))
      );
    }
  };

  //
  // set_property
  //

  // For one string constant
  template <typename AttributeName, char ...Cs>
  struct action_fn<html::tag::set_property_t, AttributeName, mpdef::list<hana::string<Cs...>>>
  {
    template <typename ParentElement>
    decltype(auto) operator()(ParentElement&& p) const
    {
      p.set(
        to_json_val(AttributeName{})
      , to_json_val(hana::string<Cs...>{})
      );
      return std::forward<ParentElement>(p);
    }
  };

  template <typename AttributeName, typename StringParams>
  struct action_fn<html::tag::set_property_t, AttributeName, StringParams>
  {
    action_fn() = delete;
  };

  template <typename Store, typename AttributeName, typename StringParams>
  struct mut_action_fn<html::tag::set_property_t, Store, AttributeName, StringParams>
  {
    Store store;
    emscripten::val el;

    mut_action_fn(Store s)
      : store(s)
      , el(emscripten::val::undefined())
    { }

    template <typename ParentElement>
    decltype(auto) operator()(ParentElement&& p)
    {
      el = p;
      update();
      return std::forward<ParentElement>(p);
    }

    void update()
    {
      std::string text_value = ui_helper::params_concat(StringParams{}, store)
        .to_string();
      el.set(
        to_json_val(AttributeName{})
      , emscripten::val(std::move(text_value))
      );
    }
  };

  //
  // text_node
  //

  template <typename Value>
  struct action_fn<html::tag::text_node_t, Value>
  {
    template <typename ParentElement>
    decltype(auto) operator()(ParentElement&& p) const
    {
      auto el = emscripten::val::global("document").template
        call<emscripten::val>("createTextNode", to_json_val(Value{}));
      p.template call<void>("appendChild", el);
      return std::forward<ParentElement>(p);
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
    Store store;
    emscripten::val el;
    emscripten::val parent_el;

    mut_action_fn(Store s)
      : store(s)
      , el(init_el())
      , parent_el(emscripten::val::undefined())
    { }

    emscripten::val init_el()
    {
      return emscripten::val::global("document").template
        call<emscripten::val>("createTextNode", emscripten::val(""));
    }

    template <typename ParentElement>
    decltype(auto) operator()(ParentElement&& p)
    {
      p.template call<void>("appendChild", el);
      parent_el = p;

      nbdl::ui_helper::match_path_spec(store, ui_spec::path_t<PathNodes...>{}, [&](auto const& value)
      {
        el.set("nodeValue", to_text_val(value));
      });

      return std::forward<ParentElement>(p);
    }

    void update()
    {
      nbdl::ui_helper::match_path_spec(store, ui_spec::path_t<PathNodes...>{}, [&](auto const& value)
      {
        el.set("nodeValue", to_text_val(value));
      });
    }
  };

  template <typename>
  struct construct_branch_renderers;

  template <typename ...X>
  struct construct_branch_renderers<hana::basic_tuple<X...>>
  {
    template <typename Store>
    static auto apply(Store store)
    {
      return hana::make_basic_tuple(X(store)...);
    }
  };

  /*
   * branch_spec
   */

  template <typename Tag, typename PathSpec, typename Branches>
  struct action_fn<ui_spec::branch_spec<Tag, PathSpec, Branches>>
  {
    action_fn() = delete;
  };

  template <typename NodeContainers, std::size_t ...i>
  NodeContainers make_branch_node_containers(std::index_sequence<i...>)
  {
    return NodeContainers{{(static_cast<void>(i), emscripten::val::undefined())...}};
  }

  //  Branches - list<pair<T, list<action...>>...>
  template <typename Store, typename Tag, typename PathSpec, typename Branches>
  struct mut_action_fn<ui_spec::branch_spec<Tag, PathSpec, Branches>, Store>
  {
    using BranchSpec = ui_spec::branch_spec<Tag, PathSpec, Branches>;
    using FnLists = mp_transform<mp_second, mp_third<BranchSpec>>;
    using Renderers = make_nested_renderer_tuple<Store, FnLists>;
    using NodeContainers = std::array<emscripten::val, mp_size<FnLists>::value>;

    Store store;
    emscripten::val parent_el;
    std::size_t branch_id;
    NodeContainers node_containers;
    Renderers renderers;

    mut_action_fn(Store s)
      : store(s)
      , parent_el(emscripten::val::undefined())
      , branch_id(-1)
      , node_containers(make_branch_node_containers<NodeContainers>(
          std::make_index_sequence<mp_size<FnLists>::value>{}
        ))
      , renderers(construct_branch_renderers<Renderers>::apply(s))
    { }

    void update_branch()
    {
      // FIXME
      // How do we know if the child is a branch and its element changes??
      // Options:
      // 1. We could flatten composed branches to prevent nesting.
      // 2. Nested renderers could have a get_element function that refers
      //    to this node's current element.

      std::size_t index = nbdl::ui_helper::get_branch_index<BranchSpec>(store);
      if (index != branch_id)
      {
        // Something changed! :D
        if (branch_id != static_cast<std::size_t>(-1))
        {
          parent_el.template call<void>(
            "replaceChild"
          , node_containers[index]
          , node_containers[branch_id]
          );
        }
        else
        {
          parent_el.template call<void>("appendChild", node_containers[index]);
        }

      }
      branch_id = index;
    }

    void update()
    {
      update_branch();

      // update children
      hana::for_each(renderers, [](auto& x)
      {
        x.update();
      });
    }

    void destroy()
    {
      hana::for_each(renderers, [](auto& x) { x.destroy(); });
    }

    template <typename ParentElement>
    decltype(auto) operator()(ParentElement&& p)
    {
      parent_el = p;

      auto temp_span = emscripten::val::global("document").template
        call<emscripten::val>("createElement", emscripten::val("span"));

      constexpr auto range = hana::make_range(hana::size_c<0>, mp_size<FnLists>{}); 
      hana::for_each(range, [&](auto index)
      {
        hana::at(renderers, index).render(temp_span);
        node_containers[decltype(index)::value] = temp_span["lastChild"];
      });

      update_branch();
      return std::forward<ParentElement>(p);
    }
  };

  /*
   * unsafe_set_inner_html
   */

  template <typename String>
  struct action_fn<html::tag::unsafe_set_inner_html_t, String>
  {
    template <typename ParentElement>
    decltype(auto) operator()(ParentElement&& p) const
    {
      p.set("innerHTML", to_json_val(String{}));
      return std::forward<ParentElement>(p);
    }
  };

  /*
   * event_attribute
   */
  template <typename AttributeName, typename Handler, typename ...Params>
  struct action_fn<html::tag::event_attribute_t, AttributeName, Handler, Params...>
  {
    action_fn() = delete;
  };

  template <typename Store, typename AttributeName, typename Handler, typename ...Params>
  struct mut_action_fn<html::tag::event_attribute_t, Store, AttributeName, Handler, Params...>
  {
    using ReceiverImpl = event_receiver_impl<Store, Handler, Params...>;

    Store store;
    std::unique_ptr<dom_event_listener> event_listener;

    mut_action_fn(Store s)
      : store(s)
      , event_listener()
    { }

    template <typename ParentElement>
    decltype(auto) operator()(ParentElement&& p)
    {
      event_listener = make_dom_event_listener(
        ReceiverImpl(store)
      , p
      , to_json_val(AttributeName{})
      );

      return std::forward<ParentElement>(p);
    }

    void update()
    { }
  };

  /*
   * add_class_if
   */
  template <typename ClassName, typename PathSpec, typename Pred>
  struct action_fn<html::tag::add_class_if_t, ClassName, PathSpec, Pred>
  {
    action_fn() = delete;
  };

  template <typename Store, typename ClassName, typename PathSpec, typename Pred_>
  struct mut_action_fn<html::tag::add_class_if_t, Store, ClassName, PathSpec, Pred_>
  {
    Store store;
    emscripten::val el;
    using Pred = nbdl::detail::default_constructible_lambda<Pred_>;

    mut_action_fn(Store s)
      : store(s)
      , el(emscripten::val::undefined())
    { }

    template <typename ParentElement>
    decltype(auto) operator()(ParentElement&& p)
    {
      el = p;
      update();
      return std::forward<ParentElement>(p);
    }

    void update()
    {
      ui_helper::match_path_spec(store, PathSpec{}, [&](auto const& value)
      {
        el["classList"].template call<void>(
          Pred{}(value) ? "add" : "remove"
        , to_json_val(ClassName{})
        );
      });
    }
  };

  /*
   * add_class_when
   */
  template <typename T, typename ClassName, typename PathSpec>
  struct action_fn<html::tag::add_class_when_t<T>, ClassName, PathSpec>
  {
    action_fn() = delete;
  };

  template <typename Store, typename T, typename ClassName, typename PathSpec>
  struct mut_action_fn<html::tag::add_class_when_t<T>, Store, ClassName, PathSpec>
  {
    Store store;
    emscripten::val el;

    mut_action_fn(Store s)
      : store(s)
      , el(emscripten::val::undefined())
    { }

    template <typename ParentElement>
    decltype(auto) operator()(ParentElement&& p)
    {
      el = p;
      update();
      return std::forward<ParentElement>(p);
    }

    void update()
    {
      bool result = false;

      ui_helper::match_path_spec(store, PathSpec{}, nbdl::match_when<T>([&](auto const&)
      {
        result = true;
      }));

      el["classList"].template call<void>(
        result ? "add" : "remove"
      , to_json_val("class"_s)
      , to_json_val(ClassName{})
      );
    }
  };

  /*
   * for_each
   */
  template <typename PathSpec, typename ItrKey, typename NodeFnList>
  struct action_fn<ui_spec::for_each_tag, PathSpec, ItrKey, NodeFnList>
  {
    action_fn() = delete;
  };

  template <typename Store, typename PathSpec, typename ItrKey, typename NodeFnList>
  struct mut_action_fn<ui_spec::for_each_tag, Store, PathSpec, ItrKey, NodeFnList>
  {
    using Container = ui_spec::detail::get_type_at_path<Store, PathSpec>;
    static_assert( nbdl::Container<Container>::value, "ui_spec::for_each supports only nbdl::Containers");
    using Range = store_range_t<
      ItrKey
    , typename std::add_const<typename std::remove_reference<Container>::type>::type
    , Store
    >;
    using NodeRenderer = renderer_impl<decltype(std::declval<Range>().begin()), NodeFnList, hana::true_>;

    Store store;
    emscripten::val el;
    std::vector<NodeRenderer> renderers;

    mut_action_fn(Store s)
      : store(s)
      , el(emscripten::val::undefined())
      , renderers()
    { }

    template <typename ParentElement>
    decltype(auto) operator()(ParentElement&& p)
    {
      el = p;
      update();
      return std::forward<ParentElement>(p);
    }

    void update()
    {
      // This is currently very naive.
      el.set("innerHTML", emscripten::val(""));
      ui_helper::match_path_spec(store, PathSpec{}, [&](auto const& container)
      {
        Range range = store_range(ItrKey{}, container, store);
        //using Renderer = renderer_impl<decltype(range.begin()), NodeFnList, hana::true_>;

        // just destroy existing renderers :'(
        for(auto& x : renderers)
        {
          x.destroy();
        }

        std::vector<NodeRenderer> temp_renderers{};
        temp_renderers.reserve(container.size());

        for (auto x : range)
        {
          NodeRenderer temp_renderer{x};
          temp_renderer.render(el);
          temp_renderers.push_back(std::move(temp_renderer));
        }
        renderers = std::move(temp_renderers);
      });
    }

    void destroy()
    {
      for(auto& x : renderers)
      {
        x.destroy();
      }
    }
  };

  /*
   * hidden_file_input
   */
  template <typename Handler, typename ...Params>
  struct action_fn<html::tag::hidden_file_input_t, Handler, Params...>
  {
    action_fn() = delete;
  };

  template <typename Store, typename Handler, typename ...Params>
  struct mut_action_fn<html::tag::hidden_file_input_t, Store, Handler, Params...>
  {
    using ChangeReceiverImpl = event_receiver_impl<Store, Handler, Params...>;

    Store store;
    std::unique_ptr<dom_event_listener> click_listener;
    std::unique_ptr<dom_event_listener> change_listener;

    mut_action_fn(Store s)
      : store(s)
      , click_listener()
      , change_listener()
    { }

    template <typename ParentElement>
    decltype(auto) operator()(ParentElement&& p)
    {
      js_val file_input{};
      js_val click_handler{};

      EM_ASM_(
        {
          var file_input = document.createElement("input");
          file_input.setAttribute("type", "file");
          file_input.style.display = "none";

          var click_handler = function(ev)
          {
            file_input.click();
          };

          Module.NBDL_DETAIL_JS_SET($0, file_input);
          Module.NBDL_DETAIL_JS_SET($1, click_handler);
        }
      , file_input.handle()
      , click_handler.handle()
      );

      emscripten::val file_input_el = file_input.as_val();
      p.template call<void>("appendChild", file_input_el);

      // parent click listener
      click_listener = make_dom_event_listener(
        p
      , emscripten::val("click")
      , click_handler.as_val()
      );

      change_listener = make_dom_event_listener(
        ChangeReceiverImpl(store)
      , std::move(file_input_el)
      , emscripten::val("change")
      );

      return std::forward<ParentElement>(p);
    }

    void update()
    { }
  };
}
#endif
