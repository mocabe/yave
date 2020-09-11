//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/config/config.hpp>

#include <memory>
#include <typeinfo>

namespace yave::editor {

  /// unique context data container
  class unique_context_data
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
    unique_context_data() noexcept = delete;
    /// dtor
    ~unique_context_data() noexcept = default;
    /// no copy
    unique_context_data(const unique_context_data&) = delete;
    /// movable
    unique_context_data(unique_context_data&&) noexcept = default;

  public:
    /// construct from data
    template <
      class T,
      class =
        std::enable_if_t<!std::is_same_v<std::decay_t<T>, unique_context_data>>>
    unique_context_data(T&& val)
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
}