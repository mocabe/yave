//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/config/config.hpp>
#include <yave/support/id.hpp>
#include <yave/support/uuid.hpp>

namespace yave {

  /// General buffer pool object value.
  /// Backends can have its own implementation of buffer manager.
  struct buffer_pool
  {
    buffer_pool(
      void* handle,
      const uuid& backend_id,
      auto (*create)(void*, uint64_t) noexcept->uint64_t,
      auto (*create_from)(void* handle, uint64_t parent) noexcept->uint64_t,
      void (*ref)(void* handle, uint64_t id) noexcept,
      void (*unref)(void* handle, uint64_t id) noexcept,
      auto (*get_use_count)(void* handle, uint64_t id) noexcept->uint64_t,
      auto (*get_data)(void* handle, uint64_t) noexcept->std::byte*,
      auto (*get_size)(void* handle, uint64_t) noexcept->uint64_t)
      : m_handle {handle}
      , m_backend_id {backend_id}
      , m_create {create}
      , m_create_from {create_from}
      , m_ref {ref}
      , m_unref {unref}
      , m_get_use_count {get_use_count}
      , m_get_data {get_data}
      , m_get_size {get_size}
    {
    }

    /// Get buffer pool handle
    [[nodiscard]] auto handle() const noexcept
    {
      return m_handle;
    }

    /// Get backend ID
    [[nodiscard]] auto backend_id() const noexcept
    {
      return m_backend_id;
    }

    /// Create new buffer
    [[nodiscard]] auto create(uint64_t size) const noexcept
    {
      return m_create(m_handle, size);
    }

    /// Create new buffer from old buffer
    [[nodiscard]] auto create_from(const uint64_t& id) const noexcept
    {
      return m_create_from(m_handle, id);
    }

    /// Increment reference count of buffer
    [[nodiscard]] auto ref(const uint64_t& id) const noexcept
    {
      return m_ref(m_handle, id);
    }

    /// Decrement reference count of buffer
    [[nodiscard]] auto unref(const uint64_t& id) const noexcept
    {
      return m_unref(m_handle, id);
    }

    /// Get use count
    [[nodiscard]] auto get_use_count(const uint64_t& id) const noexcept
    {
      return m_get_use_count(m_handle, id);
    }

    /// Access to data of buffer
    [[nodiscard]] auto get_data(const uint64_t& id) const noexcept
    {
      return m_get_data(m_handle, id);
    }

    /// Get size of buffer
    [[nodiscard]] auto get_size(const uint64_t& id) const noexcept
    {
      return m_get_size(m_handle, id);
    }

  private: /* buffer pool info */
    /// Handle to buffer pool object
    void* m_handle;
    /// Backend ID
    uuid m_backend_id;

  private: /* buffer pool function pointers */
    /// Create new buffer of specified size.
    /// \note Should return valid (non-zero) ID even when `size == 0`.
    /// \note Should return 0 when allocation failed.
    /// \note Initial reference count should be 1.
    /// \note Should be thread safe.
    auto (*m_create)(void* handle, uint64_t size) noexcept -> uint64_t;

    /// Create new buffer cpyied from exising buffer.
    /// \note Should return 0 when `id` is invalid.
    /// \note Should return 0 when allocation failed.
    /// \note Should contain same data to parent buffer.
    /// \note Initial reference count should be 1.
    /// \note Should be thread safe.
    auto (*m_create_from)(void* handle, uint64_t id) noexcept -> uint64_t;

    /// Increment reference count.
    /// \note Should be thread safe.
    void (*m_ref)(void* handle, uint64_t id) noexcept;

    /// Decrement reference count.
    /// \note Deallocate buffer only when refcount is zero.
    /// \note Should always success, even on invalid IDs.
    /// \note Should be thread safe.
    void (*m_unref)(void* handle, uint64_t id) noexcept;

    /// Get current use count.
    /// \note Should return 0 on invalid IDs.
    /// \note Should be thread safe.
    auto (*m_get_use_count)(void* handle, uint64_t id) noexcept -> uint64_t;

    /// Get data pointer.
    /// \note Should return nullptr on invalid IDs.
    /// \note Should return nullptr on zero sized buffers.
    /// \note Should return readable and writable pointer.
    /// \note Should be thread safe.
    auto (*m_get_data)(void* handle, uint64_t) noexcept -> std::byte*;

    /// Get size of buffer.
    /// \note Should return 0 on invlid IDs.
    /// \note Should be thread safe.
    auto (*m_get_size)(void* handle, uint64_t) noexcept -> uint64_t;
  };
}