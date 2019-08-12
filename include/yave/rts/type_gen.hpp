//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/box_fwd.hpp>
#include <yave/rts/type_value.hpp>
#include <yave/rts/specifiers.hpp>

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
      closure_term_export(make_tm_closure(get_term<Ts>()...));
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

    /// read UUID from constexpr char array
    [[nodiscard]] constexpr std::array<char, 16>
      read_from_constexpr_string(char const (&str)[37])
    {
      // ex) 707186a4-f043-4a08-8223-e03fe9c1b0ea\0

      char hex[32] {};
      size_t hex_idx = 0;

      // for gcc constexpr bug workaround
      bool fail = false;

      // read hex
      for (auto&& c : str) {
        if (c == '-' || c == '\0') {
          continue;
        }
        if ('0' <= c && c <= '9') {
          hex[hex_idx] = static_cast<char>(c - '0');
          ++hex_idx;
          continue;
        }
        if ('a' <= c && c <= 'f') {
          hex[hex_idx] = static_cast<char>(c - 'a' + 10);
          ++hex_idx;
          continue;
        }
        if ('A' <= c && c <= 'F') {
          hex[hex_idx] = static_cast<char>(c - 'A' + 10);
          ++hex_idx;
          continue;
        }
        fail = true;
        break; // failed to parse UUID
      }

      if (fail)
        throw;

      std::array<char, 16> ret {};

      // convert to value
      for (size_t i = 0; i < 16; ++i) {
        auto upper = 2 * i;
        auto lower = 2 * i + 1;
        ret[i]     = static_cast<char>(hex[upper] * 16 + hex[lower]);
      }
      return ret;
    }

    /// get buffer for value type
    template <class T>
    constexpr const std::array<char, 16> value_type_uuid()
    {
      return read_from_constexpr_string(
        object_type_traits<typename decltype(type_c<T>.tag())::type>::uuid);
    }

    /// get friendly name of value type.
    template <class T>
    constexpr const char* value_type_name()
    {
      return object_type_traits<typename decltype(type_c<T>.tag())::type>::name;
    }

    /// value type
    template <class T>
    struct value_type_initializer
    {
      /// 16byte aligned uuid
      alignas(16) static constexpr const std::array<char, 16> aligned_uuid =
        value_type_uuid<T>();

      /// name
      static constexpr const char* friendly_name = value_type_name<T>();

      /// value type object
      inline static const Type type {static_construct,
                                     value_type {&aligned_uuid, friendly_name}};
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
      inline static const Type type {
        static_construct,
        arrow_type {object_type_impl(type_c<T>),
                    &arrow_type_initializer<Ts...>::type}};
    };

    template <class T1, class T2>
    struct arrow_type_initializer<T1, T2>
    {
      /// arrow type object
      inline static const Type type {static_construct,
                                     arrow_type {object_type_impl(type_c<T1>),
                                                 object_type_impl(type_c<T2>)}};
    };

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
      inline static const Type type {static_construct,
                                     var_type::random_generate()};
    };

    template <class T>
    constexpr auto var_type_address(meta_type<T>)
    {
      return &var_type_initializer<T>::type;
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
        return var_type_address(term);
      } else {
        static_assert(false_v<T>);
      }
    }

  } // namespace detail

  /// object type generator
  template <class T>
  [[nodiscard]] object_ptr<const Type> object_type()
  {
    constexpr auto spec = normalize_specifier(type_c<T>);
    constexpr auto tp   = get_proxy_type(spec);
    // get term through proxy type
    // TODO: use `consteval` in C++20
    constexpr auto ptr = detail::object_type_impl(get_term(tp));
    return ptr;
  }

  // ------------------------------------------
  // guess_object_type

  template <class T, class... Ts>
  constexpr auto guess_object_type_closure(
    meta_type<T> type,
    meta_type<ClosureProxy<Ts...>> result)
  {
    if constexpr (is_arrow_type(type)) {
      return guess_object_type_closure(
        type.t2(), append(guess_object_type(type.t1()), result));
    } else {
      return append(guess_object_type(type), result);
    }
  }

  /// Guess C++ type of a type.
  /// Unknown types will be converted into proxy.
  template <class T>
  constexpr auto guess_object_type(meta_type<T> type)
  {
    if constexpr (is_arrow_type(type)) {
      return guess_object_type_closure(type, type_c<ClosureProxy<>>);
    } else if constexpr (is_value_type(type)) {
      using tag = typename decltype(type.tag())::type;
      return get_object_type(type_c<ObjectProxy<tag>>);
    } else if constexpr (is_varvalue_type(type) || is_var_type(type)) {
      using tag = typename decltype(type.tag())::type;
      return type_c<VarValueProxy<tag>>;
    } else
      static_assert(false_v<T>, "Invalid type");
  }

} // namespace yave