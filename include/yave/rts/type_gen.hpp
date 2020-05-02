//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/box_fwd.hpp>
#include <yave/rts/type_value.hpp>
#include <yave/rts/specifiers.hpp>
#include <yave/rts/id_util.hpp>

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
    template <class T>
    constexpr auto object_type_impl(meta_type<T> term);

    // ------------------------------------------
    // value type

    /// get buffer for value type
    template <class T>
    constexpr auto value_type_uuid() -> std::array<char, 16>
    {
      return detail::read_uuid_from_constexpr_string(
        object_type_traits<typename decltype(type_c<T>.tag())::type>::uuid);
    }

    /// get friendly name of value type.
    template <class T>
    constexpr auto value_type_name() -> const char*
    {
      return object_type_traits<typename decltype(type_c<T>.tag())::type>::name;
    }

    /// value type
    template <class T>
    struct value_type_initializer
    {
      /// value type object
      alignas(64) inline static const Type type {
        static_construct,
        value_type {value_type_uuid<T>(), value_type_name<T>()}};
    };

    template <class T>
    constexpr auto value_type_address(meta_type<T>)
    {
      return &value_type_initializer<T>::type;
    }

    // ------------------------------------------
    // arrow type

    template <class T, class... Ts>
    struct arrow_type_initializer
    {
      /// arrow type object
      alignas(64) static const Type type;
    };

    template <class T1, class T2>
    struct arrow_type_initializer<T1, T2>
    {
      /// arrow type object
      alignas(64) static const Type type;
    };

    template <class T, class... Ts>
    alignas(64) const Type arrow_type_initializer<T, Ts...>::type {
      static_construct,
      arrow_type {object_type_impl(type_c<T>),
                  &arrow_type_initializer<Ts...>::type}};

    template <class T1, class T2>
    alignas(64) const Type arrow_type_initializer<T1, T2>::type {
      static_construct,
      arrow_type {object_type_impl(type_c<T1>), object_type_impl(type_c<T2>)}};

    template <class... Ts>
    constexpr auto arrow_type_address(meta_type<tm_closure<Ts...>>)
    {
      return &arrow_type_initializer<Ts...>::type;
    }

    // ------------------------------------------
    // var type

    /// var type
    template <class T>
    struct var_type_initializer
    {
      /// var type object.
      alignas(64) static const Type type;
    };

    template <class T>
    alignas(64) const Type var_type_initializer<T>::type {
      static_construct,
      var_type::random_generate()};

    template <class T>
    constexpr auto var_type_address(meta_type<T>)
    {
      return &var_type_initializer<T>::type;
    }

    // ------------------------------------------
    // list type

    template <class T>
    struct list_type_initializer
    {
      /// list type object
      alignas(64) static const Type type;
    };

    template <class T>
    alignas(64) const Type list_type_initializer<T>::type {
      static_construct,
      list_type {object_type_impl(type_c<T>)}};

    template <class T>
    constexpr auto list_type_address(meta_type<tm_list<T>>)
    {
      return &list_type_initializer<T>::type;
    }

    // ------------------------------------------
    // constexpr version of object_type type

    template <class T>
    constexpr auto object_type_impl(meta_type<T> term)
    {
      if constexpr (is_tm_value(term)) {
        return value_type_address(term);
      } else if constexpr (is_tm_closure(term)) {
        return arrow_type_address(term);
      } else if constexpr (is_tm_var(term)) {
        return var_type_address(term);
      } else if constexpr (is_tm_varvalue(term)) {
        return var_type_address(term); // fallback to var_type
      } else if constexpr (is_tm_list(term)) {
        return list_type_address(term);
      } else {
        static_assert(false_v<T>, "Invalid term");
      }
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