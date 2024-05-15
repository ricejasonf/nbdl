//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <nbdl/api_signature.hpp>
#include <nbdl/util/base64_encode.hpp>

#include <boost/hana.hpp>
#include <catch.hpp>
#include <string_view>
#include <type_traits>

namespace hana = boost::hana;
using namespace std::literals;

TEST_CASE("Create a unique signature to represent an api.", "[api_signature]")
{
  using Format = hana::string<'j','s', 'o', 'n'>;
  using T1 = std::integral_constant<int, 12345>;
  using T2 = std::integral_constant<int, 123456>;
  using T3 = std::integral_constant<int, 54321>;
  using T4 = hana::tuple<T1, T2, T3>;

  // FIXME This is obviously not comprehensive and not reliable
  //       since it relies on compiler implementation details.
  constexpr auto expected_lookup = hana::make_map(
    hana::make_pair(hana::type_c<T1>, "hC2CGB8MPHXgcXPVLsFwK2lN+Y8="sv)
  , hana::make_pair(hana::type_c<T2>, "Oow0uBtBxbh+hUf8+h4Nm9DDGK4="sv)
  , hana::make_pair(hana::type_c<T3>, "82J9C34K+Xm5lnc7z1cnb8iuEWo="sv)
  , hana::make_pair(hana::type_c<T4>, "3TJmnZgDFCPXmFWNs30cE6i020k="sv)
  );

  hana::for_each(expected_lookup, [](auto const& x)
  {
    using nbdl::util::base64_encode;
    using T = typename decltype(+hana::first(x))::type;
    auto expected = hana::second(x);
    auto result = base64_encode(nbdl::api_signature<T>(Format{}));

    CHECK(result == expected);
  });
}
