//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/get_info.hpp>
#include <yave/module/std/config.hpp>

namespace yave {

  namespace node::Vec {
    /// float vec2
    struct Vec2;
    /// float vec3
    struct Vec3;
    /// float vec4
    struct Vec4;
  } // namespace node

  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::Vec::Vec2);
  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::Vec::Vec3);
  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::Vec::Vec4);

  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::Vec::Vec2, modules::_std::tag);
  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::Vec::Vec3, modules::_std::tag);
  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::Vec::Vec4, modules::_std::tag);
} // namespace yave