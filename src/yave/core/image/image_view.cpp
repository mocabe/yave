//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/core/data_types/image_view.hpp>

namespace yave {

  image_view::image_view(
    uint8_t* data,
    uint32_t width,
    uint32_t height,
    yave::image_format image_format) noexcept
    : m_data {data}
    , m_width {width}
    , m_height {height}
    , m_format {image_format}
  {
  }

  image_view::image_view(
    uint32_t width,
    uint32_t height,
    yave::image_format image_format) noexcept
    : m_data {nullptr}
    , m_width {width}
    , m_height {height}
    , m_format {image_format}
  {
  }

  image_view::image_view(const yave::image& image) noexcept
    : m_data {image.data()}
    , m_width {image.width()}
    , m_height {image.height()}
    , m_format {image.image_format()}
  {
  }

  void image_view::clear() noexcept
  {
    m_data   = nullptr;
    m_width  = 0;
    m_height = 0;
    m_format = image_format::Unknown;
  }

  bool image_view::empty() const noexcept
  {
    if (m_data == nullptr) {
      assert(m_width == 0);
      assert(m_height == 0);
      assert(m_format == image_format::Unknown);
    }
    return m_data == nullptr;
  }

  const uint8_t* image_view::data() const noexcept
  {
    return m_data;
  }

  yave::image_format image_view::image_format() const noexcept
  {
    return m_format;
  }

  uint32_t image_view::width() const noexcept
  {
    return m_width;
  }

  uint32_t image_view::height() const noexcept
  {
    return m_height;
  }

  uint32_t image_view::stride() const noexcept
  {
    return yave::byte_per_pixel(m_format) * m_width;
  }

  uint32_t image_view::channels() const noexcept
  {
    return yave::channel_size(m_format);
  }

  uint32_t image_view::pixels() const noexcept
  {
    return m_width * m_height;
  }

  uint32_t image_view::byte_per_channel() const noexcept
  {
    return yave::byte_per_channel(m_format);
  }

  uint32_t image_view::byte_per_pixel() const noexcept
  {
    return yave::byte_per_pixel(m_format);
  }

  uint32_t image_view::byte_size() const noexcept
  {
    return stride() * m_height;
  }

} // namespace yave