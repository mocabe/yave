//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/box.hpp>
#include <yave/rts/apply.hpp>
#include <yave/core/offset_of.hpp>

namespace yave {

  // fwd

  template <uint64_t N>
  struct ClosureN;
  template <class Closure1 = ClosureN<1>>
  struct Closure;

  // ------------------------------------------
  // closure_type_traits

  /// Trait class for Object::obj_name for closure objects.
  template <class T>
  struct closure_type_traits
  {
    /// Object::obj_name can be used for ID of closures.
    static constexpr const char name[] = "00000000-0000-0000-0000-000000000000";
  };

  // ------------------------------------------
  // closure_info_table

  /// Info table for closure
  struct closure_info_table : object_info_table
  {
    /// Number of arguments
    const uint64_t n_args;
    /// vtable for code
    object_ptr<const Object> (*code)(const Closure<>*) noexcept;
  };

  // ------------------------------------------
  // Closure

  /// Base class of Closue objects.
  template <class Closure1>
  struct Closure : Object
  {
    /// Arity of this closure.
    mutable uint64_t arity;

    /// dummy term for value_cast
    static constexpr auto term = type_c<tm_value<Closure<>>>;

    /// Get closure info table
    [[nodiscard]] auto get_info_table() const
    {
      return static_cast<const closure_info_table*>(
        detail::clear_info_table_tag(info_table));
    }

    /// Get number of args
    [[nodiscard]] auto n_args() const noexcept
    {
      return get_info_table()->n_args;
    }

    /// PAP?
    [[nodiscard]] bool is_pap() const noexcept
    {
      return n_args() != arity;
    }

    /// get nth vertebrae
    [[nodiscard]] auto& vertebrae(uint64_t n) const noexcept
    {
      using arg_type = typename decltype(Closure1::spine)::value_type;
      // offset to first element of argument buffer
      constexpr uint64_t arg_offset = offset_of(&Closure1::spine);
      static_assert(arg_offset % sizeof(arg_type) == 0);
      // manually calc offset to avoid UB
      return ((arg_type*)this)[arg_offset / sizeof(arg_type) + n];
    }

    /// get nth argument
    [[nodiscard]] auto arg(uint64_t n) const noexcept -> const auto&
    {
      return _get_storage(*vertebrae(n)).arg();
    }

    /// Execute core with vtable function
    /// \note will modify mutable members
    [[nodiscard]] auto call() const noexcept
    {
      return get_info_table()->code(this);
    }

    /// for object_ptr<Closure<>>
    [[nodiscard]] auto& value() const
    {
      return *this;
    }
  };

  template <>
  struct object_type_traits<Closure<>>
  {
    /// for value_cast<Closure<>> on instance of Function
    static constexpr auto info_table_tag = detail::info_table_tags::_5;
  };

  /** \brief ClosureN
   * Contains static size array for incoming arguments.
   * Arguments will be filled from back to front.
   * So, first argument of closure will be LAST element of array for arguments.
   * If all arguments are passed, arity becomes zero and the closure is ready to
   * execute code.
   */
  template <uint64_t N>
  struct ClosureN : Closure<>
  {
    /// args
    mutable std::array<object_ptr<const Apply>, N> spine = {};

    /// get raw arg
    template <uint64_t Arg>
    [[nodiscard]] auto nth_arg() const noexcept
    {
      static_assert(offset_of(&Closure<>::arity) == sizeof(Object));
      static_assert(offset_of(&ClosureN::spine) == sizeof(Closure<>));
      static_assert(Arg < N, "Invalid index of argument");

      auto& app     = spine[N - Arg - 1];
      auto& storage = _get_storage(*app);

      if (storage.is_result())
        return storage.get_result();
      else
        return storage.arg();
    }
  };
}