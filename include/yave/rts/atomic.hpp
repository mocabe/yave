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
  public:
    /// ctor
    constexpr atomic_refcount() noexcept
      : atomic {0}
    {
    }

    /// crot
    constexpr atomic_refcount(T v) noexcept
      : atomic {v}
    {
    }

    /// operatnr=
    atomic_refcount& operator=(const atomic_refcount& other) noexcept
    {
      store(other.load());
      return *this;
    }

    /// operator=
    atomic_refcount& operator=(T v) noexcept
    {
      store(v);
      return *this;
    }

    /// Atomic load with memory_order_relaxed
    [[nodiscard]] T load_relaxed() const noexcept
    {
      return atomic.load(std::memory_order_relaxed);
    }

    /// Atomic load with memory_order_acquire
    [[nodiscard]] T load_acquire() const noexcept
    {
      return atomic.load(std::memory_order_acquire);
    }

    /// Atomic store.
    void store(T v) noexcept
    {
      atomic.store(v, std::memory_order_release);
    }

    /// with memory_order_relaxed
    T fetch_add() noexcept
    {
      return atomic.fetch_add(1u, std::memory_order_relaxed);
    }

    /// with memory_order_release
    T fetch_sub() noexcept
    {
      return atomic.fetch_sub(1u, std::memory_order_release);
    }

  private:
    std::atomic<T> atomic;
    static_assert(std::atomic<T>::is_always_lock_free);
    static_assert(sizeof(T) == sizeof(std::atomic<T>));
  };

} // namespace yave