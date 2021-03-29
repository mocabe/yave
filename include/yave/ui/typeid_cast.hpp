//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <typeinfo>
#include <type_traits>

namespace yave::ui {

  /// cast to derived type by comparing typeids
  template <class Derived, class Base>
  auto typeid_cast_if(Base* b) noexcept -> Derived*
  {
    static_assert(std::is_base_of_v<Base, Derived>);

    if (b && typeid(*b) == typeid(Derived))
      return static_cast<Derived*>(b);

    return nullptr;
  }

  /// cast to derived type by comparing typeids
  template <class Derived, class Base>
  auto typeid_cast_if(const Base* b) noexcept -> const Derived*
  {
    static_assert(std::is_base_of_v<Base, Derived>);

    if (b && typeid(*b) == typeid(Derived))
      return static_cast<const Derived*>(b);

    return nullptr;
  }

  /// cast to derived type by comparing typeids
  template <class Derived, class Base>
  auto typeid_cast(Base* b) -> Derived*
  {
    if (auto p = typeid_cast_if<Derived>(b))
      return p;

    throw std::bad_cast();
  }

  /// cast to derived type by comparing typeids
  template <class Derived, class Base>
  auto typeid_cast(const Base* b) -> const Derived*
  {
    if (auto p = typeid_cast_if<Derived>(b))
      return p;

    throw std::bad_cast();
  }

} // namespace yave::ui