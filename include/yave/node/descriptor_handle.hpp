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
    constexpr DescriptorHandle(nullptr_t) noexcept
      : m_descriptor {nullptr}
    {
    }
    /// Constructor
    constexpr DescriptorHandle(const Descriptor& d) noexcept
      : m_descriptor {d}
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
    [[nodiscard]] constexpr std::uintptr_t id() const noexcept
    {
      static_assert(sizeof(Descriptor) <= sizeof(uintptr_t));
      return reinterpret_cast<std::uintptr_t>(m_descriptor);
    }

  private:
    /// discriptor pointer
    Descriptor m_descriptor;
  };

  /// operator==
  template <class D>
  [[nodiscard]] constexpr bool operator==(
    const DescriptorHandle<D>& lhs,
    const DescriptorHandle<D>& rhs) noexcept
  {
    return lhs.descriptor() == rhs.descriptor();
  }

  /// operator!=
  template <class D>
  [[nodiscard]] constexpr bool operator!=(
    const DescriptorHandle<D>& lhs,
    const DescriptorHandle<D>& rhs) noexcept
  {
    return lhs.descriptor() != rhs.descriptor();
  }

  /// operator<
  template <class D>
  [[nodiscard]] constexpr bool operator<(
    const DescriptorHandle<D>& lhs,
    const DescriptorHandle<D>& rhs) noexcept
  {
    return lhs.descriptor() < rhs.descriptor();
  }

  /// operator>
  template <class D>
  [[nodiscard]] constexpr bool operator>(
    const DescriptorHandle<D>& lhs,
    const DescriptorHandle<D>& rhs) noexcept
  {
    return lhs.descriptor() > rhs.descriptor();
  }

} // namespace yave