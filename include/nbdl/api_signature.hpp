//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_API_SIGNATURE_HPP
#define NBDL_API_SIGNATURE_HPP

#include <nbdl/detail/beast_sha1.hpp>
#include <nbdl/detail/bind_tag_of.hpp>

#include <array>
#include <boost/hana/type.hpp>
#include <string_view>

namespace nbdl
{
  namespace detail
  {
    template <typename ...T>
    constexpr char const* api_signature_helper(T...)
    {
      return __PRETTY_FUNCTION__;
    };
  }

  template <typename T>
  constexpr auto api_signature = [](auto format_name)
  {
    namespace sha1 = nbdl::detail::beast_sha1;

    std::array<unsigned char, 20> digest{};
    sha1::sha1_context ctx{};

    std::string_view type_str = detail::api_signature_helper(
      nbdl::detail::bind_tag_of<decltype(format_name)>{}
    , nbdl::detail::bind_tag_of<T>{}
    );

    sha1::init(ctx);
    sha1::update(ctx, type_str.begin(), type_str.size());
    sha1::finish(ctx, digest.data());

    return digest;
  };
}

#endif
