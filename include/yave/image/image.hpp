//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/image/image_format.hpp>

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
      uint32_t width,
      uint32_t height,
      image_format image_format) noexcept;

    /// Construct image from parameters.
    /// \effects Memory of size of `byte_size()` will be allocated.
    /// \requires Arguments should be valid to success `is_valid()` after
    /// construction.
    image(uint32_t width, uint32_t height, image_format image_format) noexcept;

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
    uint32_t width() const noexcept;
    /// height
    uint32_t height() const noexcept;
    /// number of channels
    uint32_t channels() const noexcept;
    /// format
    yave::image_format image_format() const noexcept;

    /// bytes per channels
    uint32_t byte_per_channel() const noexcept;
    /// pixel size
    uint32_t pixel_size() const noexcept;
    /// size in byte
    uint32_t byte_size() const noexcept;

  private:
    void allocate(std::uint64_t size) noexcept;
    void deallocate() noexcept;
    void copy_bytes(uint8_t* dst, uint8_t* src, uint64_t size) noexcept;
    bool is_valid() const;

  private:
    /// data ptr
    uint8_t* m_data = nullptr;
    /// width
    uint32_t m_width = 0;
    /// height
    uint32_t m_height = 0;
    /// image format
    yave::image_format m_format;
  };

} // namespace yave