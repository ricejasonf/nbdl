#include <boost/hana.hpp>
#include <mpdef/list.hpp>
#include <nbdl.hpp>
#include <string>

namespace hana = boost::hana;

struct foo_t { };
constexpr hana::type<foo_t> foo{};

int main()
{
  using nbdl::ui_spec::get;

  auto store = hana::make_map(
    hana::make_pair(foo, std::string("OK!"))
  );

#if defined(METABENCH)
  nbdl::run_sync(
    nbdl::params_promise(mpdef::make_list(
      hana::int_c<-1>
    , get(foo)
    , <%= (0..n).map { |i| "hana::int_c<#{i}>" }.join(', ') %>
    ))
  , store
  );
#endif
}
