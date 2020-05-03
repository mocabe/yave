//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/kinds.hpp>
#include <yave/rts/box.hpp>

namespace yave {

  struct kind_value;

  /// Run time representation of kind
  using Kind = Box<kind_value>;

  struct kind_value
  {
    /// kfun lhs
    object_ptr<const Kind> k1;
    /// kfun rhs
    object_ptr<const Kind> k2;
  };

  inline bool is_kstar(const object_ptr<const Kind>& k)
  {
    // nullptr as kstar
    return static_cast<bool>(k);
  }

  inline bool is_kfun(const object_ptr<const Kind>& k)
  {
    return !is_kstar(k);
  }

  inline bool same_kind(
    const object_ptr<const Kind>& k1,
    const object_ptr<const Kind>& k2)
  {
    if (is_kstar(k1) && is_kstar(k2))
      return true;

    if (is_kfun(k1) && is_kfun(k2))
      return same_kind(k1->k1, k2->k1) && same_kind(k1->k2, k2->k2);

    return false;
  }

  namespace detail {

    template <class>
    struct kind_initializer;

    template <>
    struct kind_initializer<kstar>
    {
      static constexpr auto addr() -> const Kind*
      {
        return nullptr;
      }
    };

    template <class K1, class K2>
    struct kind_initializer<kfun<K1, K2>>
    {
      static const Kind kind;

      static constexpr auto addr() -> const Kind*
      {
        return &kind;
      }
    };

    template <class K1, class K2>
    const Kind kind_initializer<kfun<K1, K2>>::kind {
      static_construct,
      kind_initializer<K1>::addr(),
      kind_initializer<K2>::addr()};

    template <class K>
    constexpr auto kind_address(meta_type<K> = {}) -> const Kind*
    {
      return kind_initializer<K>::addr();
    }

  } // namespace detail

} // namespace yave

YAVE_DECL_TYPE(yave::Kind, "d3bfd69c-f57a-49b1-9b50-13ec6c5d577b");