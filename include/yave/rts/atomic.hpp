//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <atomic>
#include <mutex>

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

    /// Atomic load.
    T load() const noexcept
    {
      return atomic.load(std::memory_order_acquire);
    }

    /// Atomic store.
    void store(T v) noexcept
    {
      atomic.store(v, std::memory_order_release);
    }

    /// use memory_order_relaxed
    T fetch_add() noexcept
    {
      return atomic.fetch_add(1u, std::memory_order_relaxed);
    }

    /// use memory_order_release
    T fetch_sub() noexcept
    {
      return atomic.fetch_sub(1u, std::memory_order_release);
    }

  private:
    std::atomic<T> atomic;
    static_assert(std::atomic<T>::is_always_lock_free);
    static_assert(sizeof(T) == sizeof(std::atomic<T>));
  };

  /// Atomic spin lock.
  template <class T>
  class atomic_spinlock
  {
  public:
    /// ctor
    constexpr atomic_spinlock() noexcept
      : m_atomic {0u}
    {
    }

    /// ctor
    constexpr atomic_spinlock(bool flg) noexcept
      : m_atomic {flg ? 1u : 0u}
    {
    }

    /// Lock.
    void lock() noexcept
    {
      while (test_and_set()) {
        /* spin lock */
      }
    }

    /// Unlock.
    void unlock() noexcept
    {
      m_atomic.store(0u, std::memory_order_release);
    }

  private:
    bool test_and_set() noexcept
    {
      return m_atomic.exchange(1u, std::memory_order_acquire);
    }

  private:
    std::atomic<T> m_atomic;
    static_assert(std::atomic<T>::is_always_lock_free);
    static_assert(sizeof(T) == sizeof(std::atomic<T>));
  };

} // namespace yave