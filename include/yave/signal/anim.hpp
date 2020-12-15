//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/box.hpp>

#include <yave/obj/frame_time/frame_time.hpp>
#include <yave/signal/specifier.hpp>

namespace yave {

  /// anim term
  template <class T>
  struct tm_anim;

  /// anim specifier
  template <class T>
  struct anim;

  /// anim tcon tag
  struct anim_tcon_tag;

  /// anim type
  template <class T>
  using ty_anim = tap<tcon<anim_tcon_tag, kfun<kstar, kstar>>, T>;

  /// anim proxy object
  template <class T>
  struct AnimProxy;

  // ------------------------------------------
  // meta_type

  template <class T>
  struct meta_type<tm_anim<T>>
  {
    using type = tm_anim<T>;

    constexpr auto t() const
    {
      return type_c<T>;
    }
  };

  template <class T>
  struct meta_type<anim<T>>
  {
    using type = anim<T>;

    constexpr auto t() const
    {
      return type_c<T>;
    }
  };

  // ------------------------------------------
  // helper

  template <class T>
  [[nodiscard]] constexpr auto make_tm_anim(meta_type<T>)
  {
    return type_c<tm_anim<T>>;
  }

  template <class T>
  [[nodiscard]] constexpr auto make_ty_anim(meta_type<T>)
  {
    return type_c<ty_anim<T>>;
  }

  // ------------------------------------------
  // generalize_tm_varvalue

  template <class T, class Target>
  constexpr auto generalize_tm_varvalue_impl(
    meta_type<tm_anim<T>> term,
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
    meta_type<tm_anim<T>> term)
  {
    return make_tm_anim(subst_term(from, to, term.t()));
  }

  // ------------------------------------------
  // genpoly_impl

  template <class T, class Gen, class Target>
  constexpr auto genpoly_impl(
    meta_type<tm_anim<T>> term,
    meta_type<Gen> gen,
    meta_type<Target> target)
  {
    return genpoly_impl(term.t(), gen, target);
  }

  // ------------------------------------------
  // type_of_impl

  template <class T, class Gen, bool Assert>
  constexpr auto type_of_impl(
    meta_type<tm_anim<T>> term,
    meta_type<Gen> gen,
    std::bool_constant<Assert> enable_assert)
  {
    auto p = type_of_impl(term.t(), gen, enable_assert);
    auto t = p.first();
    auto g = p.second();
    return make_pair(make_ty_anim(t), g);
  }

  // ------------------------------------------
  // specifier

  template <class T>
  [[nodiscard]] constexpr auto is_specifier(meta_type<anim<T>>)
  {
    return true_c;
  }

  template <class T>
  [[nodiscard]] constexpr auto normalize_specifier(meta_type<anim<T>> m)
  {
    return type_c<anim<typename decltype(normalize_specifier(m.t()))::type>>;
  }

  template <class T>
  [[nodiscard]] constexpr auto get_proxy_type(meta_type<anim<T>> m)
  {
    return type_c<AnimProxy<typename decltype(get_proxy_type(m.t()))::type>>;
  }

  // ------------------------------------------
  // get_anim_object_type

  template <class T>
  struct anim_object_value;

  template <class T>
  using Anim = Box<anim_object_value<T>>;

  template <class T>
  [[nodiscard]] constexpr auto get_anim_object_type(meta_type<ty_anim<T>>)
  {
    return type_c<Anim<T>>;
  }

  // ------------------------------------------
  // type gen

  /// proxy type of anim
  template <class T>
  struct AnimProxy : Object
  {
    /// term
    static constexpr auto term = make_tm_anim(get_term<T>());
  };

  constexpr auto anim_type_uuid()
  {
    return read_uuid_from_constexpr_string(
      "180f5a62-9030-44c1-932b-b152f6bad4c7");
  }

  constexpr auto anim_type_name()
  {
    return "Anim";
  }

  template <>
  struct tcon_traits<anim_tcon_tag>
  {
    static constexpr auto id   = anim_type_uuid();
    static constexpr auto name = anim_type_name();
  };

  template <class T>
  struct term_to_type<tm_anim<T>>
  {
    using type = ty_anim<term_to_type_t<T>>;
  };

  [[nodiscard]] inline auto anim_type_tcon() -> object_ptr<const Type>
  {
    return &type_initializer<tcon<anim_tcon_tag, kfun<kstar, kstar>>>::type;
  }

  template <class T>
  [[nodiscard]] constexpr auto guess_object_type(meta_type<ty_anim<T>>)
  {
    return get_anim_object_type(make_ty_anim(guess_object_type(type_c<T>)));
  }

  template <class T>
  struct object_type_traits<Anim<T>>
  {
    static constexpr const char name[] = "Anim<T>";
  };

  // ------------------------------------------
  // anim_storage

  struct anim_storage
  {
    /// any signal value.
    /// null when blank
    object_ptr<const Object> value;
    /// length of animation.
    /// never null
    object_ptr<const Object> length;
  };

  // ------------------------------------------
  // anim_object_value

  template <class T>
  struct anim_object_value
  {
    /// term
    static constexpr auto term = get_term<anim<T>>();

    /// storage access
    friend inline auto _get_storage(const anim_object_value& v) noexcept
      -> const anim_storage&
    {
      return *reinterpret_cast<const anim_storage*>(&(v.m_storage));
    }

    /// storage access
    friend inline auto _get_storage(anim_object_value& v) noexcept
      -> anim_storage&
    {
      return *reinterpret_cast<anim_storage*>(&(v.m_storage));
    }

    /// construct blank animation
    /// \param l length of animation
    template <class L>
    anim_object_value(object_ptr<L> l)
      : m_storage {nullptr, std::move(l)}
    {
      if (m_storage.length == nullptr)
        throw std::invalid_argument("Length of Anim<T> cannot be null");

      {
        constexpr auto lhs = type_of(get_term<signal<FrameTime>>());
        constexpr auto rhs = type_of(get_term<L>());

        static_assert(
          match(lhs, rhs).is_succ(),
          "Invalid argument type. Should result signal of FrameTime");
      }
    }

    /// construct animation
    /// \param v signal value which represents animated value
    /// \param l length of animation
    template <class V, class L>
    anim_object_value(object_ptr<V> v, object_ptr<L> l)
      : m_storage {std::move(v), std::move(l)}
    {
      if (m_storage.value == nullptr)
        throw std::invalid_argument("Value of Anim<T> canont be null");

      if (m_storage.length == nullptr)
        throw std::invalid_argument("Length of Anim<T> cannot be null");

      {
        constexpr auto lhs = type_of(get_term<signal<T>>());
        constexpr auto rhs = type_of(get_term<V>());

        static_assert(
          match(lhs, rhs).is_succ(),
          "Invalid argument type. Should result signal of T");
      }

      {
        constexpr auto lhs = type_of(get_term<signal<FrameTime>>());
        constexpr auto rhs = type_of(get_term<L>());

        static_assert(
          match(lhs, rhs).is_succ(),
          "Invalid argument type. Should result signal of FrameTime");
      }
    }

    /// blank?
    [[nodiscard]] auto is_blank() const
    {
      return _get_storage(*this).value == nullptr;
    }

    /// get signal value
    [[nodiscard]] auto value() const
    {
      assert(!is_blank());

      constexpr auto proxy =
        get_proxy_type(normalize_specifier(type_c<signal<T>>));

      using To = std::add_const_t<typename decltype(proxy)::type>;

      return static_object_cast<To>(_get_storage(*this).value);
    }

    /// get length
    [[nodiscard]] auto length() const
    {
      constexpr auto proxy =
        get_proxy_type(normalize_specifier(type_c<signal<FrameTime>>));

      using To = std::add_const_t<typename decltype(proxy)::type>;

      return static_object_cast<To>(_get_storage(*this).length);
    }

  private:
    /// storage
    anim_storage m_storage;
  };
} // namespace yave