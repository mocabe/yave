//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/config/config.hpp>
#include <yave/obj/buffer/buffer_pool.hpp>

namespace yave {

  /// Reference implementation of buffer protocol.
  /// Does NO memory pool or other kind of optimizations.
  /// Just allocate and register IDs and pointers.
  class buffer_manager
  {
  public:
    buffer_manager()                      = delete;
    buffer_manager(const buffer_manager&) = delete;

    /// Ctor
    buffer_manager(const uuid& backend_id = uuid());

    /// Dtor
    ~buffer_manager() noexcept;

    /// create
    [[nodiscard]] uid create(uint64_t size) noexcept;
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

  private:
    [[nodiscard]] auto _lock() const -> std::unique_lock<std::mutex>;
    [[nodiscard]] auto _find_data(uid id) const -> void*;
    void _insert(uid id, void* data);

  private:
    /// Thread safety is required.
    mutable std::mutex m_mtx;

  private:
    std::vector<uid> m_id;
    std::vector<void*> m_data;

  private:
    uuid m_backend_id;

  private:
    object_ptr<BufferPool> m_pool;
  };
}