//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/object_ptr.hpp>
#include <yave/rts/type_value.hpp> // clang requires definition of TypeValue to compile.
#include <yave/rts/specifiers.hpp>
#include <yave/rts/terms.hpp>

#include <cassert>

namespace yave {

  /// object type traits
  template <class T>
  struct object_type_traits;

} // namespace yave

// define system type names
#define YAVE_DECL_TYPE(TYPE, UUID)        \
  template <>                             \
  struct yave::object_type_traits<TYPE>   \
  {                                       \
    static constexpr char name[] = #TYPE; \
    static constexpr char uuid[] = UUID;  \
  }

namespace yave {

  // forward decl
  template <class T>
  [[nodiscard]] auto object_type() noexcept -> object_ptr<const Type>;

  namespace detail {

    /// \brief vtable function to delete object
    ///
    /// vtable function to delete heap allocated object.
    template <class T>
    void vtbl_destroy_func(const Object *obj) noexcept
    {
      static_assert(
        std::is_nothrow_destructible_v<T>,
        "Boxed object should have nothrow destructor");
      auto *p = static_cast<const T *>(obj);
      delete p;
    }

    /// \brief vtable function to clone object.
    ///
    /// vtable function to clone heap-allocated object.
    /// \param T value type
    /// \returns pointer to generated object.
    /// \notes return nullptr when allocation/initialization failed.
    template <class T>
    auto vtbl_clone_func(const Object *obj) noexcept -> Object *
    {
      try {
        auto p = static_cast<const T *>(obj);
        return new (std::nothrow) T {*p};
      } catch (...) {
        // TODO: return Exception object
        return nullptr;
      }
    }

    /// inherit custom term from parameter type
    template <class T>
    constexpr auto inherit_box_term()
    {
      if constexpr (has_term<T>())
        return T::term;
      else
        return type_c<tm_value<Box<T>>>;
    }

  } // namespace detail

  /// tag type to initialize object with 0 reference count
  struct static_construct_t
  {
    explicit static_construct_t() = default;
  };

  /// static_construct
  inline constexpr static_construct_t static_construct = static_construct_t();

  /// \brief Heap-allocated object generator.
  /// \param T value type
  template <class T>
  struct Box : Object
  {
    /// value type
    using value_type = T;
    /// term
    static constexpr auto term = detail::inherit_box_term<T>();

    /// Ctor
    template <
      class U,
      class... Args,
      class = std::enable_if_t<
        !std::is_same_v<std::decay_t<U>, Box> &&
        !std::is_same_v<std::decay_t<U>, static_construct_t>>>
    constexpr Box(U &&u, Args &&... args) //
      noexcept(std::is_nothrow_constructible_v<T, U, Args...>)
      : Object {info_table_initializer::get_info_table()}
      , m_value {std::forward<U>(u), std::forward<Args>(args)...}
    {
    }

    /// Ctor (static initialization)
    template <class... Args>
    constexpr Box(static_construct_t, Args &&... args) //
      noexcept(std::is_nothrow_constructible_v<Box, Args...>)
      : Box(std::forward<Args>(args)...)
    {
      // set refcount ZERO to avoid deletion
      refcount = 0u;
    }

    /// Ctor
    constexpr Box() //
      noexcept(std::is_nothrow_constructible_v<T>)
      : Object {info_table_initializer::get_info_table()}
      , m_value {}
    {
    }

    /// Copy ctor
    constexpr Box(const Box &obj) //
      noexcept(std::is_nothrow_copy_constructible_v<T>)
      : Object {info_table_initializer::get_info_table()}
      , m_value {obj.m_value}
    {
    }

    /// Move ctor
    constexpr Box(Box &&obj) //
      noexcept(std::is_nothrow_move_constructible_v<T>)
      : Object {info_table_initializer::get_info_table()}
      , m_value {std::move(obj.m_value)}
    {
    }

    /// operator=
    constexpr Box &operator=(const Box &obj) //
      noexcept(std::is_nothrow_copy_assignable_v<T>)
    {
      m_value = obj.m_value;
      return *this;
    }

    /// operator=
    constexpr Box &operator=(Box &&obj) //
      noexcept(std::is_nothrow_move_assignable_v<T>)
    {
      m_value = std::move(obj.m_value);
      return *this;
    }

    /// value getter
    auto value() noexcept -> value_type &
    {
      return m_value;
    }

    /// value getter
    auto value() const noexcept -> const value_type &
    {
      return m_value;
    }

  private:
    /// value
    value_type m_value;

  private:
    /// info table initializer
    struct info_table_initializer
    {
      // check m_value's offset
      static_assert(offset_of_member(&Box::m_value) == sizeof(Object));

      /// get info table pointer
      static constexpr auto get_info_table() -> const object_info_table *
      {
        return &info_table;
      }

    private:
      /// static object info table
      alignas(32) inline static const object_info_table info_table {
        object_type<Box>(),             //
        sizeof(Box),                    //
        object_type_traits<Box>::name,  //
        detail::vtbl_destroy_func<Box>, //
        detail::vtbl_clone_func<Box>};  //
    };
  };

} // namespace yave