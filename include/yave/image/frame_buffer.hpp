//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/config/config.hpp>
#include <yave/image/image_view.hpp>

namespace yave {

  /// Provides interface to acquire frame buffer.
  /// All frame buffers are allocated and managed by system.
  class frame_buffer
  {
  public:
    frame_buffer()                    = default;
    frame_buffer(const frame_buffer&) = default;
    frame_buffer(frame_buffer&&)      = default;

    /// Acquire current frame buffer.
    [[nodiscard]] image_view get() const;

    /// Acquire new frame buffer and swap with current frame buffer in current
    /// frame path. Pointer to old frame buffer is valid until current node
    /// returns value.
    [[nodiscard]] image_view create() const;
  };
}