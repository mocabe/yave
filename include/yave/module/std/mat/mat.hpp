//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/get_info.hpp>
#include <yave/module/std/config.hpp>

namespace yave {

  namespace node {
    struct Mat4;
    struct Mat4Rotate;
    struct Mat4RotateX;
    struct Mat4RotateY;
    struct Mat4RotateZ;
    struct Mat4Translate;
  } // namespace node

  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::Mat4);
  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::Mat4Rotate);
  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::Mat4RotateX);
  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::Mat4RotateY);
  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::Mat4RotateZ);
  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::Mat4Translate);

  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::Mat4, modules::_std::tag);
  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::Mat4Rotate, modules::_std::tag);
  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::Mat4RotateX, modules::_std::tag);
  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::Mat4RotateY, modules::_std::tag);
  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::Mat4RotateZ, modules::_std::tag);
  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::Mat4Translate, modules::_std::tag);
}