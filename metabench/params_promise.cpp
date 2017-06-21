#include <boost/hana.hpp>
#include <mpdef/list.hpp>
#include <nbdl.hpp>
#include <string>

namespace hana = boost::hana;

struct foo_t { };
constexpr hana::type<foo_t> foo{};

template <int i, typename Store>
void make_params_promise(Store const& store)
{
  using nbdl::ui_spec::get;

  nbdl::run_sync(
    nbdl::params_promise(mpdef::make_list(
      hana::int_c<-1 - i>
    , get(foo)
    , hana::int_c<1 + i>
    , hana::int_c<2 + i>
    , hana::int_c<3 + i>
    , hana::int_c<4 + i>
    ))
  , store
  );
}

int main()
{
  auto store = hana::make_map(
    hana::make_pair(foo, std::string("OK!"))
  );

#if defined(METABENCH)
  <%= (0..n).map { |i| "make_params_promise<#{i}>(store)" }.join('; ') %>;
#endif
}
