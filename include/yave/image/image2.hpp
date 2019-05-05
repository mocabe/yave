//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/image/image_format2.hpp>

#include <cstring>
#include <cstdint>
#include <cstdlib>
#include <cassert>
#include <new>

namespace yave {

  /// \brief Image data container.
  /// Image class holds std::uint8_t array with ownership.
  /// Does not have functions to image manipulation.
  class image
  {
  public:
    /// Default constructor.
    image() = default;

    /// Construct image from raw data.
    /// \requires Arguments should be valid to success `is_valid()` after
    /// construction.
    image(
      uint8_t* data,
      uint64_t width,
      uint64_t height,
      uint64_t stride,
      uint16_t channels,
      uint16_t byte_per_channel,
      pixel_format pixel_format,
      sample_format sample_format) noexcept;

    /// Construct image from parameters.
    /// \effects Memory of size of `byte_size()` will be allocated.
    /// \requires Arguments should be valid to success `is_valid()` after
    /// construction.
    image(
      uint64_t width,
      uint64_t height,
      uint64_t stride,
      uint16_t channels,
      uint16_t byte_per_channel,
      pixel_format pixel_format,
      sample_format sample_format) noexcept;

    /// Construct image from parameters.
    /// \effects Memory of size of `byte_size()` will be allocated. `m_stride`
    /// will be set to `width * byte_per_channel * channels`. \requires
    /// Arguments should be valid to success `is_valid()` after construction.
    image(
      uint64_t width,
      uint64_t height,
      uint16_t channels,
      uint16_t byte_per_channel,
      pixel_format pixel_format,
      sample_format sample_format) noexcept;

    /// Create copy of image instance.
    /// \effects Clone `other.m_data` then copy other members. If memory
    /// allocation failed, object will be default initialized. \requires `other`
    /// should be valid to success `is_valid()`
    image(const image& other) noexcept;

    /// Move image instance.
    /// \effects Copy all members from `other`, then make `other` release its
    /// ownership.
    /// \requires `other` should be valid to success `is_valid()`.
    image(image&& other) noexcept;

    /// Destruct an image data.
    /// \effects Calls `std::free()` with `m_data`.
    /// \requires `m_data` should be valid to call `std::free()`.
    ~image() noexcept;

    /// Resets the image instance.
    /// \effects All memory allocated for the object will be deallocated, then
    /// all members will be set to default value. \requires `m_data` should be
    /// valid to call `std::free`.
    void clear() noexcept;

    /// Release the owenership of the instance.
    /// \effects Calls `clear()` without deallocating `m_data`.
    /// \requires Should always success.
    void release() noexcept;

    /// Check if the instance has data.
    /// \effects None.
    /// \requires `is_valid()`
    bool empty() const noexcept;

    /// get data pointer
    uint8_t* data() const noexcept;
    /// width
    uint64_t width() const noexcept;
    /// height
    uint64_t height() const noexcept;
    /// stride
    uint64_t stride() const noexcept;
    /// channels
    uint16_t channels() const noexcept;
    /// byte per channel
    uint16_t byte_per_channel() const noexcept;
    /// pixel fromat
    pixel_format pixel_format() const noexcept;
    /// sample format
    sample_format sample_format() const noexcept;

    /// pixel size
    uint64_t pixel_size() const noexcept;
    /// size in byte
    uint64_t byte_size() const noexcept;

  private:
    void allocate(std::uint64_t size) noexcept;
    void deallocate() noexcept;
    void copy_bytes(uint8_t* dst, uint8_t* src, uint64_t size) noexcept;
    bool is_valid() const;

  private:
    /* 8byte */

    /// data ptr
    uint8_t* m_data = nullptr;
    /// width
    uint64_t m_width = 0;
    /// height
    uint64_t m_height = 0;
    /// stride byte
    uint64_t m_stride = 0;

    /* 2byte */

    /// size of channel
    uint16_t m_channels = 0;
    /// byte per channel
    uint16_t m_byte_per_channel = 0;

    /* 1byte */

    /// pixel format
    yave::pixel_format m_pixel_format = pixel_format::Unknown;
    /// sample format
    yave::sample_format m_sample_format = sample_format::Unknown;
  };

} // namespace yave