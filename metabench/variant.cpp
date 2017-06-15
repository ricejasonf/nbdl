#include <boost/hana/integral_constant.hpp>
#include <nbdl/variant.hpp>

namespace hana = boost::hana;

int main()
{
#if defined(METABENCH)
  nbdl::variant<<%= (0..n).map { |i| "hana::size_t<#{i}>" }.join(', ') %>>{};
#endif
}
