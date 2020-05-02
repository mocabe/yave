//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/meta_type.hpp>

namespace yave {

  // ------------------------------------------
  // Kinds

  /// *
  struct kstar
  {
  };

  /// * -> *
  template <class K1, class K2>
  struct kfun
  {
    using k1 = K1;
    using k2 = K2;
  };

  template <>
  struct meta_type<kstar>
  {
  };

  template <class K1, class K2>
  struct meta_type<kfun<K1, K2>>
  {
    [[nodiscard]] constexpr auto k1() const
    {
      return type_c<K1>;
    }

    [[nodiscard]] constexpr auto k2() const
    {
      return type_c<K2>;
    }
  };

  // ------------------------------------------
  // is_kind

  template <class T>
  [[nodiscard]] constexpr auto is_kind(meta_type<T>)
  {
    return false_c;
  }

  [[nodiscard]] constexpr auto is_kind(meta_type<kstar>)
  {
    return true_c;
  }

  template <class K1, class K2>
  [[nodiscard]] constexpr auto is_kind(meta_type<kfun<K1, K2>>)
  {
    return true_c;
  }

  // ------------------------------------------
  // make_kind

  [[nodiscard]] constexpr auto make_kind()
  {
    return type_c<kstar>;
  }

  template <class K1, class K2>
  [[nodiscard]] constexpr auto make_kind(meta_type<K1> k1, meta_type<K2> k2)
  {
    static_assert(is_kind(k1) && is_kind(k2));
    return type_c<kfun<K1, K2>>;
  }

} // namespace yave