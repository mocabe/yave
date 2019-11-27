//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/lib/scene/scene_config.hpp>
#include <yave/support/log.hpp>

YAVE_DECL_G_LOGGER(scene_config);

namespace yave {

  scene_config::scene_config(
    uint32_t width,
    uint32_t height,
    uint32_t frame_rate,
    uint32_t sample_rate,
    image_format format)
  {
    init_logger();

    if (width == 0 || height == 0) {
      Error(
        g_logger,
        "Invalid frame buffer size: width{}, height:{}",
        width,
        height);

      throw std::invalid_argument("Invalid frame buffer size");
    }

    m_width  = width;
    m_height = height;

    if (frame_rate == 0) {
      Error(g_logger, "Invalid frame rate: {}", frame_rate);
      throw std::invalid_argument("Invalid frame rate");
    }

    if (!time::is_compatible_rate(frame_rate)) {
      Warning(
        "Specified frame rate is not compatible with internal time resolution. "
        "This may cause innaccurate time mapping.");
    }

    m_frame_rate     = frame_rate;
    m_time_per_frame = time::per_second() / frame_rate;

    if (sample_rate == 0) {
      Error(g_logger, "Invalid sample rate: {}", sample_rate);
      throw std::invalid_argument("Invalid sample rate");
    }

    if (!time::is_compatible_rate(sample_rate)) {
      Warning(
        "Specified sample rate is not compatible with internal time "
        "resolution. This may cause innaccurate time mapping.");
    }

    m_sample_rate     = sample_rate;
    m_time_per_sample = time::per_second() / sample_rate;

    // TODO: support multiple frame buffer formats
    if (format != image_format::RGBA32F) {
      Error("This frame buffer format is not supported: {}", to_string(format));
      throw std::invalid_argument("Invalid frame buffer format");
    }

    m_frame_buffer_format = format;
  }

  uint32_t scene_config::width() const noexcept
  {
    return m_width;
  }

  uint32_t scene_config::height() const noexcept
  {
    return m_height;
  }

  uint32_t scene_config::frame_rate() const noexcept
  {
    return m_frame_rate;
  }

  uint32_t scene_config::sample_rate() const noexcept
  {
    return m_sample_rate;
  }

  time scene_config::time_per_frame() const noexcept
  {
    return m_time_per_frame;
  }

  time scene_config::time_per_sample() const noexcept
  {
    return m_time_per_sample;
  }

  image_format scene_config::frame_buffer_format() const noexcept
  {
    return m_frame_buffer_format;
  }

} // namespace yave