//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/config/config.hpp>
#include <yave/image/image_view.hpp>
#include <yave/frame_buffer/frame_buffer_manager.hpp>

#include <yave/rts/object_ptr.hpp>

namespace yave {

  template <class T>
  struct Box; // rts/box_fwd.hpp

  class frame_buffer;

  namespace FrameBuffer {
    using FrameBuffer = Box<frame_buffer>; // obj/frame_buffer.hpp
  }

  /// Provides interface to acquire frame buffer.
  /// All frame buffers are allocated and managed by system.
  class frame_buffer
  {
  public:
    /// Ctor
    frame_buffer(frame_buffer_manager& mngr);
    /// Ctor
    frame_buffer(frame_buffer_manager& mngr, uid id);
    /// Copy ctor
    frame_buffer(const frame_buffer& other);
    /// Move ctor
    frame_buffer(frame_buffer&& other);
    /// Dtor
    ~frame_buffer() noexcept;

    /// Get frame buffer object.
    [[nodiscard]] object_ptr<FrameBuffer::FrameBuffer> get() const;
    /// Get image view.
    [[nodiscard]] image_view get_image_view() const;

    /// Get buffer width
    [[nodiscard]] uint32_t width() const;
    /// Get buffer height
    [[nodiscard]] uint32_t height() const;
    /// Get buffer format
    [[nodiscard]] image_format format() const;

    /// Get data pointer.
    [[nodiscard]] uint8_t* data();
    /// Get data pointer.
    [[nodiscard]] const uint8_t* data() const;

  private:
    frame_buffer_manager& m_manager;
    uid m_id;
  };
}