//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <memory>
#include <concepts>

namespace yave::ui {

  template <class T>
  class unique;
  template <class T>
  class shared;
  template <class T>
  class weak;
  template <class...>
  class slot;

  struct trackable;

  /// Lifetime tracker
  class tracker
  {
  public:
    tracker(const tracker&)     = default;
    tracker(tracker&&) noexcept = default;
    tracker& operator=(const tracker&) = default;
    tracker& operator=(tracker&&) noexcept = default;

    template <class T>
    tracker(const unique<T>& ptr)
      : m_weak {ptr.get_weak()}
    {
    }

    template <class T>
    tracker(const shared<T>& ptr)
      : m_weak {ptr.get_weak()}
    {
    }

    /// Check if tracked object is destroyed
    bool expired() const
    {
      return m_weak.expired();
    }

    /// Try to temporarily extend lifetime of tracked object and execute
    /// function while it is alive.
    /// \retval true Success.
    /// \retval false Tracked object has been destroyed.
    template <class F>
    auto lock_with(F&& f) -> tracker&
    {
      if (auto sp = m_weak.lock())
        std::forward<F>(f)();
      return *this;
      }

    /// Monadic operator
    template <class F>
    auto and_then(F&& f) -> tracker&
    {
      if (!expired())
        std::forward<F>(f)();
      return *this;
    }

    /// Monadic operator
    template <class F>
    auto or_else(F&& f) -> tracker&
    {
      if (expired())
        std::forward<F>(f)();
      return *this;
    }

  private:
    template <class...>
    friend class slot;

    // for slot
    auto get_weak() const
    {
      return m_weak;
    }

    std::weak_ptr<const trackable> m_weak;
  };

  /// Call try_lock on multiple trackers.
  /// \retval true Success.
  /// \retval false Tracked object has been destroyed.
  template <class F, class Head, class... Tail>
    requires std::same_as<std::decay_t<Head>, tracker>
  void lock_with(F&& f, Head&& h, Tail&&... t)
  {
    if constexpr (sizeof...(t) == 0) {
      h.lock_with(std::forward<F>(f));
    } else {
      h.lock_with(
        [&] { lock_with(std::forward<F>(f), std::forward<Tail>(t)...); });
    }
  }

  /// Trackable base interface
  struct trackable
  {
    virtual ~trackable() noexcept = default;
    /// Get tracker object
    virtual auto get_tracker() const -> tracker = 0;
  };

  /// Helper class to enable creating tracker from tihs pointer
  template <class Derived>
  class enable_tracker_from_this : public std::enable_shared_from_this<Derived>
  {
  protected:
    auto tracker_from_this() const -> tracker
    {
      return tracker(shared(this->shared_from_this()));
    }

  private:
    using std::enable_shared_from_this<Derived>::shared_from_this;
    using std::enable_shared_from_this<Derived>::weak_from_this;
  };

  /// Helper class which automates implementation of trackable interface by CRTP
  template <class Derived, std::derived_from<trackable> Base>
  struct generic_trackable : Base, enable_tracker_from_this<Derived>
  {
    using Base::Base;

    auto get_tracker() const -> tracker override
    {
      return this->tracker_from_this();
    }
  };

  template <std::derived_from<trackable> T>
  class weak_ref;

  /// Weak reference
  template <std::derived_from<trackable> T>
  class weak_ref
  {
    T* m_ref;
    tracker m_tracker;

    template <std::derived_from<trackable> U>
    friend class weak_ref;

  public:
    weak_ref(T& ref)
      : m_ref {&ref}
      , m_tracker {ref.get_tracker()}
    {
    }

    weak_ref(const weak_ref&)     = default;
    weak_ref(weak_ref&&) noexcept = default;
    weak_ref& operator=(weak_ref&&) noexcept = default;
    weak_ref& operator=(const weak_ref&) = default;

    template <class U>
    weak_ref(const weak_ref<U>& other)
      : m_ref {other.m_ref}
      , m_tracker {other.m_tracker}
    {
    }

    template <class U>
    weak_ref& operator=(const weak_ref<U>& other)
    {
      m_ref     = other.m_ref;
      m_tracker = other.m_tracker;
      return *this;
    }

    bool expired() const
    {
      return m_tracker.expired();
    }

    auto get() -> T*
    {
      if (!expired())
        return m_ref;
      return nullptr;
    }

    auto get() const -> const T*
    {
      if (!expired())
        return m_ref;
      return nullptr;
    }

    explicit operator bool() const
    {
      return !expired();
    }

    template <class F>
    auto apply(F&& f) -> weak_ref&
    {
      if (auto ref = get()) {
        std::forward<F>(f)(*ref);
      }
      return *this;
    }

    template <class F>
    auto and_then(F&& f) -> weak_ref&
    {
      if (!expired()) {
        std::forward<F>(f)(*m_ref);
      }
      return *this;
    }

    template <class F>
    auto or_else(F&& f) -> weak_ref&
    {
      if (expired()) {
        std::forward<F>(f)();
      }
      return *this;
    }

    template <class F>
    auto lock_with(F&& f) -> weak_ref&
    {
      if (auto ref = get())
        m_tracker.lock_with([&] { std::forward<F>(f)(*ref); });
      return *this;
    }
  };
}