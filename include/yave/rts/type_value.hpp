//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/object_ptr.hpp>
#include <yave/rts/id_util.hpp>

#include <yave/support/offset_of_member.hpp>

#include <array>
#include <cstring>
#include <random>
#include <chrono>

namespace yave {

  // ------------------------------------------
  // TypeValue union values

  /// Value type
  struct value_type
  {
    /// 128bit UUID.
    std::array<char, 16> data;

    /// Friendly name of this value type.
    const char* name;

    /// compare two value types
    [[nodiscard]] static bool equal(
      const value_type& lhs,
      const value_type& rhs) noexcept
    {
      if constexpr (has_SSE) {
        auto xmm1 = _mm_loadu_si128((const __m128i*)lhs.data.data());
        auto xmm2 = _mm_loadu_si128((const __m128i*)rhs.data.data());
        auto cmp  = _mm_cmpeq_epi8(xmm1, xmm2);
        auto mask = _mm_movemask_epi8(cmp);
        return mask == 0xffffU;
      } else
        return lhs.data == rhs.data;
    }
  };

  /// to_string
  [[nodiscard]] inline auto to_string(const value_type& v) -> std::string
  {
    std::string ret;

    ret += v.name;

    ret.reserve(ret.capacity() + 10);

    ret += '(';
    ret += uuid_to_string_short(v.data);
    ret += ')';

    return ret;
  }

  /// Arrow type
  struct arrow_type
  {
    /// argument type
    object_ptr<const Type> captured;
    /// return type
    object_ptr<const Type> returns;
  };

  /// Any type
  struct var_type
  {
    /// unique id for VarTpye object
    uint64_t id;

    [[nodiscard]] static auto random_generate() -> var_type
    {
      // MinGW workaround: use <chrono> instead of random_device.
      static std::mt19937_64 mt(
        std::chrono::high_resolution_clock::now().time_since_epoch().count());
      return {mt()};
    }
  };

  [[nodiscard]] inline auto to_string(const var_type& v) -> std::string
  {
    std::array<uint8_t, 8> buff;
    std::memcpy(&buff, &v.id, 8);
    return id_to_string_short(buff);
  }

  /// List type
  struct list_type
  {
    /// value type of list
    object_ptr<const Type> t;
  };

  // ------------------------------------------
  // TypeValue

  struct type_value_storage
  {

    /// default ctor is disabled
    type_value_storage() = delete;

    // initializers
    type_value_storage(value_type t) noexcept
      : value {std::move(t)}
      , index {value_index}
    {
    }

    type_value_storage(arrow_type t) noexcept
      : arrow {std::move(t)}
      , index {arrow_index}
    {
    }

    type_value_storage(var_type t) noexcept
      : var {std::move(t)}
      , index {var_index}
    {
    }

    type_value_storage(list_type t) noexcept
      : list {std::move(t)}
      , index {list_index}
    {
    }

    /// Copy constructor
    type_value_storage(const type_value_storage& other) noexcept
      : index {other.index}
    {
      // copy union
      if (other.index == value_index)
        value = other.value;
      if (other.index == arrow_index)
        arrow = other.arrow;
      if (other.index == var_index)
        var = other.var;
      if (other.index == list_index)
        list = other.list;
    }

    /// Destructor
    ~type_value_storage() noexcept
    {
      // call destructor
      if (index == value_index)
        value.~value_type();
      if (index == arrow_index)
        arrow.~arrow_type();
      if (index == var_index)
        var.~var_type();
      if (index == list_index)
        list.~list_type();
    }

    template <class T>
    [[nodiscard]] static constexpr auto type_index() -> uint64_t
    {
      if constexpr (std::is_same_v<std::decay_t<T>, value_type>) {
        return value_index;
      } else if constexpr (std::is_same_v<std::decay_t<T>, arrow_type>) {
        return arrow_index;
      } else if constexpr (std::is_same_v<std::decay_t<T>, var_type>) {
        return var_index;
      } else if constexpr (std::is_same_v<std::decay_t<T>, list_type>) {
        return list_index;
      } else {
        static_assert(false_v<T>, "Invalid type of type value union");
      }
    }

    // hard-coded type index
    static constexpr uint64_t value_index = 0;
    static constexpr uint64_t arrow_index = 1;
    static constexpr uint64_t var_index   = 2;
    static constexpr uint64_t list_index  = 3;

    // 16 byte union
    union
    {
      value_type value;
      arrow_type arrow;
      var_type var;
      list_type list;
    };

    // 8 byte index
    uint64_t index;
  };

  static_assert(offset_of_member(&type_value_storage::value) == 0);
  static_assert(offset_of_member(&type_value_storage::arrow) == 0);
  static_assert(offset_of_member(&type_value_storage::var) == 0);
  static_assert(offset_of_member(&type_value_storage::list) == 0);

  /// Base class for TypeValue
  class type_value : type_value_storage
  {
    friend auto _get_storage(const type_value&) noexcept //
      -> const type_value_storage&;
    friend auto _get_storage(type_value&) noexcept //
      -> type_value_storage&;
    friend auto _get_storage(const type_value&&) noexcept //
      -> const type_value_storage&&;
    friend auto _get_storage(type_value&&) noexcept //
      -> type_value_storage&&;

    using base = type_value_storage;

  public:
    /// default ctor is disabled
    type_value() = delete;

    type_value(value_type t) noexcept
      : base {t}
    {
    }

    type_value(arrow_type t) noexcept
      : base {t}
    {
    }

    type_value(var_type t) noexcept
      : base {t}
    {
    }

    type_value(list_type t) noexcept
      : base {t}
    {
    }

    type_value(const type_value& other) noexcept
      : base {other}
    {
    }
  };

  // ------------------------------------------
  // _get_storage

  [[nodiscard]] inline auto _get_storage(const type_value& v) noexcept
    -> const type_value_storage&
  {
    return v;
  }

  [[nodiscard]] inline auto _get_storage(type_value& v) noexcept
    -> type_value_storage&
  {
    return v;
  }

  [[nodiscard]] inline auto _get_storage(const type_value&& v) noexcept
    -> const type_value_storage&&
  {
    return std::move(v);
  }

  [[nodiscard]] inline auto _get_storage(type_value&& v) noexcept
    -> type_value_storage&&
  {
    return std::move(v);
  }

  // ------------------------------------------
  // primitive utility

  template <
    size_t Idx,
    class T,
    std::enable_if_t<
      std::is_same_v<std::decay_t<T>, type_value_storage>,
      nullptr_t>* = nullptr>
  [[nodiscard]] constexpr auto _access(T&& v) noexcept -> decltype(auto)
  {
    if constexpr (Idx == type_value_storage::value_index) {
      auto&& ref = std::forward<T>(v).value;
      return ref;
    } else if constexpr (Idx == type_value_storage::arrow_index) {
      auto&& ref = std::forward<T>(v).arrow;
      return ref;
    } else if constexpr (Idx == type_value_storage::var_index) {
      auto&& ref = std::forward<T>(v).var;
      return ref;
    } else if constexpr (Idx == type_value_storage::list_index) {
      auto&& ref = std::forward<T>(v).list;
      return ref;
    } else {
      static_assert(false_v<Idx>, "Invalid index of type value union");
    }
  }

  template <
    size_t Idx,
    class T,
    std::enable_if_t<
      std::is_same_v<std::decay_t<T>, type_value_storage>,
      nullptr_t>* = nullptr>
  [[nodiscard]] constexpr auto _get(T&& v) -> decltype(auto)
  {
    if (v.index != Idx)
      throw std::bad_cast();
    return _access<Idx>(std::forward<T>(v));
  }

  // ------------------------------------------
  // std::variant like interface

  /// std::get() equivalent
  template <class T>
  [[nodiscard]] decltype(auto) get(const type_value& val)
  {
    auto&& storage = _get_storage(val);
    return _get<type_value_storage::type_index<T>()>(storage);
  }

  /// std::get() equivalent
  template <class T>
  [[nodiscard]] decltype(auto) get(const type_value&& val)
  {
    auto&& storage = _get_storage(std::move(val));
    return _get<type_value_storage::type_index<T>()>(storage);
  }

  /// std::get() equivalent
  template <class T>
  [[nodiscard]] decltype(auto) get(type_value& val)
  {
    auto&& storage = _get_storage(val);
    return _get<type_value_storage::type_index<T>()>(storage);
  }

  /// std::get() equivalent
  template <class T>
  [[nodiscard]] decltype(auto) get(type_value&& val)
  {
    auto&& storage = _get_storage(std::move(val));
    return _get<type_value_storage::type_index<T>()>(storage);
  }

  /// std::get_if() equivalent
  template <class T>
  [[nodiscard]] constexpr auto get_if(const type_value* val) noexcept
    -> std::add_pointer_t<const T>
  {
    constexpr auto Idx = type_value_storage::type_index<T>();
    if (val && Idx == _get_storage(*val).index)
      return &get<T>(*val);
    return nullptr;
  }

  /// std::get_if() equivalent
  template <class T>
  [[nodiscard]] constexpr auto get_if(type_value* val) noexcept
    -> std::add_pointer_t<T>
  {
    constexpr auto Idx = type_value_storage::type_index<T>();
    if (val && Idx == _get_storage(*val).index)
      return &get<T>(*val);
    return nullptr;
  }

} // namespace yave