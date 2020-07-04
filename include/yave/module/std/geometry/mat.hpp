//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/get_info.hpp>
#include <yave/module/std/config.hpp>
#include <yave/module/std/transform/rotate.hpp>
#include <yave/module/std/transform/translate.hpp>

namespace yave {

  namespace node {
    /// Mat4
    struct Mat4;
  } // namespace node

  namespace modules::_std::geometry {
    /// tag
    struct tag;
  } // namespace modules::_std::geometry

  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::Mat4);
  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::Mat4, modules::_std::tag);

  // clang-format off
  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::Rotate, modules::_std::geometry::tag);
  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::RotateX, modules::_std::geometry::tag);
  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::RotateY, modules::_std::geometry::tag);
  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::RotateZ, modules::_std::geometry::tag);
  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::Translate, modules::_std::geometry::tag);
  // clang-format on
}