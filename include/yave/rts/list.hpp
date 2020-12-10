//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/dynamic_typing.hpp>

namespace yave {

  /// list term
  template <class T>
  struct tm_list;

  /// list specifier
  template <class T>
  struct list;

  /// list tcon tag
  struct list_tcon_tag;

  /// list type
  template <class T>
  using ty_list = tap<tcon<list_tcon_tag, kfun<kstar, kstar>>, T>;

  /// list proxy object
  template <class T>
  struct ListProxy;

  // ------------------------------------------
  // meta_type

  template <class T>
  struct meta_type<tm_list<T>>
  {
    using type = tm_list<T>;
    constexpr auto t() const
    {
      return type_c<T>;
    }
  };

  // ------------------------------------------
  // helper

  template <class T>
  [[nodiscard]] constexpr auto make_tm_list(meta_type<T>)
  {
    return type_c<tm_list<T>>;
  }

  template <class T>
  [[nodiscard]] constexpr auto make_ty_list(meta_type<T>)
  {
    return type_c<ty_list<T>>;
  }

  // ------------------------------------------
  // generalize_tm_varvalue

  template <class T, class Target>
  constexpr auto generalize_tm_varvalue_impl(
    meta_type<tm_list<T>> term,
    meta_type<Target> target)
  {
    return generalize_tm_varvalue_impl(term.t(), target);
  }

  // ------------------------------------------
  // subst_term_impl

  template <class From, class To, class T>
  constexpr auto subst_term_impl(
    meta_type<From> from,
    meta_type<To> to,
    meta_type<tm_list<T>> term)
  {
    return make_tm_list(subst_term(from, to, term.t()));
  }

  // ------------------------------------------
  // genpoly_impl

  template <class T, class Gen, class Target>
  constexpr auto genpoly_impl(
    meta_type<tm_list<T>> term,
    meta_type<Gen> gen,
    meta_type<Target> target)
  {
    return genpoly_impl(term.t(), gen, target);
  }

  // ------------------------------------------
  // type_of_impl

  template <class T, class Gen, bool Assert>
  constexpr auto type_of_impl(
    meta_type<tm_list<T>> term,
    meta_type<Gen> gen,
    std::bool_constant<Assert> enable_assert)
  {
    auto p = type_of_impl(term.t(), gen, enable_assert);
    auto t = p.first();
    auto g = p.second();
    return make_pair(make_ty_list(t), g);
  }

  // ------------------------------------------
  // specifier

  template <class T>
  [[nodiscard]] constexpr auto is_specifier(meta_type<list<T>>)
  {
    return true_c;
  }

  template <class T>
  [[nodiscard]] constexpr auto normalize_specifier(meta_type<list<T>>)
  {
    return type_c<
      list<typename decltype(normalize_specifier(type_c<T>))::type>>;
  }

  template <class T>
  [[nodiscard]] constexpr auto get_proxy_type(meta_type<list<T>>)
  {
    return type_c<
      ListProxy<typename decltype(get_proxy_type(type_c<T>))::type>>;
  }

  template <class T>
  [[nodiscard]] constexpr auto get_argument_proxy_type(meta_type<list<T>> l)
  {
    return get_proxy_type(l);
  }

  // ------------------------------------------
  // get_list_object_type

  template <class T>
  struct list_object_value;

  template <class T>
  using List = Box<list_object_value<T>>;

  template <class T>
  [[nodiscard]] constexpr auto get_list_object_type(meta_type<ty_list<T>>)
  {
    return type_c<List<T>>;
  }

  // ------------------------------------------
  // type gen

  /// proxy type of list
  template <class T>
  struct ListProxy : Object
  {
    /// term
    static constexpr auto term = make_tm_list(get_term<T>());
  };

  constexpr auto list_type_uuid()
  {
    return read_uuid_from_constexpr_string(
      "d14b9346-a02d-4a53-aaa5-64cdf3f2e4b3");
  }

  constexpr auto list_type_name()
  {
    return "[]";
  }

  template <>
  struct tcon_traits<list_tcon_tag>
  {
    static constexpr auto id   = list_type_uuid();
    static constexpr auto name = list_type_name();
  };

  template <class T>
  struct term_to_type<tm_list<T>>
  {
    using type = ty_list<term_to_type_t<T>>;
  };

  [[nodiscard]] inline auto list_type_tcon() -> object_ptr<const Type>
  {
    return &type_initializer<tcon<list_tcon_tag, kfun<kstar, kstar>>>::type;
  }

  template <class T>
  [[nodiscard]] constexpr auto guess_object_type(meta_type<ty_list<T>>)
  {
    return get_list_object_type(make_ty_list(guess_object_type(type_c<T>)));
  }

  template <class T>
  struct object_type_traits<List<T>>
  {
    static constexpr const char name[] = "List<T>";
  };

  // ------------------------------------------
  // has_type

  template <class Type, class Term, class U>
  bool has_type_impl(
    meta_type<Type>,
    meta_type<tm_list<Term>>,
    const object_ptr<U>& obj)
  {
    return same_type(get_type(obj), object_type<Type>());
  }

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

      constexpr auto elem_tp  = type_of(get_term<T>());
      constexpr auto car_type = type_of(get_term<Car>());

      if constexpr (!match(elem_tp, car_type).is_succ())
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

      if constexpr (!match(elem_tp, car_type).is_succ())
        static_assert(
          false_v<decltype(car_type), decltype(elem_tp)>,
          "Invalid Car type. Should result T");

      if constexpr (!match(list_tp, cdr_type).is_succ())
        static_assert(false_v<T>, "Invalid Cdr type. Should result List<T>");
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

  // ------------------------------------------
  // dynamic typing

  [[nodiscard]] inline auto make_list_type(const object_ptr<const Type>& t)
  {
    return make_object<const Type>(tap_type {list_type_tcon(), t});
  }

  [[nodiscard]] inline auto is_list_type(const object_ptr<const Type>& t)
  {
    if (auto tap = is_tap_type_if(t))
      if (same_type(tap->t1, list_type_tcon()))
        return true;

    return false;
  }

} // namespace yave