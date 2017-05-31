#include <boost/hana.hpp>
#include <mpdef/list.hpp>
#include <nbdl.hpp>
#include <string>

namespace hana = boost::hana;

int main()
{
  auto my_promise = nbdl::promise([](auto& resolver, auto value)
  {
    if (value > hana::size_c<10>)
      resolver.reject(value);
    else
      resolver.resolve(value + hana::size_c<1>);
  });

  volatile std::size_t result;
#if defined(METABENCH)
  nbdl::run_sync(
    hana::make_tuple(
      hana::make_tuple(
        <%= (0..n).map { |i| "my_promise" }.join(', ') %>
      )
    , [&](auto value) { result = decltype(value)::value; }
    , nbdl::catch_([](auto&&) { })
    )
  , hana::size_c<0>
  );
#endif
}
