//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/lib/image/image.hpp>

#include <filesystem>

namespace yave {

  /// Load supported image from file.
  /// Uses file extension to detect file format.
  image load_image_auto(
    const std::filesystem::path& file,
    const std::pmr::polymorphic_allocator<std::byte>& alloc = {});

  /// Load PNG image from file
  image load_image_png(
    const std::filesystem::path& file,
    const std::pmr::polymorphic_allocator<std::byte>& alloc = {});

  /// Load JPEG image from file
  /// Supported: RGB8
  image load_image_jpeg(
    const std::filesystem::path& file,
    const std::pmr::polymorphic_allocator<std::byte>& alloc = {});

  /// Load TIFF image from file
  image load_image_tiff(
    const std::filesystem::path& file,
    const std::pmr::polymorphic_allocator<std::byte>& alloc = {});

} // namespace yave