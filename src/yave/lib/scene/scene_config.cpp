//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/lib/scene/scene_config.hpp>
#include <yave/support/log.hpp>

YAVE_DECL_G_LOGGER(scene_config)

namespace yave {

  scene_config::scene_config()
    : scene_config(
      1280,
      729,
      60,
      yave::image_format::rgba8,
      yave::audio_format::pcm_44100_stereo)
  {
  }

  scene_config::scene_config(
    uint32_t width,
    uint32_t height,
    uint32_t frame_rate,
    yave::image_format frame_format,
    yave::audio_format audio_format)
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
        g_logger,
        "Specified frame rate is not compatible with internal time resolution. "
        "This may cause innaccurate time mapping.");
    }

    m_frame_rate = frame_rate;

    if (frame_format == image_format::unknown) {
      throw std::invalid_argument("Invalid frame buffer format");
    }
    m_frame_format = frame_format;

    if (audio_format == audio_format::unknown) {
      throw std::invalid_argument("Invalid audio format");
    }
    m_audio_format = audio_format;
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

  image_format scene_config::frame_format() const noexcept
  {
    return m_frame_format;
  }

  audio_format scene_config::audio_format() const noexcept
  {
    return m_audio_format;
  }

} // namespace yave