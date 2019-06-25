//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/core/scene_config.hpp>
#include <yave/support/log.hpp>

namespace {
  std::shared_ptr<spdlog::logger> g_logger;

  void init_logger()
  {
    [[maybe_unused]] static auto init = [] {
      g_logger = yave::add_logger("scene_config");
      return 1;
    }();
  }
} // namespace

namespace yave {

  scene_config::scene_config(
    uint32_t width,
    uint32_t height,
    uint32_t frame_rate,
    uint32_t sample_rate,
    image_format format)
  {
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

  uint32_t scene_config::width() const
  {
    return m_width;
  }

  uint32_t scene_config::height() const
  {
    return m_height;
  }

  uint32_t scene_config::frame_rate() const
  {
    return m_frame_rate;
  }

  uint32_t scene_config::sample_rate() const
  {
    return m_sample_rate;
  }

  time scene_config::time_per_frame() const
  {
    return m_time_per_frame;
  }

  time scene_config::time_per_sample() const
  {
    return m_time_per_sample;
  }

  image_format scene_config::frame_buffer_format() const
  {
    return m_frame_buffer_format;
  }

} // namespace yave