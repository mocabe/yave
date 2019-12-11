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
    : m_mngr {}
    , m_format {format}
    , m_width {width}
    , m_height {height}
  {
    // clang-format off

    // create pool object
    m_pool = make_object<FrameBufferPool>(
      (void*)this,
      backend_id,
      [](void* handle)              noexcept -> uint64_t     { return ((frame_buffer_manager*)handle)->create().data; },
      [](void* handle, uint64_t id) noexcept -> uint64_t     { return ((frame_buffer_manager*)handle)->create_from({id}).data; },
      [](void* handle, uint64_t id) noexcept -> void         { return ((frame_buffer_manager*)handle)->ref({id}); },
      [](void* handle, uint64_t id) noexcept -> void         { return ((frame_buffer_manager*)handle)->unref({id}); },
      [](void* handle, uint64_t id) noexcept -> uint64_t     { return ((frame_buffer_manager*)handle)->use_count({id}); },
      [](void* handle, uint64_t id) noexcept -> uint8_t*     { return ((frame_buffer_manager*)handle)->data({id}); },
      [](void* handle)              noexcept -> uint64_t     { return ((frame_buffer_manager*)handle)->size(); },
      [](void* handle)              noexcept -> image_format { return ((frame_buffer_manager*)handle)->format(); },
      [](void* handle)              noexcept -> uint32_t     { return ((frame_buffer_manager*)handle)->width(); },
      [](void* handle)              noexcept -> uint32_t     { return ((frame_buffer_manager*)handle)->height(); });

    // clang-format on
  }

  frame_buffer_manager::~frame_buffer_manager() noexcept
  {
  }

  frame_buffer_manager::frame_buffer_manager(
    frame_buffer_manager&& other) noexcept
  {
    m_mngr   = std::move(other.m_mngr);
    m_format = std::move(other.m_format);
    m_width  = std::move(other.m_width);
    m_height = std::move(other.m_height);
    m_pool   = std::move(other.m_pool);
  }

  frame_buffer_manager& frame_buffer_manager::operator=(
    frame_buffer_manager&& other) noexcept
  {
    m_mngr   = std::move(other.m_mngr);
    m_format = std::move(other.m_format);
    m_width  = std::move(other.m_width);
    m_height = std::move(other.m_height);
    m_pool   = std::move(other.m_pool);
    return *this;
  }

  uid frame_buffer_manager::create() noexcept
  {
    // Use channel size as alignment value, we can use much larger alignment
    // for SIMD supoprt.
    return m_mngr.create(size(), byte_per_channel(m_format));
  }

  uid frame_buffer_manager::create_from(uid id) noexcept
  {
    return m_mngr.create_from(id);
  }

  void frame_buffer_manager::ref(uid id) noexcept
  {
    m_mngr.ref(id);
  }

  void frame_buffer_manager::unref(uid id) noexcept
  {
    m_mngr.unref(id);
  }

  auto frame_buffer_manager::use_count(uid id) const noexcept -> uint64_t
  {
    return m_mngr.use_count(id);
  }

  auto frame_buffer_manager::data(uid id) const noexcept -> uint8_t*
  {
    return m_mngr.data(id);
  }

  auto frame_buffer_manager::size() const noexcept -> uint64_t
  {
    return byte_per_pixel(m_format) * m_width * m_height;
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

  auto frame_buffer_manager::get_pool_object() const noexcept
    -> object_ptr<FrameBufferPool>
  {
    return m_pool;
  }
}