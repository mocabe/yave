//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/get_info.hpp>
#include <yave/module/std/config.hpp>

namespace yave {

  namespace node {
    /// Rotate
    /// a -> Float -> FVec3 -> a
    struct Rotate;
    /// RotateX
    /// a -> Float -> a
    struct RotateX;
    /// RotateY
    /// a -> Float -> a
    struct RotateY;
    /// RotateZ
    /// a -> Float -> a
    struct RotateZ;
  } // namespace node

  // declaration only
  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::Rotate);
  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::RotateX);
  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::RotateY);
  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::RotateZ);
} // namespace yave