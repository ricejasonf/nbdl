//
// Copyright Jason Rice 2015
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef _MPDEF_METASTRUCT_KEYS

#include<mpdef/MPDEF_VFUNC.hpp>

// TODO this needs to be reversed
#define _MPDEF_METASTRUCT_KEYS(...) \
  static constexpr std::size_t length = __MPDEF_NARG__(__VA_ARGS__); \
  MPDEF_VFUNC(_MPDEF_METASTRUCT_KEYS, __VA_ARGS__)

#define _MPDEF_METASTRUCT_KEYS63(NAME, ...) \
  static constexpr auto NAME = at_c<length - 63>; \
  _MPDEF_METASTRUCT_KEYS62(__VA_ARGS__)
#define _MPDEF_METASTRUCT_KEYS62(NAME, ...) \
  static constexpr auto NAME = at_c<length - 62>; \
  _MPDEF_METASTRUCT_KEYS61(__VA_ARGS__)
#define _MPDEF_METASTRUCT_KEYS61(NAME, ...) \
  static constexpr auto NAME = at_c<length - 61>; \
  _MPDEF_METASTRUCT_KEYS60(__VA_ARGS__)
#define _MPDEF_METASTRUCT_KEYS60(NAME, ...) \
  static constexpr auto NAME = at_c<length - 60>; \
  _MPDEF_METASTRUCT_KEYS59(__VA_ARGS__)
#define _MPDEF_METASTRUCT_KEYS59(NAME, ...) \
  static constexpr auto NAME = at_c<length - 59>; \
  _MPDEF_METASTRUCT_KEYS58(__VA_ARGS__)
#define _MPDEF_METASTRUCT_KEYS58(NAME, ...) \
  static constexpr auto NAME = at_c<length - 58>; \
  _MPDEF_METASTRUCT_KEYS57(__VA_ARGS__)
#define _MPDEF_METASTRUCT_KEYS57(NAME, ...) \
  static constexpr auto NAME = at_c<length - 57>; \
  _MPDEF_METASTRUCT_KEYS56(__VA_ARGS__)
#define _MPDEF_METASTRUCT_KEYS56(NAME, ...) \
  static constexpr auto NAME = at_c<length - 56>; \
  _MPDEF_METASTRUCT_KEYS55(__VA_ARGS__)
#define _MPDEF_METASTRUCT_KEYS55(NAME, ...) \
  static constexpr auto NAME = at_c<length - 55>; \
  _MPDEF_METASTRUCT_KEYS54(__VA_ARGS__)
#define _MPDEF_METASTRUCT_KEYS54(NAME, ...) \
  static constexpr auto NAME = at_c<length - 54>; \
  _MPDEF_METASTRUCT_KEYS53(__VA_ARGS__)
#define _MPDEF_METASTRUCT_KEYS53(NAME, ...) \
  static constexpr auto NAME = at_c<length - 53>; \
  _MPDEF_METASTRUCT_KEYS52(__VA_ARGS__)
#define _MPDEF_METASTRUCT_KEYS52(NAME, ...) \
  static constexpr auto NAME = at_c<length - 52>; \
  _MPDEF_METASTRUCT_KEYS51(__VA_ARGS__)
#define _MPDEF_METASTRUCT_KEYS51(NAME, ...) \
  static constexpr auto NAME = at_c<length - 51>; \
  _MPDEF_METASTRUCT_KEYS50(__VA_ARGS__)
#define _MPDEF_METASTRUCT_KEYS50(NAME, ...) \
  static constexpr auto NAME = at_c<length - 50>; \
  _MPDEF_METASTRUCT_KEYS49(__VA_ARGS__)
#define _MPDEF_METASTRUCT_KEYS49(NAME, ...) \
  static constexpr auto NAME = at_c<length - 49>; \
  _MPDEF_METASTRUCT_KEYS48(__VA_ARGS__)
#define _MPDEF_METASTRUCT_KEYS48(NAME, ...) \
  static constexpr auto NAME = at_c<length - 48>; \
  _MPDEF_METASTRUCT_KEYS47(__VA_ARGS__)
#define _MPDEF_METASTRUCT_KEYS47(NAME, ...) \
  static constexpr auto NAME = at_c<length - 47>; \
  _MPDEF_METASTRUCT_KEYS46(__VA_ARGS__)
#define _MPDEF_METASTRUCT_KEYS46(NAME, ...) \
  static constexpr auto NAME = at_c<length - 46>; \
  _MPDEF_METASTRUCT_KEYS45(__VA_ARGS__)
#define _MPDEF_METASTRUCT_KEYS45(NAME, ...) \
  static constexpr auto NAME = at_c<length - 45>; \
  _MPDEF_METASTRUCT_KEYS44(__VA_ARGS__)
#define _MPDEF_METASTRUCT_KEYS44(NAME, ...) \
  static constexpr auto NAME = at_c<length - 44>; \
  _MPDEF_METASTRUCT_KEYS43(__VA_ARGS__)
#define _MPDEF_METASTRUCT_KEYS43(NAME, ...) \
  static constexpr auto NAME = at_c<length - 43>; \
  _MPDEF_METASTRUCT_KEYS42(__VA_ARGS__)
#define _MPDEF_METASTRUCT_KEYS42(NAME, ...) \
  static constexpr auto NAME = at_c<length - 42>; \
  _MPDEF_METASTRUCT_KEYS41(__VA_ARGS__)
#define _MPDEF_METASTRUCT_KEYS41(NAME, ...) \
  static constexpr auto NAME = at_c<length - 41>; \
  _MPDEF_METASTRUCT_KEYS40(__VA_ARGS__)
#define _MPDEF_METASTRUCT_KEYS40(NAME, ...) \
  static constexpr auto NAME = at_c<length - 40>; \
  _MPDEF_METASTRUCT_KEYS39(__VA_ARGS__)
#define _MPDEF_METASTRUCT_KEYS39(NAME, ...) \
  static constexpr auto NAME = at_c<length - 39>; \
  _MPDEF_METASTRUCT_KEYS38(__VA_ARGS__)
#define _MPDEF_METASTRUCT_KEYS38(NAME, ...) \
  static constexpr auto NAME = at_c<length - 38>; \
  _MPDEF_METASTRUCT_KEYS37(__VA_ARGS__)
#define _MPDEF_METASTRUCT_KEYS37(NAME, ...) \
  static constexpr auto NAME = at_c<length - 37>; \
  _MPDEF_METASTRUCT_KEYS36(__VA_ARGS__)
#define _MPDEF_METASTRUCT_KEYS36(NAME, ...) \
  static constexpr auto NAME = at_c<length - 36>; \
  _MPDEF_METASTRUCT_KEYS35(__VA_ARGS__)
#define _MPDEF_METASTRUCT_KEYS35(NAME, ...) \
  static constexpr auto NAME = at_c<length - 35>; \
  _MPDEF_METASTRUCT_KEYS34(__VA_ARGS__)
#define _MPDEF_METASTRUCT_KEYS34(NAME, ...) \
  static constexpr auto NAME = at_c<length - 34>; \
  _MPDEF_METASTRUCT_KEYS33(__VA_ARGS__)
#define _MPDEF_METASTRUCT_KEYS33(NAME, ...) \
  static constexpr auto NAME = at_c<length - 33>; \
  _MPDEF_METASTRUCT_KEYS32(__VA_ARGS__)
#define _MPDEF_METASTRUCT_KEYS32(NAME, ...) \
  static constexpr auto NAME = at_c<length - 32>; \
  _MPDEF_METASTRUCT_KEYS31(__VA_ARGS__)
#define _MPDEF_METASTRUCT_KEYS31(NAME, ...) \
  static constexpr auto NAME = at_c<length - 31>; \
  _MPDEF_METASTRUCT_KEYS30(__VA_ARGS__)
#define _MPDEF_METASTRUCT_KEYS30(NAME, ...) \
  static constexpr auto NAME = at_c<length - 30>; \
  _MPDEF_METASTRUCT_KEYS29(__VA_ARGS__)
#define _MPDEF_METASTRUCT_KEYS29(NAME, ...) \
  static constexpr auto NAME = at_c<length - 29>; \
  _MPDEF_METASTRUCT_KEYS28(__VA_ARGS__)
#define _MPDEF_METASTRUCT_KEYS28(NAME, ...) \
  static constexpr auto NAME = at_c<length - 28>; \
  _MPDEF_METASTRUCT_KEYS27(__VA_ARGS__)
#define _MPDEF_METASTRUCT_KEYS27(NAME, ...) \
  static constexpr auto NAME = at_c<length - 27>; \
  _MPDEF_METASTRUCT_KEYS26(__VA_ARGS__)
#define _MPDEF_METASTRUCT_KEYS26(NAME, ...) \
  static constexpr auto NAME = at_c<length - 26>; \
  _MPDEF_METASTRUCT_KEYS25(__VA_ARGS__)
#define _MPDEF_METASTRUCT_KEYS25(NAME, ...) \
  static constexpr auto NAME = at_c<length - 25>; \
  _MPDEF_METASTRUCT_KEYS24(__VA_ARGS__)
#define _MPDEF_METASTRUCT_KEYS24(NAME, ...) \
  static constexpr auto NAME = at_c<length - 24>; \
  _MPDEF_METASTRUCT_KEYS23(__VA_ARGS__)
#define _MPDEF_METASTRUCT_KEYS23(NAME, ...) \
  static constexpr auto NAME = at_c<length - 23>; \
  _MPDEF_METASTRUCT_KEYS22(__VA_ARGS__)
#define _MPDEF_METASTRUCT_KEYS22(NAME, ...) \
  static constexpr auto NAME = at_c<length - 22>; \
  _MPDEF_METASTRUCT_KEYS21(__VA_ARGS__)
#define _MPDEF_METASTRUCT_KEYS21(NAME, ...) \
  static constexpr auto NAME = at_c<length - 21>; \
  _MPDEF_METASTRUCT_KEYS20(__VA_ARGS__)
#define _MPDEF_METASTRUCT_KEYS20(NAME, ...) \
  static constexpr auto NAME = at_c<length - 20>; \
  _MPDEF_METASTRUCT_KEYS19(__VA_ARGS__)
#define _MPDEF_METASTRUCT_KEYS19(NAME, ...) \
  static constexpr auto NAME = at_c<length - 19>; \
  _MPDEF_METASTRUCT_KEYS18(__VA_ARGS__)
#define _MPDEF_METASTRUCT_KEYS18(NAME, ...) \
  static constexpr auto NAME = at_c<length - 18>; \
  _MPDEF_METASTRUCT_KEYS17(__VA_ARGS__)
#define _MPDEF_METASTRUCT_KEYS17(NAME, ...) \
  static constexpr auto NAME = at_c<length - 17>; \
  _MPDEF_METASTRUCT_KEYS16(__VA_ARGS__)
#define _MPDEF_METASTRUCT_KEYS16(NAME, ...) \
  static constexpr auto NAME = at_c<length - 16>; \
  _MPDEF_METASTRUCT_KEYS15(__VA_ARGS__)
#define _MPDEF_METASTRUCT_KEYS15(NAME, ...) \
  static constexpr auto NAME = at_c<length - 15>; \
  _MPDEF_METASTRUCT_KEYS14(__VA_ARGS__)
#define _MPDEF_METASTRUCT_KEYS14(NAME, ...) \
  static constexpr auto NAME = at_c<length - 14>; \
  _MPDEF_METASTRUCT_KEYS13(__VA_ARGS__)
#define _MPDEF_METASTRUCT_KEYS13(NAME, ...) \
  static constexpr auto NAME = at_c<length - 13>; \
  _MPDEF_METASTRUCT_KEYS12(__VA_ARGS__)
#define _MPDEF_METASTRUCT_KEYS12(NAME, ...) \
  static constexpr auto NAME = at_c<length - 12>; \
  _MPDEF_METASTRUCT_KEYS11(__VA_ARGS__)
#define _MPDEF_METASTRUCT_KEYS11(NAME, ...) \
  static constexpr auto NAME = at_c<length - 11>; \
  _MPDEF_METASTRUCT_KEYS10(__VA_ARGS__)
#define _MPDEF_METASTRUCT_KEYS10(NAME, ...) \
  static constexpr auto NAME = at_c<length - 10>; \
  _MPDEF_METASTRUCT_KEYS9(__VA_ARGS__)
#define _MPDEF_METASTRUCT_KEYS9(NAME, ...) \
  static constexpr auto NAME = at_c<length - 9>; \
  _MPDEF_METASTRUCT_KEYS8(__VA_ARGS__)
#define _MPDEF_METASTRUCT_KEYS8(NAME, ...) \
  static constexpr auto NAME = at_c<length - 8>; \
  _MPDEF_METASTRUCT_KEYS7(__VA_ARGS__)
#define _MPDEF_METASTRUCT_KEYS7(NAME, ...) \
  static constexpr auto NAME = at_c<length - 7>; \
  _MPDEF_METASTRUCT_KEYS6(__VA_ARGS__)
#define _MPDEF_METASTRUCT_KEYS6(NAME, ...) \
  static constexpr auto NAME = at_c<length - 6>; \
  _MPDEF_METASTRUCT_KEYS5(__VA_ARGS__)
#define _MPDEF_METASTRUCT_KEYS5(NAME, ...) \
  static constexpr auto NAME = at_c<length - 5>; \
  _MPDEF_METASTRUCT_KEYS4(__VA_ARGS__)
#define _MPDEF_METASTRUCT_KEYS4(NAME, ...) \
  static constexpr auto NAME = at_c<length - 4>; \
  _MPDEF_METASTRUCT_KEYS3(__VA_ARGS__)
#define _MPDEF_METASTRUCT_KEYS3(NAME, ...) \
  static constexpr auto NAME = at_c<length - 3>; \
  _MPDEF_METASTRUCT_KEYS2(__VA_ARGS__)
#define _MPDEF_METASTRUCT_KEYS2(NAME, ...) \
  static constexpr auto NAME = at_c<length - 2>; \
  _MPDEF_METASTRUCT_KEYS1(__VA_ARGS__)
#define _MPDEF_METASTRUCT_KEYS1(NAME, ...) \
  static constexpr auto NAME = at_c<length - 1>;

#endif
