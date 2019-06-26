//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/core/config.hpp>
#include <yave/core/rts/object_ptr.hpp>
#include <yave/data/lib/frame_buffer_manager.hpp>
#include <yave/data/lib/image_view.hpp>

namespace yave {

  template <class T>
  struct Box; // rts/box_fwd.hpp

  class frame_buffer;

  using FrameBuffer = Box<frame_buffer>; // obj/frame_buffer.hpp

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
    [[nodiscard]] object_ptr<FrameBuffer> get() const;
    /// Get image view.
    [[nodiscard]] mutable_image_view get_image_view();
    /// Get image view.
    [[nodiscard]] const_image_view get_image_view() const;

  private:
    frame_buffer() = delete;

  private:
    frame_buffer_manager& m_manager;
    uid m_id;
  };
}