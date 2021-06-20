//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/object_ptr.hpp>
#include <yave/rts/id_util.hpp>
#include <yave/rts/kind_gen.hpp>
#include <yave/core/type_traits.hpp>

#include <yave/core/offset_of.hpp>

#include <array>
#include <cstring>
#include <random>
#include <chrono>

namespace yave {

  // ------------------------------------------
  // TypeValue union values

  /// tcon type
  struct tcon_type
  {
    /// 128bit UUID.
    std::array<char, 16> id;

    /// kind
    object_ptr<const Kind> kind;

    /// Friendly name of this value type.
    const char* name;
  };

  [[nodiscard]] inline bool tcon_id_eq(
    std::array<char, 16> lhs,
    std::array<char, 16> rhs)
  {
    if constexpr (has_SSE) {
      auto xmm1 = _mm_loadu_si128((const __m128i*)lhs.data());
      auto xmm2 = _mm_loadu_si128((const __m128i*)rhs.data());
      auto cmp  = _mm_cmpeq_epi8(xmm1, xmm2);
      auto mask = _mm_movemask_epi8(cmp);
      return mask == 0xffffU;
    } else
      return lhs == rhs;
  }

  /// to_string
  [[nodiscard]] inline auto to_string(const tcon_type& v) -> std::string
  {
    std::string ret;

    ret += v.name;

    ret.reserve(ret.capacity() + 10);

    ret += '(';
    ret += uuid_to_string_short(v.id);
    ret += ')';

    return ret;
  }

  /// tap type
  struct tap_type
  {
    /// applied type
    object_ptr<const Type> t1;
    /// argument type
    object_ptr<const Type> t2;
  };

  /// tvar type
  struct tvar_type
  {
    /// unique id for VarTpye object
    uint64_t id;

    /// kind (always kstar now)
    object_ptr<const Kind> kind = detail::kind_address<kstar>();

    [[nodiscard]] static auto random_generate() -> tvar_type
    {
      // MinGW workaround: use <chrono> instead of random_device.
      static std::mt19937_64 mt(
        std::chrono::high_resolution_clock::now().time_since_epoch().count());
      return {mt()};
    }
  };

  [[nodiscard]] inline bool tvar_id_eq(uint64_t lhs, uint64_t rhs)
  {
    return lhs == rhs;
  }

  [[nodiscard]] inline auto to_string(const tvar_type& v) -> std::string
  {
    std::array<uint8_t, 8> buff;
    std::memcpy(&buff, &v.id, 8);
    return id_to_string_short(buff);
  }

  // ------------------------------------------
  // TypeValue

  struct type_value_storage
  {
    /// default ctor is disabled
    type_value_storage() = delete;

    // initializers
    type_value_storage(tcon_type t) noexcept
      : con {std::move(t)}
      , index {tcon_idx}
    {
    }

    type_value_storage(tap_type t) noexcept
      : ap {std::move(t)}
      , index {tap_idx}
    {
    }

    type_value_storage(tvar_type t) noexcept
      : var {std::move(t)}
      , index {tvar_idx}
    {
    }

    /// Copy constructor
    type_value_storage(const type_value_storage& other) noexcept
      : index {other.index}
    {
      // copy union
      if (other.index == tcon_idx)
        con = other.con;
      if (other.index == tap_idx)
        ap = other.ap;
      if (other.index == tvar_idx)
        var = other.var;
    }

    /// Destructor
    ~type_value_storage() noexcept
    {
      // call destructor
      if (index == tcon_idx)
        con.~tcon_type();
      if (index == tap_idx)
        ap.~tap_type();
      if (index == tvar_idx)
        var.~tvar_type();
    }

    template <class T>
    [[nodiscard]] static constexpr auto type_index() -> uint64_t
    {
      if constexpr (std::is_same_v<std::decay_t<T>, tcon_type>) {
        return tcon_idx;
      } else if constexpr (std::is_same_v<std::decay_t<T>, tap_type>) {
        return tap_idx;
      } else if constexpr (std::is_same_v<std::decay_t<T>, tvar_type>) {
        return tvar_idx;
      } else {
        static_assert(false_v<T>, "Invalid type of type value union");
      }
    }

    // hard-coded type index
    static constexpr uint64_t tcon_idx = 0;
    static constexpr uint64_t tap_idx  = 1;
    static constexpr uint64_t tvar_idx = 2;

    // union
    union
    {
      tcon_type con;
      tap_type ap;
      tvar_type var;
    };

    // 8 byte index
    uint64_t index;
  };

  static_assert(offset_of(&type_value_storage::con) == 0);
  static_assert(offset_of(&type_value_storage::ap) == 0);
  static_assert(offset_of(&type_value_storage::var) == 0);

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

    type_value(tcon_type t) noexcept
      : base {t}
    {
    }

    type_value(tap_type t) noexcept
      : base {t}
    {
    }

    type_value(tvar_type t) noexcept
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
    if constexpr (Idx == type_value_storage::tcon_idx) {
      auto&& ref = std::forward<T>(v).con;
      return ref;
    } else if constexpr (Idx == type_value_storage::tap_idx) {
      auto&& ref = std::forward<T>(v).ap;
      return ref;
    } else if constexpr (Idx == type_value_storage::tvar_idx) {
      auto&& ref = std::forward<T>(v).var;
      return ref;
    } else {
      static_assert(false_v<T>, "Invalid index of type value union");
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