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
    struct Rotate;
    /// RotateX
    struct RotateX;
    /// RotateY
    struct RotateY;
    /// RotateZ
    struct RotateZ;
  } // namespace node

  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::Rotate);
  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::RotateX);
  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::RotateY);
  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::RotateZ);

  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::Rotate, modules::_std::tag);
  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::RotateX, modules::_std::tag);
  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::RotateY, modules::_std::tag);
  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::RotateZ, modules::_std::tag);

} // namespace yave