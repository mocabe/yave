//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/config/config.hpp>

#include <yave/support/id.hpp>

namespace yave {

  /// Simple wrapper for descriptor pointers.
  template <class Descriptor>
  class descriptor_handle
  {
  public:
    /// Constructor
    constexpr descriptor_handle() noexcept
      : m_descriptor {nullptr}
      , m_id {0}
    {
    }
    /// Constructor
    constexpr descriptor_handle(nullptr_t) noexcept
      : m_descriptor {nullptr}
      , m_id {0}
    {
    }
    /// Constructor
    explicit constexpr descriptor_handle(const Descriptor& d, uid id) noexcept
      : m_descriptor {d}
      , m_id {id}
    {
    }
    /// Copy Constructor
    constexpr descriptor_handle(const descriptor_handle&) = default;
    /// Move Constructor
    constexpr descriptor_handle(descriptor_handle&&) = default;
    /// operator=
    constexpr descriptor_handle& operator=(const descriptor_handle&) = default;
    /// operator=
    constexpr descriptor_handle& operator=(descriptor_handle&&) = default;

    /// check if having valid value
    [[nodiscard]] constexpr bool has_value() const noexcept
    {
      return static_cast<bool>(m_descriptor);
    }

    /// check if having valid value
    [[nodiscard]] constexpr explicit operator bool() const noexcept
    {
      return has_value();
    }

    /// Get descriptor value.
    [[nodiscard]] constexpr Descriptor descriptor() const noexcept
    {
      return m_descriptor;
    }

    /// Get descriptor id.
    [[nodiscard]] constexpr uid id() const noexcept
    {
      return m_id;
    }

  private:
    /// discriptor pointer
    Descriptor m_descriptor;
    /// id
    uid m_id;
  };

  /// operator==
  template <class D>
  [[nodiscard]] constexpr bool operator==(
    const descriptor_handle<D>& lhs,
    const descriptor_handle<D>& rhs) noexcept
  {
    if (lhs.id() == rhs.id()) {
      // comparing node handle from different node_graph instances
      assert(lhs.descriptor() == rhs.descriptor());
      return true;
    }
    return false;
  }

  /// operator!=
  template <class D>
  [[nodiscard]] constexpr bool operator!=(
    const descriptor_handle<D>& lhs,
    const descriptor_handle<D>& rhs) noexcept
  {
    return !(lhs == rhs);
  }

  /// operator<
  template <class D>
  [[nodiscard]] constexpr bool operator<(
    const descriptor_handle<D>& lhs,
    const descriptor_handle<D>& rhs) noexcept
  {
    return lhs.id() < rhs.id();
  }

  /// operator>
  template <class D>
  [[nodiscard]] constexpr bool operator>(
    const descriptor_handle<D>& lhs,
    const descriptor_handle<D>& rhs) noexcept
  {
    return lhs.id() > rhs.id();
  }

} // namespace yave