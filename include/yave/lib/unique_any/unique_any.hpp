//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/config/config.hpp>

#include <memory>
#include <typeinfo>
#include <stdexcept>

namespace yave {

  /// unique version of std::any
  class unique_any
  {
    struct holder_base
    {
      // dtor
      virtual ~holder_base() noexcept = default;
      // get type
      virtual auto type() noexcept -> const std::type_info& = 0;
      // get data
      virtual auto data() noexcept -> void* = 0;
    };

    template <class T>
    struct holder final : holder_base
    {
      T value;

      template <class U>
      holder(U&& v)
        : value {std::forward<U>(v)}
      {
      }

      auto type() noexcept -> const std::type_info& override
      {
        return typeid(T);
      }

      auto data() noexcept -> void* override
      {
        return static_cast<T*>(&value);
      }
    };

    std::unique_ptr<holder_base> m_data = nullptr;

  public:
    /// ctor
    unique_any() noexcept = default;
    /// dtor
    ~unique_any() noexcept = default;
    /// no copy
    unique_any(const unique_any&) = delete;
    /// movable
    unique_any(unique_any&&) noexcept = default;
    /// no copy
    unique_any& operator=(const unique_any&) = delete;
    /// movable
    unique_any& operator=(unique_any&&) noexcept = default;

  public:
    /// construct from data
    template <
      class T,
      class = std::enable_if_t<!std::is_same_v<std::decay_t<T>, unique_any>>>
    unique_any(T&& val)
      : m_data {std::make_unique<holder<std::decay_t<T>>>(std::forward<T>(val))}
    {
    }

  public:
    /// event this class cannot be default constructed, it still can be empty
    /// after move.
    [[nodiscard]] bool empty() const noexcept
    {
      return !static_cast<bool>(m_data);
    }

    /// get type of contained object
    [[nodiscard]] auto type() const noexcept -> const std::type_info&
    {
      return !empty() ? m_data->type() : typeid(void);
    }

    /// data
    [[nodiscard]] auto data() const noexcept -> const void*
    {
      return !empty() ? m_data->data() : nullptr;
    }

    /// data
    [[nodiscard]] auto data() noexcept -> void*
    {
      return !empty() ? m_data->data() : nullptr;
    }
  };

  template <class T>
  [[nodiscard]] auto unique_any_cast(unique_any* v) -> T*
  {
    if (v && v->type() == typeid(T)) {
      return static_cast<T*>(v->data());
    }
    return nullptr;
  }

  template <class T>
  [[nodiscard]] auto unique_any_cast(const unique_any* v) -> const T*
  {
    if (v && v->type() == typeid(T)) {
      return static_cast<const T*>(v->data());
    }
    return nullptr;
  }

  template <class T>
  [[nodiscard]] auto& unique_any_cast(unique_any& v)
  {
    if (auto ptr = unique_any_cast<T>(&v)) {
      return *ptr;
    }
    throw std::bad_cast();
  }

  template <class T>
  [[nodiscard]] auto& unique_any_cast(const unique_any& v)
  {
    if (auto ptr = unique_any_cast<T>(&v)) {
      return *ptr;
    }
    throw std::bad_cast();
  }

} // namespace yave