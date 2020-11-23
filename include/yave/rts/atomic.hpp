//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <atomic>
#include <mutex>

#include <yave/config/intrin.hpp>

namespace yave {

  /// Atomic reference count.
  template <class T>
  class atomic_refcount
  {
    static_assert(std::atomic_ref<T>::is_always_lock_free);
    static_assert(std::atomic_ref<T>::required_alignment == alignof(T));

  public:
    /// ctor
    constexpr atomic_refcount() noexcept
      : m_val {0}
    {
    }

    /// crot
    constexpr atomic_refcount(T v) noexcept
      : m_val {v}
    {
    }

    /// operatnr=
    atomic_refcount& operator=(const atomic_refcount& other) noexcept
    {
      m_val = other.m_val;
      return *this;
    }

    /// operator=
    atomic_refcount& operator=(T v) noexcept
    {
      m_val = v;
      return *this;
    }

    /// Atomic load with memory_order_relaxed
    [[nodiscard]] T load_relaxed() const noexcept
    {
      const auto ref = std::atomic_ref(const_cast<T&>(m_val));
      return ref.load(std::memory_order_relaxed);
    }

    /// Atomic load with memory_order_acquire
    [[nodiscard]] T load_acquire() const noexcept
    {
      const auto ref = std::atomic_ref(const_cast<T&>(m_val));
      return ref.load(std::memory_order_acquire);
    }

    /// Atomic store.
    void store(T v) noexcept
    {
      auto ref = std::atomic_ref(m_val);
      ref.store(v, std::memory_order_release);
    }

    /// with memory_order_relaxed
    T fetch_add() noexcept
    {
      auto ref = std::atomic_ref(m_val);
      return ref.fetch_add(1u, std::memory_order_relaxed);
    }

    /// with memory_order_release
    T fetch_sub() noexcept
    {
      auto ref = std::atomic_ref(m_val);
      return ref.fetch_sub(1u, std::memory_order_release);
    }

  private:
    T m_val;
  };

} // namespace yave