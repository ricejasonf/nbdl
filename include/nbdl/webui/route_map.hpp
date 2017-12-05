//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_WEB_ROUTE_HPP
#define NBDL_WEB_ROUTE_HPP

#include <nbdl/bind_sequence.hpp>
#include <nbdl/fwd/webui/route_map.hpp>
#include <nbdl/string.hpp>
#include <nbdl/variant.hpp>

#include <algorithm>
#include <boost/mp11/algorithm.hpp>
#include <boost/mp11/list.hpp>
#include <boost/mp11/map.hpp>
#include <boost/hana/ext/std/array.hpp>
#include <boost/hana/unpack.hpp>
#include <iterator>
#include <mpdef/pair.hpp>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

namespace nbdl::webui
{
  namespace hana = boost::hana;
  using namespace boost::mp11;

  namespace detail
  {
    template <typename X>
    void append_route_param(std::string& s, X&& x)
    {
      s += '/';
      if constexpr(nbdl::String<X>::value)
      {
        s += std::forward<X>(x);
      }
      else
      {
        s += std::to_string(std::forward<X>(x));
      }
    }

    // zero is an invalid integral param
    inline std::size_t to_integral_param(std::string_view param)
    {
      std::size_t result = 0;

      for (char c : param)
      {
        std::size_t digit_value = c - '0';
        if (digit_value > 9)
        {
          // invalid character
          return 0;
        }
        result *= 10;
        result += digit_value;
      }

      return result;
    }

    template <typename Member, typename = void>
    struct bind_route_param_impl
    {
      bool operator()(Member& m, std::string_view param) const
      {
        m = param;
        return m.size() > 0;
      };
    };

    template <typename Member>
    struct bind_route_param_impl<Member, std::enable_if_t<std::is_integral<Member>::value>>
    {
      bool operator()(Member& m, std::string_view param) const
      {
        m = to_integral_param(param);
        return m != 0;
      };
    };

    template <typename Member>
    bool bind_route_param(Member& m, std::string_view param)
    {
      return bind_route_param_impl<Member>{}(m, param);
    }

    inline std::string_view parse_route_param(std::string_view& s)
    {
      if (s.size() == 0 || s[0] != '/')
      {
        return {};
      }
      // remove leading slash
      s.remove_prefix(1);

      std::size_t end_pos = std::min(s.find('/'), s.size());
      auto* start_ptr = s.data();
      s.remove_prefix(end_pos);

      return std::string_view{start_ptr, end_pos};
    }

    template <typename ...Key>
    struct route_names_impl
    {
      static constexpr auto apply()
      {
        return std::array<std::string_view, sizeof...(Key)>{{
          std::string_view{Key{}.c_str(), hana::size(Key{})}...
        }};
      }
    };

    template <typename Map>
    constexpr auto route_names = mp_apply<route_names_impl, mp_map_keys<Map>>::apply();
  }

  template <typename Map>
  class route_map<Map>
  {
    // max 10 parameters
    using Params = std::array<std::string_view, 10>;
    using ReverseMap = mp_transform<mp_reverse, Map>;
    using Variant = mp_apply<route_variant, mp_map_keys<ReverseMap>>;

    Variant make_from_params(Params const& p, std::size_t params_count) const
    {
      auto itr = std::find_if(
        detail::route_names<Map>.begin()
      , detail::route_names<Map>.end()
      , [&](auto const& x)
        {
          return p[0] == x;
        }
      );

      if (itr == detail::route_names<Map>.end())
      {
        return Variant{};
      }
      else
      {
        std::size_t const offset = std::distance(detail::route_names<Map>.begin(), itr);
        Variant var{};
        mp_with_index<mp_size<Map>::value>(offset, [&](auto index)
        {
          using T = mp_at<mp_map_keys<ReverseMap>, decltype(index)>;
          hana::unpack(p, [&](auto&&, auto ...param)
          {
            T value{};
            nbdl::bind_sequence(value, [&, param...](auto& ...member)
            {
              if (sizeof...(member) != params_count - 1)
              {
                var = Variant{};
                return;
              }

              bool is_params_valid = (detail::bind_route_param(member, param) && ...);
              if (is_params_valid)
              {
                var = value;
              }
              else
              {
                // a param was invalid
                var = Variant{};
              }
            });
          });
        });

        return var;
      }
    }

    public:

    template <typename T>
    Variant to_variant(T&& t) const
    {
      return Variant(std::forward<T>(t));
    }

    template <typename T>
    nbdl::string to_string(T&& t) const
    {
      using String = mp_second<mp_map_find<ReverseMap, std::decay_t<T>>>;
      static_assert(
        !std::is_void<String>::value
      , "Route string must map to type in route map."
      );

      std::string temp{};
      temp.reserve(50);
      temp += '/';
      temp += std::string_view(String{}.c_str(), hana::size(String{}));
      nbdl::bind_sequence(std::forward<T>(t), [&](auto&& ...xs)
      {
        (detail::append_route_param(temp, std::forward<decltype(xs)>(xs)), ...);
      });
      return temp;
    }

    Variant from_string(nbdl::string const& s) const
    {
      Variant v{};

      if (s.size() < 1 || *s.begin() != '/')
      {
        return Variant{};
      }

      // params include the name of the route
      Params params{};
      std::size_t params_count = 0;
      std::string_view view(&(*s.begin()), s.size());

      if (s.size() == 1)
      {
        // root
        params_count = 1;
      }
      else
      {
        for (auto& x : params)
        {
          x = detail::parse_route_param(view);
          if (x.size() == 0)
          {
            break;
          }
          params_count++;
        }

        // trailing params
        if (view.size() != 0)
        {
          return Variant{};
        }
      }
      
      return make_from_params(params, params_count);
    }

    // expose variant type
    using variant = Variant;
  };

  template <typename T>
  template <typename Name>
  constexpr auto route_pair_fn<T>::operator()(Name) const
  {
    return mpdef::pair<Name, T>{};
  } 

  template <typename ...Pairs>
  constexpr auto make_route_map_fn::operator()(Pairs ...) const
  {
    return route_map<mp_list<Pairs...>>{};
  }
}

#endif
