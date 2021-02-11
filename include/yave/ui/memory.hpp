//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <memory>

namespace yave::ui {

  template <class T>
  class unique;
  template <class T>
  class shared;
  template <class T>
  class weak;

  namespace detail {
    class make_unique_impl;
    class make_shared_impl;
  } // namespace detail

  /// Unique pointer for UI elements
  template <class T>
  class unique
  {
    std::shared_ptr<T> m_ptr;

    friend detail::make_unique_impl;

    unique(std::shared_ptr<T>&& up)
      : m_ptr {std::move(up)}
    {
    }

  public:
    using element_type = typename std::shared_ptr<T>::element_type;
    using weak_type    = typename std::shared_ptr<T>::weak_type;

  public:
    unique()                  = default;
    unique(const unique&)     = delete;
    unique(unique&&) noexcept = default;
    unique& operator=(const unique&) = delete;
    unique& operator=(unique&&) noexcept = default;

    template <class U>
    unique(unique<U> other) noexcept
      : m_ptr {std::move(other.m_ptr)}
    {
    }

    template <class U>
    unique& operator=(const unique<U> other) noexcept
    {
      m_ptr = std::move(other);
      return *this;
    }

    unique(std::nullptr_t) noexcept
      : m_ptr {nullptr}
    {
    }

    unique& operator=(std::nullptr_t) noexcept
    {
      m_ptr = nullptr;
      return *this;
    }

    auto get() const noexcept
    {
      return m_ptr.get();
    }

    auto operator*() const
    {
      return m_ptr.operator*();
    }

    auto operator->() const noexcept
    {
      return m_ptr.operator->();
    }

    explicit operator bool() const noexcept
    {
      return m_ptr.operator bool();
    }

    void swap(unique& other) noexcept
    {
      m_ptr.swap(other.m_ptr);
    }

    auto get_weak() const
    {
      return weak_type(*this);
    }

    auto operator<=>(const unique&) const = default;
  };

  namespace detail {
    class make_unique_impl
    {
    public:
      template <class T, class... Args>
      static auto make_unique(Args&&... args) -> unique<T>
      {
        return unique(std::make_shared<T>(std::forward<Args>(args)...));
      }
    };
  } // namespace detail

  /// make_unique
  template <class T, class... Args>
  auto make_unique(Args&&... args) -> unique<T>
  {
    return detail::make_unique_impl::make_unique<T>(
      std::forward<Args>(args)...);
  }

  /// Shared pointer for UI elements
  template <class T>
  class shared
  {
    std::shared_ptr<T> m_ptr;

    friend weak<T>;
    friend detail::make_shared_impl;

    shared(std::shared_ptr<T>&& sp)
      : m_ptr {std::move(sp)}
    {
    }

  public:
    using element_type = typename std::shared_ptr<T>::element_type;
    using weak_type    = typename std::shared_ptr<T>::weak_type;

  public:
    shared()                  = default;
    shared(const shared&)     = default;
    shared(shared&&) noexcept = default;
    shared& operator=(const shared&) = default;
    shared& operator=(shared&&) = default;

    template <class U>
    shared(shared<U> other) noexcept
      : m_ptr {std::move(other.m_ptr)}
    {
    }

    template <class U>
    shared& operator=(const shared<U> other) noexcept
    {
      m_ptr = std::move(other);
      return *this;
    }

    shared(std::nullptr_t)
      : m_ptr {nullptr}
    {
    }

    shared& operator=(std::nullptr_t)
    {
      m_ptr = nullptr;
      return *this;
    }

    auto get() const noexcept
    {
      return m_ptr.get();
    }

    auto operator*() const noexcept
    {
      return m_ptr.operator*();
    }

    auto operator->() const noexcept
    {
      return m_ptr.operator->();
    }

    explicit operator bool() const noexcept
    {
      return m_ptr.operator bool();
    }

    auto use_cound() const noexcept
    {
      return m_ptr.use_count();
    }

    void swap(shared& other) noexcept
    {
      m_ptr.swap(other.m_ptr);
    }

    auto get_weak() const
    {
      return weak_type(*this);
    }

    auto operator<=>(const shared&) const = default;
  };

  namespace detail {
    class make_shared_impl
    {
    public:
      template <class T, class... Args>
      static auto make_shared(Args&&... args) -> unique<T>
      {
        return shared(std::make_shared<T>(std::forward<Args>(args)...));
      }
    };
  } // namespace detail

  /// make_shared
  template <class T, class... Args>
  auto make_shared(Args&&... args) -> shared<T>
  {
    return detail::make_shared_impl::make_shared<T>(
      std::forward<Args>(args)...);
  }

  /// Weak pointer for UI elements
  template <class T>
  class weak
  {
    std::weak_ptr<T> m_ptr;

  public:
    using element_type = typename std::weak_ptr<T>::element_type;

  public:
    weak()                = default;
    weak(const weak&)     = default;
    weak(weak&&) noexcept = default;
    weak& operator=(const weak&) = default;
    weak& operator=(weak&&) = default;

    template <class U>
    weak(weak<U> other) noexcept
      : m_ptr {std::move(other.m_ptr)}
    {
    }

    template <class U>
    weak& operator=(const weak<U> other) noexcept
    {
      m_ptr = std::move(other);
    }

    template <class U>
    weak(const shared<U>& s) noexcept
      : m_ptr {s.get_weak()}
    {
    }

    template <class U>
    weak& operator=(const shared<U>& other) noexcept
    {
      operator=(other.get_weak());
      return *this;
    }

    template <class U>
    weak(const unique<U>& u) noexcept
      : m_ptr {u.get_weak()}
    {
    }

    template <class U>
    weak& operator=(const unique<U>& other) noexcept
    {
      operator=(other.get_weak());
      return *this;
    }

    weak(std::nullptr_t)
      : m_ptr {nullptr}
    {
    }

    weak& operator=(std::nullptr_t)
    {
      m_ptr = nullptr;
      return *this;
    }

    auto get() const noexcept
    {
      return m_ptr.get();
    }

    auto operator*() const noexcept
    {
      return m_ptr.operator*();
    }

    auto operator->() const noexcept
    {
      return m_ptr.operator->();
    }

    explicit operator bool() const noexcept
    {
      return m_ptr.operator bool();
    }

    auto use_count() const noexcept
    {
      return m_ptr.use_count();
    }

    bool expired() const noexcept
    {
      return m_ptr.expired();
    }

    auto lock() const noexcept -> shared<T>
    {
      return shared<T>(m_ptr.lock());
    }

    void swap(weak& other) noexcept
    {
      m_ptr.swap(other.m_ptr);
    }

    auto operator<=>(const weak&) const = default;
  };

} // namespace yave::ui