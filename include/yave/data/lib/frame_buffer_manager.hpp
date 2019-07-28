//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/core/config.hpp>
#include <yave/data/lib/image_format.hpp>
#include <yave/support/id.hpp>
#include <yave/support/uuid.hpp>

#include <vector>
#include <mutex>

namespace yave {

  /// Proxy data structue for backend frame buffer manager.
  class frame_buffer_manager
  {
  public:
    frame_buffer_manager(
      void* handle,
      uuid backend_id,
      auto (*create0)(void*)->uid,
      auto (*create1)(void*, uid)->uid,
      void (*ref)(void*, uid),
      void (*unref)(void*, uid),
      auto (*get_data)(void*, uid)->uint8_t*,
      auto (*format)(const void*)->image_format,
      auto (*width)(const void*)->uint32_t,
      auto (*height)(const void*)->uint32_t)
      : m_handle {handle}
      , m_backend_id {backend_id}
      , m_create0 {create0}
      , m_create1 {create1}
      , m_ref {ref}
      , m_unref {unref}
      , m_get_data {get_data}
      , m_format {format}
      , m_width {width}
      , m_height {height}
    {
    }

    frame_buffer_manager(const frame_buffer_manager&) = default;
    frame_buffer_manager& operator=(const frame_buffer_manager&) = default;

    auto handle() const -> void*
    {
      return m_handle;
    }

    auto backend_id() const -> const uuid&
    {
      return m_backend_id;
    }

    auto create() -> uid
    {
      return m_create0(m_handle);
    }

    auto create(const uid& parent) -> uid
    {
      return m_create1(m_handle, parent);
    }

    auto ref(const uid& id)
    {
      return m_ref(m_handle, id);
    }

    auto unref(const uid& id)
    {
      return m_unref(m_handle, id);
    }

    auto get_data(const uid& id) -> uint8_t*
    {
      return m_get_data(m_handle, id);
    }

    auto format() const -> image_format
    {
      return m_format(m_handle);
    }

    auto width() const -> uint32_t
    {
      return m_width(m_handle);
    }

    auto height() const -> uint32_t
    {
      return m_height(m_handle);
    }

  private:
    void* m_handle;
    uuid m_backend_id;

    auto (*m_create0)(void* handle) -> uid;
    auto (*m_create1)(void* handle, uid parent) -> uid;
    void (*m_ref)(void* handle, uid id);
    void (*m_unref)(void* handle, uid id);
    auto (*m_get_data)(void* handle, uid) -> uint8_t*;
    auto (*m_format)(const void* handle) -> image_format;
    auto (*m_width)(const void* handle) -> uint32_t;
    auto (*m_height)(const void* handle) -> uint32_t;
  };
}