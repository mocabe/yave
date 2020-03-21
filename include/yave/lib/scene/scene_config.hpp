//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/config/config.hpp>
#include <yave/lib/time/time.hpp>
#include <yave/lib/image/image_format.hpp>
#include <yave/lib/audio/audio_format.hpp>

namespace yave {

  class scene_config
  {
  public:
    /// Setup scene config.
    /// \param width non-zero frame buffer width
    /// \param height non-zero frame buffer height
    /// \param frame_rate non-zero frame rate (fps)
    /// \param frame_format frame buffer format
    /// \param audio_format audio buffer format
    scene_config(
      uint32_t width,
      uint32_t height,
      uint32_t frame_rate,
      image_format frame_format,
      audio_format audio_format);

    ~scene_config() noexcept                    = default;
    scene_config(const scene_config& other)     = default;
    scene_config(scene_config&& other) noexcept = default;
    scene_config& operator=(const scene_config& other) = default;
    scene_config& operator=(scene_config&& other) noexcept = default;

    [[nodiscard]] auto width() const noexcept -> uint32_t;
    [[nodiscard]] auto height() const noexcept -> uint32_t;
    [[nodiscard]] auto frame_rate() const noexcept -> uint32_t;
    [[nodiscard]] auto frame_format() const noexcept -> image_format;
    [[nodiscard]] auto audio_format() const noexcept -> audio_format;

  private:
    uint32_t m_width;
    uint32_t m_height;

  private:
    uint32_t m_frame_rate;

  private:
    yave::image_format m_frame_format;
    yave::audio_format m_audio_format;
  };
} // namespace yave