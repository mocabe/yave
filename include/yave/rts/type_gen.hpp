//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/box_fwd.hpp>
#include <yave/rts/type_value.hpp>
#include <yave/rts/specifiers.hpp>
#include <yave/rts/id_util.hpp>
#include <yave/rts/kind_gen.hpp>

#include <yave/support/offset_of_member.hpp>

#include <cstring>
#include <string>
#include <memory>
#include <vector>
#include <utility>
#include <exception>
#include <array>

/// type
YAVE_DECL_TYPE(yave::Type, "7c6b944f-a5da-41f8-b51d-e39b6a615953");
/// Object
YAVE_DECL_TYPE(yave::Object, "8b0ab920-cedc-458d-987b-e654c643d217");

namespace yave {

  // ------------------------------------------
  // proxy types

  /// proxy type of arbitary closure type
  template <class... Ts>
  struct ClosureProxy : Object
  {
    /// term
    static constexpr auto term =
      polymorphic_term_export(make_tm_closure(get_term<Ts>()...));
  };

  /// proxy type of argument closure type
  template <class... Ts>
  struct ClosureArgumentProxy : Object
  {
    /// term
    static constexpr auto term = make_tm_closure(get_term<Ts>()...);
  };

  /// proy type of instance of type variable
  template <class Tag>
  struct VarValueProxy : Object
  {
    /// term
    static constexpr auto term = type_c<tm_varvalue<Tag>>;
  };

  /// proxy type of list
  template <class T>
  struct ListProxy : Object
  {
    /// term
    static constexpr auto term = make_tm_list(get_term<T>());
  };

  /// proxy type of named objec type
  template <class T>
  struct ObjectProxy : Object
  {
    // term
    static constexpr auto term = get_term<T>();
  };

  namespace detail {

    // fwd
    template <class>
    struct type_initializer;

    // fwd
    template <class>
    struct term_to_type;

    template <class T>
    using term_to_type_t = typename term_to_type<T>::type;

    // ------------------------------------------
    // tcon type

    template <class Tag>
    struct tcon_traits;

    template <class Tag, class Kind>
    struct type_initializer<tcon<Tag, Kind>>
    {
      inline static const Type type //
        {static_construct,
         tcon_type {tcon_traits<Tag>::id,
                    kind_address<kstar>(),
                    tcon_traits<Tag>::name}};
    };

    // ------------------------------------------
    // tap type

    template <class T1, class T2>
    struct type_initializer<tap<T1, T2>>
    {
      inline static const Type type //
        {static_construct,
         tap_type {&type_initializer<T1>::type, &type_initializer<T2>::type}};
    };

    // ------------------------------------------
    // tvar type

    template <class Tag, class Kind>
    struct type_initializer<tvar<Tag, Kind>>
    {
      inline static const Type type //
        {static_construct, tvar_type::random_generate()};
    };

    // ------------------------------------------
    // value type

    /// get buffer for value type
    template <class Tag>
    constexpr auto value_type_uuid() -> std::array<char, 16>
    {
      return detail::read_uuid_from_constexpr_string(
        object_type_traits<Tag>::uuid);
    }

    /// get friendly name of value type.
    template <class Tag>
    constexpr auto value_type_name() -> const char*
    {
      return object_type_traits<Tag>::name;
    }

    template <class Tag>
    struct tcon_traits<value_tcon_tag<Tag>>
    {
      static constexpr auto id   = value_type_uuid<Tag>();
      static constexpr auto name = value_type_name<Tag>();
    };

    template <class Tag>
    struct term_to_type<tm_value<Tag>>
    {
      using type = ty_value<Tag>;
    };

    // ------------------------------------------
    // arrow type

    constexpr auto arrow_type_uuid()
    {
      return read_uuid_from_constexpr_string(
        "c41c249c-4bb1-4fe7-80c8-f9e0c9780f45");
    }

    constexpr auto arrow_type_name()
    {
      return "ty_arrow";
    }

    template <>
    struct tcon_traits<arrow_tcon_tag>
    {
      static constexpr auto id   = arrow_type_uuid();
      static constexpr auto name = arrow_type_name();
    };

    template <class T1, class... Ts>
    struct term_to_type<tm_closure<T1, Ts...>>
    {
      using type =
        ty_arrow<term_to_type_t<T1>, term_to_type_t<tm_closure<Ts...>>>;
    };

    template <class T>
    struct term_to_type<tm_closure<T>>
    {
      using type = term_to_type_t<T>;
    };

    // ------------------------------------------
    // var type

    template <class Tag>
    struct term_to_type<tm_var<Tag>>
    {
      using type = ty_var<Tag>;
    };

    // ------------------------------------------
    // list type

    constexpr auto list_type_uuid()
    {
      return read_uuid_from_constexpr_string(
        "d14b9346-a02d-4a53-aaa5-64cdf3f2e4b3");
    }

    constexpr auto list_type_name()
    {
      return "ty_list";
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

    // ------------------------------------------
    // constexpr version of object_type type

    template <class Term>
    constexpr auto object_type_impl(meta_type<Term>)
    {
      return &type_initializer<term_to_type_t<Term>>::type;
    }

  } // namespace detail

  /// object type generator
  template <class T>
  [[nodiscard]] auto object_type() noexcept -> object_ptr<const Type>
  {
    constexpr auto ptr = detail::object_type_impl(get_term(type_c<T>));
    return ptr;
  }

  // ------------------------------------------
  // guess_object_type

  template <class T, class... Ts>
  [[nodiscard]] constexpr auto guess_object_type_closure(
    meta_type<T> type,
    meta_type<ClosureProxy<Ts...>> result)
  {
    return append(guess_object_type(type), result);
  }

  template <class T1, class T2, class... Ts>
  [[nodiscard]] constexpr auto guess_object_type_closure(
    meta_type<ty_arrow<T1, T2>>,
    meta_type<ClosureProxy<Ts...>> result)
  {
    return guess_object_type_closure(
      type_c<T2>, append(guess_object_type(type_c<T1>), result));
  }

  /// Guess C++ type of a type.
  /// Unknown types will be converted into proxy.
  template <class T>
  [[nodiscard]] constexpr auto guess_object_type(meta_type<ty_list<T>>)
  {
    return get_list_object_type(make_ty_list(guess_object_type(type_c<T>)));
  }

  template <class T1, class T2>
  [[nodiscard]] constexpr auto guess_object_type(
    meta_type<ty_arrow<T1, T2>> type)
  {
    return guess_object_type_closure(type, type_c<ClosureProxy<>>);
  }

  template <class Tag>
  [[nodiscard]] constexpr auto guess_object_type(meta_type<ty_value<Tag>> type)
  {
    return get_value_object_type(type);
  }

  template <class Tag>
  [[nodiscard]] constexpr auto guess_object_type(meta_type<ty_varvalue<Tag>>)
  {
    return type_c<VarValueProxy<Tag>>;
  }

  template <class T>
  [[nodiscard]] constexpr auto guess_object_type(meta_type<ty_var<T>>)
  {
    return type_c<VarValueProxy<T>>;
  }

} // namespace yave