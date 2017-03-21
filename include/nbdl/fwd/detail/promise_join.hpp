//
// Copyright Jason Rice 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_FWD_DETAIL_PROMISE_JOIN_HPP
#define NBDL_FWD_DETAIL_PROMISE_JOIN_HPP

namespace nbdl::detail
{
  struct promise_join_nested_tag { };

  struct promise_zero_t
  {
    using hana_tag = promise_join_nested_tag;

    template <typename Resolver, typename ...Args>
    void resolve(Resolver& r, Args&& ...args)
    {
      r.resolve(std::forward<Args>(args)...);
    }

    template <typename Resolver, typename ...Args>
    void reject(Resolver& r, Args&& ...args)
    {
      r.reject(std::forward<Args>(args)...);
    }
  };

  template <typename Current, typename Next>
  struct promise_join_nested_t
  {
    using hana_tag = promise_join_nested_tag;

    Current current;
    Next next;

    template <typename T, typename U>
    promise_join_nested_t(T&& c, U&& n)
      : current(std::forward<T>(c))
      , next(std::forward<U>(n))
    { }

    template <typename ...Args>
    void resolve(Args&& ...args)
    {
      current.resolve(next, std::forward<Args>(args)...);
    }

    template <typename ...Args>
    void reject(Args&&... args)
    {
      // here's the difference
      // skips current so nested pipes don't handle
      // errors from parent
      next.reject(std::forward<Args>(args)...);
    }

    void terminate()
    {
      next.terminate();
    }
  };

  struct promise_join_tag { };

  template <typename Current, typename Next>
  struct promise_join_t
  {
    using hana_tag = promise_join_tag;

    Current current;
    Next next;

    template <typename T, typename U>
    promise_join_t(T&& c, U&& n)
      : current(std::forward<T>(c))
      , next(std::forward<U>(n))
    { }

    template <typename ...Args>
    void resolve(Args&& ...args)
    {
      current.resolve(next, std::forward<Args>(args)...);
    }

    template <typename ...Args>
    void reject(Args&&... args)
    {
      current.reject(next, std::forward<Args>(args)...);
    }

    void terminate()
    {
      next.terminate();
    }
  };

  struct promise_join_fn
  {
    template <typename Current, typename Next>
    auto operator()(Current&& current, Next&& next) const;
  };

  constexpr promise_join_fn promise_join{};
}

#endif
