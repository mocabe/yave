//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/lib/image/image.hpp>

namespace yave {

  image::image(
    uint8_t* data,
    const uint32_t& width,
    const uint32_t& height,
    const yave::image_format& image_format,
    const std::pmr::polymorphic_allocator<std::byte>& alloc) noexcept
    : m_data {data}
    , m_width {width}
    , m_height {height}
    , m_format {image_format}
    , m_alloc {alloc}
  {
    assert(_is_valid());
  }

  image::image(
    const uint32_t& width,
    const uint32_t& height,
    const yave::image_format& image_format,
    const std::pmr::polymorphic_allocator<std::byte>& alloc)
    : m_data {nullptr}
    , m_width {width}
    , m_height {height}
    , m_format {image_format}
    , m_alloc {alloc}
  {
    m_data = _alloc(byte_size(), m_alloc);
    std::memset(m_data, 0, byte_size());
    assert(_is_valid());
  }

  image::image(
    const image& other,
    const std::pmr::polymorphic_allocator<std::byte>& alloc)
    : m_data {nullptr}
    , m_width {other.m_width}
    , m_height {other.m_height}
    , m_format {other.m_format}
    , m_alloc {alloc}
  {
    m_data = _alloc(other.byte_size(), m_alloc);
    _copy_bytes(m_data, other.m_data, other.byte_size());
    assert(_is_valid());
  }

  image::image(image&& other) noexcept
    : image(
      other.m_data,
      other.m_width,
      other.m_height,
      other.m_format,
      other.m_alloc)
  {
    other.m_data = nullptr;
    other.clear();
  }

  image& image::operator=(const image& other)
  {
    clear();
    m_data   = _alloc(other.byte_size(), m_alloc);
    m_width  = other.m_width;
    m_height = other.m_height;
    m_format = other.m_format;
    _copy_bytes(m_data, other.m_data, other.byte_size());
    assert(_is_valid());
    assert(other._is_valid());
    return *this;
  }

  image& image::operator=(image&& other)
  {
    if (m_alloc == other.m_alloc) {
      clear();
      std::swap(m_data, other.m_data);
      std::swap(m_width, other.m_width);
      std::swap(m_height, other.m_height);
      std::swap(m_format, other.m_format);
      assert(_is_valid());
      assert(other._is_valid());
      return *this;
    }
    return this->operator=(other);
  }

  image::~image() noexcept
  {
    _dealloc(m_data, byte_size(), m_alloc);
  }

  void image::clear() noexcept
  {
    _dealloc(m_data, byte_size(), m_alloc);
    m_data   = nullptr;
    m_width  = 0;
    m_height = 0;
    m_format = image_format::unknown;
    assert(_is_valid());
  }

  uint8_t* image::release() noexcept
  {
    uint8_t* tmp = nullptr;
    std::swap(tmp, m_data);
    clear();
    return tmp;
  }

  bool image::empty() const noexcept
  {
    assert(_is_valid());
    return m_data == nullptr;
  }

  uint8_t* image::data() noexcept
  {
    return m_data;
  }

  const uint8_t* image::data() const noexcept
  {
    return m_data;
  }

  auto image::allocator() const noexcept
    -> const std::pmr::polymorphic_allocator<std::byte>&
  {
    return m_alloc;
  }

  uint32_t image::width() const noexcept
  {
    return m_width;
  }

  uint32_t image::height() const noexcept
  {
    return m_height;
  }

  uint32_t image::channels() const noexcept
  {
    return channel_size(m_format);
  }

  uint32_t image::stride() const noexcept
  {
    return yave::byte_per_pixel(m_format) * m_width;
  }

  uint32_t image::byte_per_channel() const noexcept
  {
    return yave::byte_per_channel(m_format);
  }

  uint32_t image::byte_per_pixel() const noexcept
  {
    return yave::byte_per_pixel(m_format);
  }

  image_format image::image_format() const noexcept
  {
    return m_format;
  }

  uint32_t image::pixels() const noexcept
  {
    return m_width * m_height;
  }

  uint32_t image::byte_size() const noexcept
  {
    return m_width * m_height * byte_per_pixel();
  }

  auto image::_alloc(
    size_t size,
    std::pmr::polymorphic_allocator<std::byte>& alloc) -> uint8_t*
  {
    if (size == 0)
      return nullptr;
    else
      return (uint8_t*)alloc.allocate(size);
  }

  void image::_dealloc(
    uint8_t* ptr,
    size_t size,
    std::pmr::polymorphic_allocator<std::byte>& alloc) noexcept
  {
    if (ptr)
      alloc.deallocate((std::byte*)ptr, size);
  }

  void image::_copy_bytes(uint8_t* dst, uint8_t* src, uint64_t size) noexcept
  {
    std::memcpy(dst, src, size);
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
  bool image::_is_valid() const
  {
    if (m_data == nullptr) {
      return                               //
        m_width == 0 &&                    //
        m_height == 0 &&                   //
        m_format == image_format::unknown; //
    }
    return true;
  }

  mutable_image_view image::get_mutable_image_view() noexcept
  {
    return mutable_image_view(*this);
  }

  mutable_image_view image::get_image_view() noexcept
  {
    return get_mutable_image_view();
  }

  const_image_view image::get_const_image_view() const noexcept
  {
    return const_image_view(*this);
  }

  const_image_view image::get_image_view() const noexcept
  {
    return get_const_image_view();
  }
} // namespace yave