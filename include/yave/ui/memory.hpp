//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/ui/tracker.hpp>

#include <stdexcept>
#include <cassert>

namespace yave::ui {

  template <class T>
  class unique;
  template <class T>
  class shared;
  template <class T>
  class weak;

  /// Unique pointer for UI elements
  template <class T>
  class unique
  {
    std::shared_ptr<T> m_ptr;

    template <class U>
    friend class unique;

    friend class shared<T>;
    friend class tracker;

    template <class U, class... Args>
    friend auto make_unique(Args&&... args) -> unique<U>;

    explicit unique(std::shared_ptr<T>&& up) noexcept
      : m_ptr {std::move(up)}
    {
    }

    auto get_weak() const
    {
      return std::weak_ptr(m_ptr);
    }

  public:
    /// element type
    using element_type = typename std::shared_ptr<T>::element_type;

    unique()                  = default;
    unique(const unique&)     = delete;
    unique(unique&&) noexcept = default;
    unique& operator=(const unique&) = delete;
    unique& operator=(unique&&) noexcept = default;

    explicit unique(std::unique_ptr<T>&& up) noexcept
      : m_ptr {std::move(up)}
    {
    }

    template <class U>
    unique(unique<U>&& other) noexcept
      : m_ptr {std::move(other.m_ptr)}
    {
    }

    template <class U>
    unique& operator=(unique<U>&& other) noexcept
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

    auto& operator*() const
    {
      assert(m_ptr);
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

    /// call static_cast on pointer value
    template <class U>
    auto cast_static() && noexcept -> unique<U>
    {
      return unique<U>(std::static_pointer_cast<U>(std::move(m_ptr)));
    }

    /// call dynamic_cast on pointer
    /// \note keep pointer value on failure
    template <class U>
    auto cast_dynamic() && noexcept -> unique<U>
    {
      return unique<U>(std::dynamic_pointer_cast<U>(std::move(m_ptr)));
    }

    auto operator<=>(const unique&) const -> std::strong_ordering = default;
  };

  template <class T, class... Args>
  auto make_unique(Args&&... args) -> unique<T>
  {
    return unique(std::make_shared<T>(std::forward<Args>(args)...));
  }

  /// call static_cast on pointer value
  template <class T, class U>
  auto static_pointer_cast(unique<U>&& p)
  {
    return std::move(p).template cast_static<T>();
  }

  /// call dynamic_cast on pointer
  /// \note keep pointer value on failure
  template <class T, class U>
  auto dynamic_pointer_cast(unique<U>&& p)
  {
    return std::move(p).template cast_dynamic<T>();
  }

  /// Shared pointer for UI elements
  template <class T>
  class shared
  {
    std::shared_ptr<T> m_ptr;

    template <class U>
    friend class shared;

    friend class weak<T>;
    friend class tracker;

    auto get_weak() const
    {
      return std::weak_ptr(m_ptr);
    }

  public:
    using element_type = typename std::shared_ptr<T>::element_type;
    using weak_type    = weak<T>;

    shared()                  = default;
    shared(const shared&)     = default;
    shared(shared&&) noexcept = default;
    shared& operator=(const shared&) = default;
    shared& operator=(shared&&) = default;

    explicit shared(std::shared_ptr<T>&& sp) noexcept
      : m_ptr {std::move(sp)}
    {
    }

    template <class U>
    shared(shared<U> other) noexcept
      : m_ptr {std::move(other.m_ptr)}
    {
    }

    template <class U>
    shared& operator=(shared<U> other) noexcept
    {
      m_ptr = std::move(other);
      return *this;
    }

    template <class U>
    shared(unique<U>&& other) noexcept
      : m_ptr {std::move(other.m_ptr)}
    {
    }

    template <class U>
    shared& operator=(unique<U>&& other) noexcept
    {
      m_ptr = std::move(other.m_ptr);
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

    auto& operator*() const noexcept
    {
      assert(m_ptr);
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

    template <class U>
    auto cast_static() const& noexcept -> shared<U>
    {
      return shared<U>(std::static_pointer_cast<U>(m_ptr));
    }

    template <class U>
    auto cast_static() && noexcept -> shared<U>
    {
      return shared<U>(std::static_pointer_cast<U>(std::move(m_ptr)));
    }

    template <class U>
    auto cast_dynamic() const& noexcept -> shared<U>
    {
      return shared<U>(std::dynamic_pointer_cast<U>(m_ptr));
    }

    template <class U>
    auto cast_dynamic() && noexcept -> shared<U>
    {
      return shared<U>(std::dynamic_pointer_cast<U>(std::move(m_ptr)));
    }

    auto operator<=>(const shared&) const -> std::strong_ordering = default;
  };

  template <class T>
  shared(unique<T>) -> shared<T>;

  template <class T, class... Args>
  auto make_shared(Args&&... args) -> shared<T>
  {
    return shared(std::make_shared<T>(std::forward<Args>(args)...));
  }

  template <class T, class U>
  auto static_pointer_cast(const shared<U>& p)
  {
    return p.template cast_static<T>();
  }

  template <class T, class U>
  auto dynamic_pointer_cast(const shared<U>& p)
  {
    return p.template cast_dynamic<T>();
  }

} // namespace yave::ui