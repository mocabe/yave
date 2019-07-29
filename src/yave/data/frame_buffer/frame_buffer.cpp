//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/data/obj/frame_buffer.hpp>
#include <yave/data/obj/frame_buffer_manager.hpp>

namespace yave {

  frame_buffer::frame_buffer(const object_ptr<FrameBufferPool>& mngr)
    : m_manager {mngr}
  {
    m_id = m_manager->create();
  }

  frame_buffer::frame_buffer(const object_ptr<FrameBufferPool>& mngr, uid id)
    : m_manager {mngr}
    , m_id {id}
  {
  }

  frame_buffer::frame_buffer(const frame_buffer& other)
    : m_manager {other.m_manager}
    , m_id {other.m_id}
  {
    m_manager->ref(m_id);
  }

  frame_buffer::frame_buffer(frame_buffer&& other)
    : m_manager {other.m_manager}
    , m_id {other.m_id}
  {
  }

  frame_buffer::~frame_buffer() noexcept
  {
    m_manager->unref(m_id);
  }

  object_ptr<FrameBuffer> frame_buffer::copy() const
  {
    // copy to new buffer
    return make_object<FrameBuffer>(*m_manager, m_manager->create(m_id));
  }

  mutable_image_view frame_buffer::get_image_view()
  {
    return mutable_image_view(
      m_manager->get_data(m_id),
      m_manager->width(),
      m_manager->height(),
      m_manager->format());
  }

  const_image_view frame_buffer::get_image_view() const
  {
    return const_image_view(
      m_manager->get_data(m_id),
      m_manager->width(),
      m_manager->height(),
      m_manager->format());
  }

} // namespace yave