//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/obj/frame_buffer/frame_buffer.hpp>
#include <yave/obj/frame_buffer/frame_buffer_pool.hpp>

namespace yave {

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