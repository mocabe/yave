//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/data/lib/image2.hpp>

namespace yave {

  image::image(
    uint8_t* data,
    uint64_t width,
    uint64_t height,
    uint64_t stride,
    uint16_t channels,
    uint16_t byte_per_channel,
    yave::pixel_format pixel_format,
    yave::sample_format sample_format) noexcept
    : m_data {data}
    , m_width {width}
    , m_height {height}
    , m_stride {stride}
    , m_channels {channels}
    , m_byte_per_channel {byte_per_channel}
    , m_pixel_format {pixel_format}
    , m_sample_format {sample_format}
  {
    assert(is_valid());
  }

  image::image(
    uint64_t width,
    uint64_t height,
    uint64_t stride,
    uint16_t channels,
    uint16_t byte_per_channel,
    yave::pixel_format pixel_format,
    yave::sample_format sample_format) noexcept
    : m_data {nullptr}
    , m_width {width}
    , m_height {height}
    , m_stride {stride}
    , m_channels {channels}
    , m_byte_per_channel {byte_per_channel}
    , m_pixel_format {pixel_format}
    , m_sample_format {sample_format}
  {
    allocate(byte_size());

    if (m_data == nullptr)
      clear();
    else
      std::memset(m_data, 0, byte_size());

    assert(is_valid());
  }

  image::image(
    uint64_t width,
    uint64_t height,
    uint16_t channels,
    uint16_t byte_per_channel,
    yave::pixel_format pixel_format,
    yave::sample_format sample_format) noexcept
    : image(
        width,
        height,
        width * byte_per_channel * channels,
        channels,
        byte_per_channel,
        pixel_format,
        sample_format)
  {
  }

  image::image(const image& other) noexcept
    : image()
  {
    if (other.m_data != nullptr) {
      allocate(other.byte_size());
      if (m_data != nullptr) {
        copy_bytes(m_data, other.m_data, other.byte_size());
        m_width            = other.m_width;
        m_height           = other.m_height;
        m_stride           = other.m_stride;
        m_channels         = other.m_channels;
        m_byte_per_channel = other.m_byte_per_channel;
        m_pixel_format     = other.m_pixel_format;
        m_sample_format    = other.m_sample_format;
      }
    }
    assert(is_valid());
  }

  image::image(image&& other) noexcept
    : image(
        other.m_data,
        other.m_width,
        other.m_height,
        other.m_stride,
        other.m_channels,
        other.m_byte_per_channel,
        other.m_pixel_format,
        other.m_sample_format)
  {
    other.m_data = nullptr;
    // `clear()` is not called so `other` will become invalid, but destructor
    // accepts invalid object as long as `m_data` is valid to call
    // `std::free()`.
  }

  image::~image() noexcept
  {
    deallocate();
  }

  void image::clear() noexcept
  {
    deallocate();
    m_width            = 0;
    m_height           = 0;
    m_stride           = 0;
    m_channels         = 0;
    m_byte_per_channel = 0;
    m_sample_format    = sample_format::Unknown;
    m_pixel_format     = pixel_format::Unknown;
    assert(is_valid());
  }

  void image::release() noexcept
  {
    m_data = nullptr;
    clear();
  }

  bool image::empty() const noexcept
  {
    assert(is_valid());
    return m_data == nullptr;
  }

  uint8_t* image::data() const noexcept
  {
    return m_data;
  }

  uint64_t image::width() const noexcept
  {
    return m_width;
  }

  uint64_t image::height() const noexcept
  {
    return m_height;
  }

  uint64_t image::stride() const noexcept
  {
    return m_stride;
  }

  uint16_t image::channels() const noexcept
  {
    return m_channels;
  }

  uint16_t image::byte_per_channel() const noexcept
  {
    return m_byte_per_channel;
  }

  pixel_format image::pixel_format() const noexcept
  {
    return m_pixel_format;
  }

  sample_format image::sample_format() const noexcept
  {
    return m_sample_format;
  }

  uint64_t image::pixel_size() const noexcept
  {
    return m_width * m_height;
  }

  uint64_t image::byte_size() const noexcept
  {
    return m_stride * m_height;
  }

  /** \brief Check if the instance is in valid state.
   * Validate the instance following rules below:
   *   if `m_data` is `nullptr`, then other members should be initialized as
   * default, otherwise `m_stride` should be larger than `m_width *
   * m_byte_per_channel * m_channels`.
   *
   * \effects None.
   * \requires None.
   * \returns Boolean result of validation.
   * \notes internal
   */
  bool image::is_valid() const
  {
    if (m_data == nullptr) {
      return                                       //
        m_width == 0 &&                            //
        m_height == 0 &&                           //
        m_stride == 0 &&                           //
        m_channels == 0 &&                         //
        m_byte_per_channel == 0 &&                 //
        m_pixel_format == pixel_format::Unknown && //
        m_sample_format == sample_format::Unknown; //
    } else {
      return m_stride >= m_width * m_byte_per_channel * m_channels;
    }
  }

  /** \brief Allocate memory.
   * \param size Byte size to allocate.
   * \effects Deallocate current data and set allocated memory to `m_data`.
   * \requires None.
   * \notes internal
   */
  void image::allocate(uint64_t size) noexcept
  {
    deallocate();
    m_data = (uint8_t*)std::malloc(size);
  }

  /** \brief Deallocate `m_data`.
   * \effects Calls `std::free()` with `m_data` and set `m_data` to `nullptr`.
   * \requires `m_data` should be valid to call`std::free()`.
   * \notes internal
   */
  void image::deallocate() noexcept
  {
    std::free(m_data);
    m_data = nullptr;
  }

  /** \brief copy data.
   * Copy `size` bytes of data from `src` to `dst`.
   *
   * \effects Calls `std::memcpy()` with arguments.
   * \requires Arguments should be valid to calls `std::memcpy()`.
   * \notes internal
   */
  void image::copy_bytes(uint8_t* dst, uint8_t* src, uint64_t size) noexcept
  {
    std::memcpy(dst, src, size);
  }

}