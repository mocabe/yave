//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/lib/image/image.hpp>
#include <yave/lib/filesystem/path.hpp>

namespace yave {

  /// Load supported image from file.
  /// Uses extension to detect file format.
  image load_image_auto(const filesystem::path& file);

  /// Load PNG image from file
  image load_image_png(const filesystem::path& file);

  /// Load JPEG image from file
  image load_image_jpeg(const filesystem::path& file);

  /// Load TIFF image from file
  image load_image_tiff(const filesystem::path& file);

} // namespace yave