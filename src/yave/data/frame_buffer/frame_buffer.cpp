//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/data/obj/frame_buffer.hpp>
#include <yave/data/obj/frame_buffer_pool.hpp>

namespace yave {

  frame_buffer::frame_buffer(const object_ptr<FrameBufferPool>& pl)
    : m_pool {pl}
  {
    m_id = m_pool->create();
  }

  frame_buffer::frame_buffer(const object_ptr<FrameBufferPool>& pl, uid id)
    : m_pool {pl}
    , m_id {id}
  {
  }

  frame_buffer::frame_buffer(const frame_buffer& other)
    : m_pool {other.m_pool}
    , m_id {other.m_id}
  {
    m_pool->ref(m_id);
  }

  frame_buffer::frame_buffer(frame_buffer&& other)
    : m_pool {other.m_pool}
    , m_id {other.m_id}
  {
  }

  frame_buffer::~frame_buffer() noexcept
  {
    m_pool->unref(m_id);
  }

  object_ptr<FrameBuffer> frame_buffer::copy() const
  {
    // copy to new buffer
    return make_object<FrameBuffer>(m_pool, m_pool->create(m_id));
  }

  mutable_image_view frame_buffer::get_image_view()
  {
    return mutable_image_view(
      m_pool->get_data(m_id),
      m_pool->width(),
      m_pool->height(),
      m_pool->format());
  }

  const_image_view frame_buffer::get_image_view() const
  {
    return const_image_view(
      m_pool->get_data(m_id),
      m_pool->width(),
      m_pool->height(),
      m_pool->format());
  }

} // namespace yave