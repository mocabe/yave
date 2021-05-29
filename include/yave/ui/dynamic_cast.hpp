//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/ui/typeid_cast.hpp>

#include <concepts>

namespace yave::ui {

  template <class Derived>
  struct enable_dynamic_cast
  {
    /// dynamic type check
    template <std::derived_from<Derived> T>
    bool is() const
    {
      return dynamic_cast_p<T>();
    }

    /// dynamic cast
    template <std::derived_from<Derived> T>
    auto as() const -> const T&
    {
      return dynamic_cast_r<T>();
    }

    /// dynamic cast
    template <std::derived_from<Derived> T>
    auto as() -> T&
    {
      return dynamic_cast_r<T>();
    }

    /// dynamic cast to get pointer
    template <std::derived_from<Derived> T>
    auto get_as() const -> const T*
    {
      return dynamic_cast_p<T>();
    }

    /// dynamic cast to get pointer
    template <std::derived_from<Derived> T>
    auto get_as() -> T*
    {
      return dynamic_cast_p<T>();
    }

  private:
    auto as_derived() const
    {
      return static_cast<const Derived*>(this);
    }

    auto as_derived()
    {
      return static_cast<Derived*>(this);
    }

    template <class T>
    auto dynamic_cast_p() const
    {
      if constexpr (std::is_final_v<T>)
        return typeid_cast<const T>(as_derived());
      else
        return dynamic_cast<const T*>(as_derived());
    }

    template <class T>
    auto dynamic_cast_p()
    {
      if constexpr (std::is_final_v<T>)
        return typeid_cast<T>(as_derived());
      else
        return dynamic_cast<T*>(as_derived());
    }

    template <class T>
    auto dynamic_cast_r() const
    {
      if constexpr (std::is_final_v<T>)
        return typeid_cast<const T>(*as_derived());
      else
        return dynamic_cast<const T>(*as_derived());
    }

    template <class T>
    auto dynamic_cast_r()
    {
      if constexpr (std::is_final_v<T>)
        return typeid_cast<T>(*as_derived());
      else
        return dynamic_cast<T>(*as_derived());
    }
  };
} // namespace yave::ui