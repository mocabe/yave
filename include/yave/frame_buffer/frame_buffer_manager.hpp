//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/config/config.hpp>
#include <yave/image/image.hpp>
#include <yave/tools/id.hpp>

#include <vector>
#include <mutex>

namespace yave {

  class frame_buffer_manager
  {
  public:
    /// ctor
    frame_buffer_manager(
      const image_format& format,
      uint32_t width,
      uint32_t height);

    /// copy ctor is deleted
    frame_buffer_manager(const frame_buffer_manager&) = delete;

    /// dtor
    ~frame_buffer_manager() noexcept;

    /// Create new framebuffer
    uid create();
    /// Copy to new framebuffer
    uid create(const uint8_t* parent);
    /// Copy to new framebuffer
    uid create(uid parent);

    /// Increase reference count.
    void ref(uid id);
    /// Decrease reference count.
    void unref(uid id);

    /// Get pointer to internal buffer.
    uint8_t* get_data(uid id);
    /// Get pointer to internal buffer.
    const uint8_t* get_data(uid id) const;

    /// Get image format.
    image_format format() const;
    /// Get width of buffers.
    uint32_t width() const;
    /// Get height of buffers.
    uint32_t height() const;

    /// Get number of buffers.
    size_t size() const;
    /// Get list of buffers.
    std::vector<uid> buffers() const;

  private:
    image_format m_format;
    uint32_t m_width;
    uint32_t m_height;

  private:
    mutable std::mutex m_mtx;
    std::vector<uid> m_id;
    std::vector<void*> m_data;
  };
}