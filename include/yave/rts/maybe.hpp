//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/box.hpp>

namespace yave {

  template <class T>
  struct maybe_object_value;

  template <class T>
  using Maybe = Box<maybe_object_value<T>>;

  template <class T>
  struct object_type_traits<Maybe<T>>
  {
    static constexpr const char name[] = "Maybe<T>";
  };

  // ------------------------------------------
  // maybe_storage

  struct maybe_storage
  {
    /// nullptr or valid
    object_ptr<const Object> value = nullptr;

    /// has value?
    bool has_value() const noexcept;

    /// nothing?
    bool is_nothing() const noexcept;

    /// get statically allocated nothing object
    static auto get_nothing() noexcept -> object_ptr<const Object>;

  private:
    static const Maybe<Object> _nothing;
  };

  // ------------------------------------------
  // maybe_object_value

  template <class T>
  struct maybe_object_value
  {
    /// term
    static constexpr auto term = get_term<maybe<T>>();

    /// value type
    using value_type = T;

    [[nodiscard]] friend inline auto _get_storage(
      const maybe_object_value& v) noexcept -> const maybe_storage&
    {
      return *reinterpret_cast<const maybe_storage*>(&(v.m_storage));
    }

    [[nodiscard]] friend inline auto _get_storage(
      maybe_object_value& v) noexcept -> maybe_storage&
    {
      return *reinterpret_cast<maybe_storage*>(&(v.m_storage));
    }

    maybe_object_value() noexcept
      : m_storage {nullptr}
    {
    }

    template <class U>
    maybe_object_value(object_ptr<U> u)
      : m_storage {std::move(u)}
    {
      constexpr auto lhs = type_of(get_term<T>());
      constexpr auto rhs = type_of(get_term<U>());

      if constexpr (!match(lhs, rhs).is_succ())
        static_assert(false_v<U>, "Invalid argument type. Should result T");
    }

    /// has value?
    [[nodiscard]] bool has_value() const
    {
      return _get_storage(*this).has_value();
    }

    /// nothing?
    [[nodiscard]] bool is_nothing() const
    {
      return _get_storage(*this).is_nothing();
    }

    /// get value
    [[nodiscard]] auto value() const
    {
      if (is_nothing())
        throw std::runtime_error("bad Maybe access");

      using To = std::add_const_t<typename decltype(
        get_argument_proxy_type(normalize_specifier(type_c<T>)))::type>;

      // cast to proxy type
      return static_object_cast<To>(_get_storage(*this).value);
    }

  private:
    /// storage
    maybe_storage m_storage;
  };

  inline const Maybe<Object> maybe_storage::_nothing {static_construct};

  inline bool maybe_storage::is_nothing() const noexcept
  {
    return !has_value();
  }

  inline bool maybe_storage::has_value() const noexcept
  {
    return value.get();
  }

  inline auto maybe_storage::get_nothing() noexcept -> object_ptr<const Object>
  {
    return object_ptr<const Object>(&_nothing);
  }

  // ------------------------------------------
  // make_maybe

  /// Create Maybe
  template <class T>
  [[nodiscard]] auto make_maybe()
  {
    return static_object_cast<const Maybe<T>>(maybe_storage::get_nothing());
  }

  /// Create Maybe
  template <class T, class U>
  [[nodiscard]] auto make_maybe(object_ptr<U> v)
  {
    return make_object<Maybe<T>>(std::move(v));
  }
}