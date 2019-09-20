//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/config/config.hpp>
#include <yave/lib/image/image_view.hpp>
#include <yave/lib/buffer/buffer.hpp>
#include <yave/obj/frame_buffer/frame_buffer_pool.hpp>
#include <yave/support/id.hpp>

namespace yave {

  /// Frame buffer object value.
  struct frame_buffer : buffer_base<frame_buffer, FrameBufferPool>
  {
    frame_buffer(const object_ptr<FrameBufferPool>& pool)
      : buffer_base(pool)
    {
      m_id = pool->create();

      if (m_id == uid())
        throw std::bad_alloc();
    }

    frame_buffer(const object_ptr<FrameBufferPool>& pool, uid id)
      : buffer_base(pool)
    {
      m_id = pool->create_from(id);

      if (m_id == uid())
        throw std::bad_alloc();
    }

    /// Get image view.
    [[nodiscard]] mutable_image_view get_image_view();
    /// Get image view.
    [[nodiscard]] const_image_view get_image_view() const;
  };
}