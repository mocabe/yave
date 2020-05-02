//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/meta_type.hpp>

namespace yave {

  /// Monadic error handler
  template <class T, bool Succ>
  struct meta_result
  {
    [[nodiscard]] constexpr auto is_succ() const
    {
      return std::bool_constant<Succ> {};
    }
    [[nodiscard]] constexpr auto is_error() const
    {
      return std::bool_constant<!Succ> {};
    }
    [[nodiscard]] constexpr auto value() const
    {
      static_assert(Succ);
      return T {};
    }
    [[nodiscard]] constexpr auto error() const
    {
      static_assert(!Succ);
      return T {};
    }

    template <class F>
    [[nodiscard]] constexpr auto map(F f) const
    {
      if constexpr (Succ)
        return make_succ(f(value()));
      else
        return *this;
    }

    template <class F>
    [[nodiscard]] constexpr auto and_then(F f) const
    {
      if constexpr (Succ)
        return f(value());
      else
        return *this;
    }

    template <class F>
    [[nodiscard]] constexpr auto or_else(F f) const
    {
      if constexpr (Succ)
        return *this;
      else
        f(value());
    }
  };

  template <class T>
  [[nodiscard]] constexpr auto make_succ(meta_type<T>)
  {
    return meta_result<meta_type<T>, true> {};
  }

  template <class... Ts>
  [[nodiscard]] constexpr auto make_succ(meta_tuple<Ts...>)
  {
    return meta_result<meta_tuple<Ts...>, true> {};
  }

  template <class T>
  [[nodiscard]] constexpr auto make_error(meta_type<T>)
  {
    return meta_result<meta_type<T>, false> {};
  }
}