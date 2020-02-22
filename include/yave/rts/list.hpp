//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/box.hpp>

namespace yave {

  template <class T>
  struct list_object_value;

  template <class T>
  using List = Box<list_object_value<T>>;

  template <class T>
  struct object_type_traits<List<T>>
  {
    static constexpr const char name[] = "List<T>";
  };

  // ------------------------------------------
  // list_storage

  struct list_storage
  {
    /// delayed value of T
    object_ptr<const Object> car;
    /// delayed value of List<T>
    object_ptr<const Object> cdr;

    /// nil?
    bool is_nil() const noexcept;

    /// get statically allocated nil object
    static auto get_nil() noexcept -> object_ptr<const Object>;

  private:
    static const List<Object> _nil;
  };

  // ------------------------------------------
  // list_object_value

  /// Value of List<T>
  /// TODO: Add lazy iterator support.
  template <class T>
  struct list_object_value
  {
    /// term
    static constexpr auto term = get_term<list<T>>();

    /// value type
    using value_type = T;

    /// friend function to get address of storage
    [[nodiscard]] friend inline auto _get_storage(
      const list_object_value& v) noexcept -> const list_storage&
    {
      return *reinterpret_cast<const list_storage*>(&(v.m_storage));
    }

    /// friend function to get address of storage
    [[nodiscard]] friend inline auto _get_storage(list_object_value& v) noexcept
      -> list_storage&
    {
      return *reinterpret_cast<list_storage*>(&(v.m_storage));
    }

    /// Default Ctor
    list_object_value() noexcept
    {
      assert(is_nil());
    }

    /// Create new list with single element.
    /// \requires `car != nullptr`
    template <class Car>
    list_object_value(object_ptr<Car> car)
      : m_storage {std::move(car), list_storage::get_nil()}
    {
      if (m_storage.car == nullptr)
        throw std::invalid_argument("car == nullptr");

      constexpr auto elem_tp = type_of(get_term<T>());

      if constexpr (type_of(get_term<Car>()) != elem_tp)
        static_assert(false_v<Car>, "Invalid Car type. Should result T");
    }

    /// Create new list with two element (i.e. Cons operation).
    /// \requires `car != nullptr && cdr != nullptr`
    template <class Car, class Cdr>
    list_object_value(object_ptr<Car> car, object_ptr<Cdr> cdr)
      : m_storage {std::move(car), std::move(cdr)}
    {
      if (m_storage.car == nullptr)
        throw std::invalid_argument("car == nullptr");

      if (m_storage.cdr == nullptr)
        throw std::invalid_argument("cdr == nullptr");

      constexpr auto elem_tp = type_of(get_term<T>());
      constexpr auto list_tp = type_of(get_term<list<T>>());

      constexpr auto car_type = type_of(get_term<Car>());
      constexpr auto cdr_type = type_of(get_term<Cdr>());

      if constexpr (car_type != elem_tp) {
        static_assert(false_v<T>, "Invalid Car type. Should result T");
      }

      if constexpr (cdr_type != list_tp) {
        static_assert(false_v<T>, "Invalid Cdr type. Should result List<T>");
      }
    }

    /// Dtor
    ~list_object_value() noexcept
    {
    }

    /// Copy ctor
    list_object_value(const list_object_value& other) noexcept
      : m_storage {_get_storage(other)}
    {
    }

    /// Move ctor
    list_object_value(list_object_value&& other) noexcept
      : m_storage {std::move(_get_storage(other))}
    {
    }

    /// operator=
    list_object_value& operator=(const list_object_value& other) noexcept
    {
      _get_storage(*this) = _get_storage(other);
      return *this;
    }

    /// operator=
    list_object_value& operator=(list_object_value&& other) noexcept
    {
      _get_storage(*this) = std::move(_get_storage(other));
      return *this;
    }

    /// Empty?
    [[nodiscard]] bool is_nil() const noexcept
    {
      return _get_storage(*this).is_nil();
    }

    /// Get delayed head of list
    [[nodiscard]] auto head() const
    {
      const auto& storage = _get_storage(*this);

      if (is_nil())
        throw std::out_of_range("head() on nil list");

      using To = std::add_const_t<typename decltype(
        get_argument_proxy_type(normalize_specifier(type_c<T>)))::type>;

      // cast to proxy type
      return static_object_cast<To>(storage.car);
    }

    /// Get delayed tail of list
    [[nodiscard]] auto tail() const
    {
      const auto& storage = _get_storage(*this);

      if (is_nil())
        throw std::out_of_range("tail() on nil list");

      return static_object_cast<const ListProxy<T>>(storage.cdr);
    }

  private:
    list_storage m_storage;
  };

  inline const List<Object> list_storage::_nil {static_construct};

  inline bool list_storage::is_nil() const noexcept
  {
    return cdr == nullptr;
  }

  inline auto list_storage::get_nil() noexcept -> object_ptr<const Object>
  {
    return object_ptr<const Object>(&_nil);
  }

  // ------------------------------------------
  // make_list

  namespace detail {
    template <class T, class Head, class... Tail>
    auto make_list_impl(Head&& h, Tail&&... tail)
    {
      if constexpr (sizeof...(Tail) == 0)
        return make_object<const List<T>>(std::forward<Head>(h));
      else
        return make_object<const List<T>>(
          std::forward<Head>(h),
          make_list_impl<T>(std::forward<Tail>(tail)...));
    }
  } // namespace detail

  /// Create empty list
  template <class T>
  [[nodiscard]] auto make_list()
  {
    // Technically UB but can't beat performance...
    return static_object_cast<const List<T>>(list_storage::get_nil());
  }

  /// Create new list from arguments
  template <class T, class... Ts>
  [[nodiscard]] auto make_list(object_ptr<Ts>... args)
  {
    constexpr auto tp = type_of(get_term<T>());

    static_assert(!is_tyerror(tp));

    // check all arguments have same type to T
    if constexpr (((type_of(get_term<Ts>()) == tp) && ...))
      return detail::make_list_impl<T>(std::move(args)...);
    else
      static_assert(false_v<T>, "Inalid argument type. Should reuslt T");
  }

} // namespace yave