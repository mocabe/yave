//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/core/data_types/frame_buffer.hpp>
#include <yave/core/objects/frame_buffer.hpp>

namespace yave {

  frame_buffer::frame_buffer(frame_buffer_manager& mngr)
    : m_manager {mngr}
  {
    m_id = m_manager.create();
  }

  frame_buffer::frame_buffer(frame_buffer_manager& mngr, uid id)
    : m_manager {mngr}
    , m_id {id}
  {
  }

  frame_buffer::frame_buffer(const frame_buffer& other)
    : m_manager {other.m_manager}
    , m_id {other.m_id}
  {
    m_manager.ref(m_id);
  }

  frame_buffer::frame_buffer(frame_buffer&& other)
    : m_manager {other.m_manager}
    , m_id {other.m_id}
  {
  }

  frame_buffer::~frame_buffer() noexcept
  {
    m_manager.unref(m_id);
  }

  [[nodiscard]] object_ptr<FrameBuffer> frame_buffer::get() const
  {
    // copy to new buffer
    return make_object<FrameBuffer>(m_manager, m_manager.create(m_id));
  }

  [[nodiscard]] image_view frame_buffer::get_image_view() const
  {
    return image_view(
      m_manager.get_data(m_id),
      m_manager.width(),
      m_manager.height(),
      m_manager.format());
  }

  [[nodiscard]] uint32_t frame_buffer::width() const
  {
    return m_manager.width();
  }

  [[nodiscard]] uint32_t frame_buffer::height() const
  {
    return m_manager.height();
  }

  [[nodiscard]] image_format frame_buffer::format() const
  {
    return m_manager.format();
  }

  [[nodiscard]] const uint8_t* frame_buffer::data() const
  {
    return m_manager.get_data(m_id);
  }

  [[nodiscard]] uint8_t* frame_buffer::data()
  {
    return m_manager.get_data(m_id);
  }
}