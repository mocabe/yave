//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/dynamic_typing.hpp>

namespace yave {

  //
  // Implementation of Maybe type constructor.
  // This might be a good example for extending RTS static type system without
  // modifying existing headers.
  //

  /// maybe term
  template <class T>
  struct tm_maybe;

  /// maybe specifier
  template <class T>
  struct maybe;

  /// maybe tcon tag
  struct maybe_tcon_tag;

  /// maybe type
  template <class T>
  using ty_maybe = tap<tcon<maybe_tcon_tag, kfun<kstar, kstar>>, T>;

  /// maybe proxy object
  template <class T>
  struct MaybeProxy;

  // ------------------------------------------
  // meta_type

  template <class T>
  struct meta_type<tm_maybe<T>>
  {
    using type = tm_maybe<T>;

    constexpr auto t() const
    {
      return type_c<T>;
    }
  };

  template <class T>
  struct meta_type<maybe<T>>
  {
    using type = maybe<T>;

    constexpr auto t() const
    {
      return type_c<T>;
    }
  };

  // ------------------------------------------
  // helper

  template <class T>
  [[nodiscard]] constexpr auto make_tm_maybe(meta_type<T>)
  {
    return type_c<tm_maybe<T>>;
  }

  template <class T>
  [[nodiscard]] constexpr auto make_ty_maybe(meta_type<T>)
  {
    return type_c<ty_maybe<T>>;
  }

  // ------------------------------------------
  // generalize_tm_varvalue

  template <class T, class Target>
  constexpr auto generalize_tm_varvalue_impl(
    meta_type<tm_maybe<T>> term,
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
    meta_type<tm_maybe<T>> term)
  {
    return make_tm_maybe(subst_term(from, to, term.t()));
  }

  // ------------------------------------------
  // genpoly_impl

  template <class T, class Gen, class Target>
  constexpr auto genpoly_impl(
    meta_type<tm_maybe<T>> term,
    meta_type<Gen> gen,
    meta_type<Target> target)
  {
    return genpoly_impl(term.t(), gen, target);
  }

  // ------------------------------------------
  // type_of_impl

  template <class T, class Gen, bool Assert>
  constexpr auto type_of_impl(
    meta_type<tm_maybe<T>> term,
    meta_type<Gen> gen,
    std::bool_constant<Assert> enable_assert)
  {
    auto p = type_of_impl(term.t(), gen, enable_assert);
    auto t = p.first();
    auto g = p.second();
    return make_pair(make_ty_maybe(t), g);
  }

  // ------------------------------------------
  // specifier

  template <class T>
  [[nodiscard]] constexpr auto is_specifier(meta_type<maybe<T>>)
  {
    return true_c;
  }

  template <class T>
  [[nodiscard]] constexpr auto normalize_specifier(meta_type<maybe<T>> m)
  {
    return type_c<maybe<typename decltype(normalize_specifier(m.t()))::type>>;
  }

  template <class T>
  [[nodiscard]] constexpr auto get_proxy_type(meta_type<maybe<T>> m)
  {
    return type_c<MaybeProxy<typename decltype(get_proxy_type(m.t()))::type>>;
  }

  template <class T>
  [[nodiscard]] constexpr auto get_argument_proxy_type(meta_type<maybe<T>> m)
  {
    return get_proxy_type(m);
  }

  // ------------------------------------------
  // get_maybe_object_type

  template <class T>
  struct maybe_object_value;

  template <class T>
  using Maybe = Box<maybe_object_value<T>>;

  template <class T>
  [[nodiscard]] constexpr auto get_maybe_object_type(meta_type<ty_maybe<T>>)
  {
    return type_c<Maybe<T>>;
  }

  // ------------------------------------------
  // type gen

  /// proxy type of maybe
  template <class T>
  struct MaybeProxy : Object
  {
    /// term
    static constexpr auto term = make_tm_maybe(get_term<T>());
  };

  constexpr auto maybe_type_uuid()
  {
    return read_uuid_from_constexpr_string(
      "6b8bcbb1-6283-46c1-8b68-6e8ad6c99d46");
  }

  constexpr auto maybe_type_name()
  {
    return "Maybe";
  }

  template <>
  struct tcon_traits<maybe_tcon_tag>
  {
    static constexpr auto id   = maybe_type_uuid();
    static constexpr auto name = maybe_type_name();
  };

  template <class T>
  struct term_to_type<tm_maybe<T>>
  {
    using type = ty_maybe<term_to_type_t<T>>;
  };

  [[nodiscard]] inline auto maybe_type_tcon() -> object_ptr<const Type>
  {
    return &type_initializer<tcon<maybe_tcon_tag, kfun<kstar, kstar>>>::type;
  }

  template <class T>
  [[nodiscard]] constexpr auto guess_object_type(meta_type<ty_maybe<T>>)
  {
    return get_maybe_object_type(make_ty_maybe(guess_object_type(type_c<T>)));
  }

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

  // ------------------------------------------
  // dynamc typing

  [[nodiscard]] inline auto make_maybe_type(const object_ptr<const Type>& t)
  {
    return make_object<const Type>(tap_type {maybe_type_tcon(), t});
  }

  [[nodiscard]] inline auto is_maybe_type(const object_ptr<const Type>& t)
  {
    if (auto tap = is_tap_type_if(t))
      if (same_type(tap->t1, maybe_type_tcon()))
        return true;

    return false;
  }
} // namespace yave