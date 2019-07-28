//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/data/lib/image_view.hpp>
#include <yave/data/lib/blend_operation.hpp>

namespace yave {

  /// Alpha blend images. Blend `src` image over `dst` image.
  /// \param src source image view (const)
  /// \param dst destination image view (mutable)
  /// \requires both `src` and `dst` images should have same size and format.
  void alpha_blend_RGBA8UI(
    const const_image_view& src,
    const mutable_image_view& dst,
    blend_operation op);

  /// Alpha blend images. Blend `src` image over `dst` image.
  /// \param src source image view (const)
  /// \param dst destination image view (mutable)
  /// \requires both `src` and `dst` images should have same size and format.
  void alpha_blend_RGBA16UI(
    const const_image_view& src,
    const mutable_image_view& dst,
    blend_operation op);

  /// Alpha blend images. Blend `src` image over `dst` image.
  /// \param src source image view (const)
  /// \param dst destination image view (mutable)
  /// \requires both `src` and `dst` images should have same size and format.
  void alpha_blend_RGBA32F(
    const const_image_view& src,
    const mutable_image_view& dst,
    blend_operation op);

} // namespace yave