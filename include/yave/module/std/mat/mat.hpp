//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/get_info.hpp>
#include <yave/module/std/config.hpp>

namespace yave {

  namespace node::Mat {
    struct Mat4;
    struct Rotate;
    struct RotateX;
    struct RotateY;
    struct RotateZ;
    struct Translate;
  } // namespace node

  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::Mat::Mat4);
  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::Mat::Rotate);
  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::Mat::RotateX);
  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::Mat::RotateY);
  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::Mat::RotateZ);
  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::Mat::Translate);

  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::Mat::Mat4, modules::_std::tag);
  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::Mat::Rotate, modules::_std::tag);
  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::Mat::RotateX, modules::_std::tag);
  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::Mat::RotateY, modules::_std::tag);
  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::Mat::RotateZ, modules::_std::tag);
  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::Mat::Translate, modules::_std::tag);
}