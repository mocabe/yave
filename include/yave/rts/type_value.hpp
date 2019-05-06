//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/object_ptr.hpp>

#include <yave/tools/id.hpp>

#include <array>
#include <cstring>

namespace yave {

  // ------------------------------------------
  // TypeValue union values

  /// Value type
  struct value_type
  {
    /// Max name length
    static constexpr uint64_t buffer_size = 32;
    /// buffer type
    using buffer_type = std::array<char, buffer_size>;
    /// buffer
    const buffer_type* name;

    /// get C-style string
    const char* c_str() const
    {
      return name->data();
    }

    /// compare two value types
    [[nodiscard]] static bool
      compare(const value_type& lhs, const value_type& rhs)
    {
      if constexpr (has_AVX2 && buffer_size == 32) {
        // AVX2
        // buffers should be aligned as 32byte
        // load each buffers into 256-bit registers
        auto ymm0 =
          _mm256_load_si256(reinterpret_cast<const __m256i*>(lhs.name->data()));
        auto ymm1 =
          _mm256_load_si256(reinterpret_cast<const __m256i*>(rhs.name->data()));
        // compare
        auto cmpeq = _mm256_cmpeq_epi8(ymm0, ymm1);
        // get mask
        unsigned mask = _mm256_movemask_epi8(cmpeq);
        // clear upper bits for other SIMD operations
        _mm256_zeroupper();
        return mask == 0xffffffffU;
      } else if constexpr (has_AVX && buffer_size == 32) {
        // AVX
        // buffers should be aligned as 16byte
        // load buffer into 2 xmm registers
        auto xmm0 = _mm_load_si128(
          reinterpret_cast<const __m128i*>(lhs.name->data() + 0));
        auto xmm1 = _mm_load_si128(
          reinterpret_cast<const __m128i*>(lhs.name->data() + 16));
        // compare registers to another buffer on memory
        auto cmp1 = _mm_cmpeq_epi8(
          xmm0, *reinterpret_cast<const __m128i*>(rhs.name->data() + 0));
        auto cmp2 = _mm_cmpeq_epi8(
          xmm1, *reinterpret_cast<const __m128i*>(rhs.name->data() + 16));
        // get cmp result
        auto cmp = _mm_and_si128(cmp1, cmp2);
        // get mask
        auto mask = _mm_movemask_epi8(cmp);
        return mask == 0xffffU;
      } else {
        // fallback to memcmp
        return std::memcmp(        //
                 lhs.name->data(), //
                 rhs.name->data(), //
                 buffer_size) == 0;
      }
    }
  };

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
    uid id;
  };

  // ------------------------------------------
  // TypeValue

  struct type_object_value_storage
  {

    /// default ctor is disabled
    type_object_value_storage() = delete;

    // initializers
    type_object_value_storage(value_type t)
      : value {std::move(t)}
      , index {value_index}
    {
    }

    type_object_value_storage(arrow_type t)
      : arrow {std::move(t)}
      , index {arrow_index}
    {
    }

    type_object_value_storage(var_type t)
      : var {std::move(t)}
      , index {var_index}
    {
    }

    /// Copy constructor
    type_object_value_storage(const type_object_value_storage& other)
      : index {other.index}
    {
      // copy union
      if (other.index == value_index) {
        value = other.value;
      }
      if (other.index == arrow_index) {
        arrow = other.arrow;
      }
      if (other.index == var_index) {
        var = other.var;
      }
    }

    /// Destructor
    ~type_object_value_storage() noexcept
    {
      // call destructor
      if (index == value_index)
        value.~value_type();
      if (index == arrow_index)
        arrow.~arrow_type();
      if (index == var_index)
        var.~var_type();
    }

    template <class T>
    [[nodiscard]] static constexpr uint64_t type_index()
    {
      if constexpr (std::is_same_v<std::decay_t<T>, value_type>) {
        return value_index;
      } else if constexpr (std::is_same_v<std::decay_t<T>, arrow_type>) {
        return arrow_index;
      } else if constexpr (std::is_same_v<std::decay_t<T>, var_type>) {
        return var_index;
      } else {
        static_assert(false_v<T>);
      }
    }

    // hard-coded type index
    static constexpr uint64_t value_index = 0;
    static constexpr uint64_t arrow_index = 1;
    static constexpr uint64_t var_index   = 2;

    // 16 byte union
    union
    {
      value_type value;
      arrow_type arrow;
      var_type var;
    };

    // 8 byte index
    uint64_t index;
  };

  /// Base class for TypeValue
  class type_object_value : type_object_value_storage
  {
    friend const type_object_value_storage&    //
      _get_storage(const type_object_value&);  //
                                               //
    friend type_object_value_storage&          //
      _get_storage(type_object_value&);        //
                                               //
    friend const type_object_value_storage&&   //
      _get_storage(const type_object_value&&); //
                                               //
    friend type_object_value_storage&&         //
      _get_storage(type_object_value&&);       //

    using base = type_object_value_storage;

  public:
    /// default ctor is disabled
    type_object_value() = delete;

    type_object_value(value_type t) noexcept
      : base {t}
    {
    }

    type_object_value(arrow_type t) noexcept
      : base {t}
    {
    }

    type_object_value(var_type t) noexcept
      : base {t}
    {
    }

    type_object_value(const type_object_value& other) noexcept
      : base {other}
    {
    }
  };

  // ------------------------------------------
  // _get_storage

  [[nodiscard]] inline const type_object_value_storage&
    _get_storage(const type_object_value& v)
  {
    return v;
  }

  [[nodiscard]] inline type_object_value_storage&
    _get_storage(type_object_value& v)
  {
    return v;
  }

  [[nodiscard]] inline const type_object_value_storage&&
    _get_storage(const type_object_value&& v)
  {
    return std::move(v);
  }

  [[nodiscard]] inline type_object_value_storage&&
    _get_storage(type_object_value&& v)
  {
    return std::move(v);
  }

  // ------------------------------------------
  // primitive utility

  template <
    size_t Idx,
    class T,
    std::enable_if_t<
      std::is_same_v<std::decay_t<T>, type_object_value_storage>,
      nullptr_t>* = nullptr>
  [[nodiscard]] constexpr decltype(auto) _access(T&& v)
  {
    if constexpr (Idx == type_object_value_storage::value_index) {
      auto&& ref = std::forward<T>(v).value;
      return ref;
    } else if constexpr (Idx == type_object_value_storage::arrow_index) {
      auto&& ref = std::forward<T>(v).arrow;
      return ref;
    } else if constexpr (Idx == type_object_value_storage::var_index) {
      auto&& ref = std::forward<T>(v).var;
      return ref;
    } else {
      static_assert(false_v<Idx>, "Invalid index for TypeValue");
    }
  }

  template <
    size_t Idx,
    class T,
    std::enable_if_t<
      std::is_same_v<std::decay_t<T>, type_object_value_storage>,
      nullptr_t>* = nullptr>
  [[nodiscard]] constexpr decltype(auto) _get(T&& v)
  {
    if (v.index != Idx)
      throw std::bad_cast();
    return _access<Idx>(std::forward<T>(v));
  }

  // ------------------------------------------
  // std::variant like interface

  /// std::get() equivalent
  template <class T>
  [[nodiscard]] decltype(auto) get(const type_object_value& val)
  {
    auto&& storage = _get_storage(val);
    return _get<type_object_value_storage::type_index<T>()>(storage);
  }

  /// std::get() equivalent
  template <class T>
  [[nodiscard]] decltype(auto) get(const type_object_value&& val)
  {
    auto&& storage = _get_storage(std::move(val));
    return _get<type_object_value_storage::type_index<T>()>(storage);
  }

  /// std::get() equivalent
  template <class T>
  [[nodiscard]] decltype(auto) get(type_object_value& val)
  {
    auto&& storage = _get_storage(val);
    return _get<type_object_value_storage::type_index<T>()>(storage);
  }

  /// std::get() equivalent
  template <class T>
  [[nodiscard]] decltype(auto) get(type_object_value&& val)
  {
    auto&& storage = _get_storage(std::move(val));
    return _get<type_object_value_storage::type_index<T>()>(storage);
  }

  /// std::get_if() equivalent
  template <class T>
  [[nodiscard]] constexpr std::add_pointer_t<const T>
    get_if(const type_object_value* val)
  {
    constexpr auto Idx = type_object_value_storage::type_index<T>();
    if (val && Idx == _get_storage(*val).index)
      return &get<T>(*val);
    return nullptr;
  }

  /// std::get_if() equivalent
  template <class T>
  [[nodiscard]] constexpr std::add_pointer_t<T> get_if(type_object_value* val)
  {
    constexpr auto Idx = type_object_value_storage::type_index<T>();
    if (val && Idx == _get_storage(*val).index)
      return &get<T>(*val);
    return nullptr;
  }

} // namespace yave