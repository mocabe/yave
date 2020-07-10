//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/lib/image/image_format.hpp>
#include <yave/lib/image/image_view.hpp>

#include <cstring>
#include <cstdint>
#include <cstdlib>
#include <cassert>
#include <memory_resource>

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
    /// \requires `data` should be valid memory storage allocated using `alloc`
    image(
      std::byte* data,
      const uint32_t& width,
      const uint32_t& height,
      const image_format& image_format,
      const std::pmr::polymorphic_allocator<std::byte>& alloc) noexcept;

    /// Construct image from parameters.
    /// \effects Memory of size of `byte_size()` will be allocated.
    /// \requires Arguments should be valid to success `is_valid()` after
    /// construction.
    image(
      const uint32_t& width,
      const uint32_t& height,
      const image_format& image_format,
      const std::pmr::polymorphic_allocator<std::byte>& alloc = {});

    /// Create copy of image instance.
    /// \effects Clone `other.m_data` then copy other members. If memory
    /// allocation failed, object will be default initialized. \requires `other`
    /// should be valid to success `is_valid()`
    image(
      const image& other,
      const std::pmr::polymorphic_allocator<std::byte>& alloc = {});

    /// Move image instance.
    /// \effects Copy all members from `other`, then make `other` release its
    /// ownership.
    /// \requires `other` should be valid to success `is_valid()`.
    image(image&& other) noexcept;

    /// Copy assign.
    /// \effects Copy image from other image instance. Uses its own allocator.
    /// \requres `other.is_valid()`
    image& operator=(const image& other);

    /// Move assign
    /// \effects Move image from other image when both have same allocator,
    /// otherwise fallback to copy.
    /// \requres `other.is_valid()`
    image& operator=(image&& other);

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
    [[nodiscard]] auto release() noexcept -> std::byte*;

    /// Check if the instance has data.
    /// \effects None.
    /// \requires `is_valid()`
    [[nodiscard]] bool empty() const noexcept;

    /// get data pointer
    [[nodiscard]] const std::byte* data() const noexcept;

    /// get data pointer
    [[nodiscard]] std::byte* data() noexcept;

    /// format
    [[nodiscard]] yave::image_format image_format() const noexcept;

    /// allocator
    [[nodiscard]] auto allocator() const noexcept
      -> const std::pmr::polymorphic_allocator<std::byte>&;

    /// width
    [[nodiscard]] uint32_t width() const noexcept;

    /// height
    [[nodiscard]] uint32_t height() const noexcept;

    /// stride
    [[nodiscard]] uint32_t stride() const noexcept;

    /// number of channels
    [[nodiscard]] uint32_t channels() const noexcept;

    /// pixel size
    [[nodiscard]] uint32_t pixels() const noexcept;

    /// bytes per channels
    [[nodiscard]] uint32_t byte_per_channel() const noexcept;

    /// bytes per pixel
    [[nodiscard]] uint32_t byte_per_pixel() const noexcept;

    /// size in byte
    [[nodiscard]] uint32_t byte_size() const noexcept;

    /// Get mutable image view
    [[nodiscard]] mutable_image_view get_mutable_image_view() noexcept;

    /// Get mutable image view
    [[nodiscard]] mutable_image_view get_image_view() noexcept;

    /// Get const image view
    [[nodiscard]] const_image_view get_image_view() const noexcept;

    /// Get const image view
    [[nodiscard]] const_image_view get_const_image_view() const noexcept;

  private:
    /// Allocate memory using allocator
    std::byte* _alloc(
      size_t size,
      std::pmr::polymorphic_allocator<std::byte>& alloc);
    /// Deallocate memory
    void _dealloc(
      std::byte* ptr,
      size_t size,
      std::pmr::polymorphic_allocator<std::byte>& alloc) noexcept;
    /// Copy bytes from src to dst
    void _copy_bytes(std::byte* dst, std::byte* src, uint64_t size) noexcept;
    /// Validation
    bool _is_valid() const;

  private:
    /// data ptr
    std::byte* m_data = nullptr;
    /// width
    uint32_t m_width = 0;
    /// height
    uint32_t m_height = 0;
    /// image format
    yave::image_format m_format = yave::image_format::unknown;
    /// allocator
    std::pmr::polymorphic_allocator<std::byte> m_alloc;
  };

} // namespace yave