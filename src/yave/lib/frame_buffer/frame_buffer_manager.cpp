//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/lib/frame_buffer/frame_buffer_manager.hpp>
#include <yave/obj/frame_buffer/frame_buffer_pool.hpp>

#include <yave/support/id.hpp>
#include <yave/rts/atomic.hpp>

namespace yave {

  frame_buffer_manager::frame_buffer_manager(
    uint32_t width,
    uint32_t height,
    const image_format& format,
    const uuid& backend_id)
    : m_format {format}
    , m_width {width}
    , m_height {height}
  {
    // clang-format off

    // create pool object
    m_pool = make_object<FrameBufferPool>(
      (void*)this,
      backend_id,
      [](void* handle, uint64_t sz) noexcept -> uid { return ((frame_buffer_manager*)handle)->create(sz); },
      [](void* handle, uid id) noexcept -> uid      { return ((frame_buffer_manager*)handle)->create_from(id); },
      [](void* handle, uid id) noexcept -> void     { return ((frame_buffer_manager*)handle)->ref(id); },
      [](void* handle, uid id) noexcept -> void     { return ((frame_buffer_manager*)handle)->unref(id); },
      [](void* handle, uid id) noexcept -> uint64_t { return ((frame_buffer_manager*)handle)->use_count(id); },
      [](void* handle, uid id) noexcept -> uint8_t* { return ((frame_buffer_manager*)handle)->data(id); },
      [](void* handle, uid id) noexcept -> uint64_t { return ((frame_buffer_manager*)handle)->size(id); },
      [](void* handle) noexcept -> uid              { return ((frame_buffer_manager*)handle)->create(); },
      [](void* handle) noexcept -> image_format     { return ((frame_buffer_manager*)handle)->format(); },
      [](void* handle) noexcept -> uint32_t         { return ((frame_buffer_manager*)handle)->width(); },
      [](void* handle) noexcept -> uint32_t         { return ((frame_buffer_manager*)handle)->height(); },
      [](void* handle) noexcept -> uint64_t         { return ((frame_buffer_manager*)handle)->byte_size(); });

    // clang-format on
  }

  frame_buffer_manager::~frame_buffer_manager() noexcept
  {
  }

  uid frame_buffer_manager::create() noexcept
  {
    return buffer_manager::create(byte_size());
  }

  uid frame_buffer_manager::create_from(uid id) noexcept
  {
    return buffer_manager::create_from(id);
  }

  auto frame_buffer_manager::format() const noexcept -> image_format
  {
    return m_format;
  }

  auto frame_buffer_manager::width() const noexcept -> uint32_t
  {
    return m_width;
  }

  auto frame_buffer_manager::height() const noexcept -> uint32_t
  {
    return m_height;
  }

  auto frame_buffer_manager::byte_size() const noexcept -> uint64_t
  {
    return byte_per_pixel(m_format) * m_width * m_height;
  }

  auto frame_buffer_manager::get_pool_object() const noexcept
    -> object_ptr<FrameBufferPool>
  {
    return m_pool;
  }
}