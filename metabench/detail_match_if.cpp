#include <boost/hana.hpp>
#include <mpdef/list.hpp>
#include <nbdl.hpp>
#include <string>

namespace hana = boost::hana;

template <std::size_t i>
struct eq_fn
{
  bool operator()(std::size_t value) const
  {
    return value == i;
  }
};

struct always_true_fn
{
  constexpr hana::true_ operator()(std::size_t) const
  { return {}; }
};

int main()
{
  std::size_t result = 9000;
#if defined(METABENCH)
  nbdl::run_sync(
    nbdl::pipe(
      nbdl::detail::match_if(
        mpdef::make_list(
            <%= (0..n).map { |i| "eq_fn<#{i}>{}" }.join(', ') %>
          , always_true_fn{}
        )
      )
    , [&](auto index) { result = index; }
    )
  , 500
  );
#endif
}
