//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/box.hpp>
#include <yave/rts/apply.hpp>

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

    /// Get number of args
    [[nodiscard]] auto n_args() const noexcept
    {
      return static_cast<const closure_info_table*>(info_table)->n_args;
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
      constexpr uint64_t offset = offset_of_member(&Closure1::spine);
      static_assert(offset % sizeof(arg_type) == 0);
      // manually calc offset to avoid UB
      return ((arg_type*)this)[offset / sizeof(arg_type) + n];
    }

    /// get nth argument
    [[nodiscard]] auto arg(uint64_t n) const noexcept -> const auto&
    {
      return _get_storage(*vertebrae(n)).arg();
    }

  public: /* can modify mutable members */
    /// Execute core with vtable function
    [[nodiscard]] auto call() const noexcept
    {
      return static_cast<const closure_info_table*>(info_table)->code(this);
    }
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
    /// get raw arg
    template <uint64_t Arg>
    [[nodiscard]] auto nth_arg() const noexcept
    {
      static_assert(offset_of_member(&Closure<>::arity) == sizeof(Object));
      static_assert(offset_of_member(&ClosureN::spine) == sizeof(Closure<>));
      static_assert(Arg < N, "Invalid index of argument");

      auto& app     = spine[N - Arg - 1];
      auto& storage = _get_storage(*app);

      if (storage.is_result())
        return storage.get_result();
      else
        return storage.arg();
    }

    /// args
    mutable std::array<object_ptr<const Apply>, N> spine = {};
  };
}