//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/core/config/config.hpp>
#include <yave/core/data_types/time.hpp>
#include <yave/core/data_types/image.hpp>

namespace yave {

  class scene_config
  {
  public:
    /// Setup scene config.
    /// \param width non-zero frame buffer width
    /// \param height non-zero frame buffer height
    /// \param frame_rate non-zero frame rate (fps)
    /// \param sample_rate non-zero sample rate (Hz)
    /// \param format frame buffer format
    ///
    /// Currently, following image formats are supported for frame
    /// buffer format:
    ///   image_format::RGBA32F
    scene_config(
      uint32_t width,
      uint32_t height,
      uint32_t frame_rate,
      uint32_t sample_rate,
      image_format format);

    ~scene_config() noexcept                = default;
    scene_config(const scene_config& other) = default;
    scene_config(scene_config&& other)      = default;
    scene_config& operator=(const scene_config& other) = default;
    scene_config& operator=(scene_config&& other) noexcept = default;

    uint32_t width() const noexcept;
    uint32_t height() const noexcept;
    uint32_t frame_rate() const noexcept;
    uint32_t sample_rate() const noexcept;
    time time_per_frame() const noexcept;
    time time_per_sample() const noexcept;
    image_format frame_buffer_format() const noexcept;

  private:
    uint32_t m_width;
    uint32_t m_height;

  private:
    uint32_t m_frame_rate;
    time m_time_per_frame;

  private:
    uint32_t m_sample_rate;
    time m_time_per_sample;

  private:
    image_format m_frame_buffer_format;
  };
}