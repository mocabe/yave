//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/config/config.hpp>
#include <yave/obj/buffer/buffer_pool.hpp>
#include <yave/obj/buffer/buffer.hpp>

namespace yave {

  /// Simple implementation of buffer pool interface.
  /// Does NO memory pooling or any kind of optimization.
  class buffer_manager
  {
  public:
    /// Ctor
    buffer_manager();
    /// Ctor
    buffer_manager(const uuid& backend_id);

    /// deleted
    buffer_manager(const buffer_manager&) = delete;

    /// Dtor
    ~buffer_manager() noexcept;

  public: /* BufferPool interface */
    /// create
    [[nodiscard]] uid create(
      uint64_t size,
      uint64_t alignment = alignof(uint8_t)) noexcept;
    /// create from
    [[nodiscard]] uid create_from(uid id) noexcept;

    /// ref
    void ref(uid id) noexcept;
    /// unref
    void unref(uid id) noexcept;

    /// Get use count
    [[nodiscard]] auto use_count(uid id) const noexcept -> uint64_t;
    /// Get data pointer
    [[nodiscard]] auto data(uid id) const noexcept -> uint8_t*;
    /// Get buffer size
    [[nodiscard]] auto size(uid id) const noexcept -> uint64_t;

    /// Get pool object
    [[nodiscard]] auto get_pool_object() const noexcept
      -> object_ptr<BufferPool>;

  protected:
    [[nodiscard]] auto _lock() const -> std::unique_lock<std::mutex>;
    [[nodiscard]] auto _find_data(uid id) const -> void*;
    void _insert(uid id, void* data);

  protected:
    /// Thread safety is required.
    mutable std::mutex m_mtx;

  protected:
    std::vector<uid> m_id;
    std::vector<void*> m_data;

  protected:
    uuid m_backend_id;

  protected:
    object_ptr<BufferPool> m_pool;
  };
}