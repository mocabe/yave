//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/config/config.hpp>

namespace yave {

  /// Simple wrapper for descriptor pointers.
  template <class Descriptor>
  class DescriptorHandle
  {
  public:
    /// Constructor
    constexpr DescriptorHandle() noexcept
      : m_descriptor {nullptr}
      , m_id {0}
    {
    }
    /// Constructor
    constexpr DescriptorHandle(nullptr_t) noexcept
      : m_descriptor {nullptr}
      , m_id {0}
    {
    }
    /// Constructor
    explicit constexpr DescriptorHandle(const Descriptor& d, uid id) noexcept
      : m_descriptor {d}
      , m_id {id}
    {
    }
    /// Copy Constructor
    constexpr DescriptorHandle(const DescriptorHandle&) = default;
    /// Move Constructor
    constexpr DescriptorHandle(DescriptorHandle&&) = default;
    /// operator=
    constexpr DescriptorHandle& operator=(const DescriptorHandle&) = default;
    /// operator=
    constexpr DescriptorHandle& operator=(DescriptorHandle&&) = default;

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
    const DescriptorHandle<D>& lhs,
    const DescriptorHandle<D>& rhs) noexcept
  {
    return lhs.id() == rhs.id();
  }

  /// operator!=
  template <class D>
  [[nodiscard]] constexpr bool operator!=(
    const DescriptorHandle<D>& lhs,
    const DescriptorHandle<D>& rhs) noexcept
  {
    return lhs.id() != rhs.id();
  }

  /// operator<
  template <class D>
  [[nodiscard]] constexpr bool operator<(
    const DescriptorHandle<D>& lhs,
    const DescriptorHandle<D>& rhs) noexcept
  {
    return lhs.id() < rhs.id();
  }

  /// operator>
  template <class D>
  [[nodiscard]] constexpr bool operator>(
    const DescriptorHandle<D>& lhs,
    const DescriptorHandle<D>& rhs) noexcept
  {
    return lhs.id() > rhs.id();
  }

} // namespace yave