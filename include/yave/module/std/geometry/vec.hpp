//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/get_info.hpp>
#include <yave/module/std/config.hpp>
#include <yave/module/std/primitive/data_holder.hpp>
#include <yave/obj/vec/vec.hpp>

namespace yave {

  namespace node {
    /// float vec2
    struct Vec2;
    /// float vec3
    struct Vec3;
    /// float vec4
    struct Vec4;
  } // namespace node

  using FVec2DataProperty = ValueDataTypeProperty<FVec2>;
  YAVE_DECL_DATA_TYPE_PROPERTY(FVec2, FVec2, FVec2DataProperty);

  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::Vec2);
  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::Vec3);
  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::Vec4);

  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::Vec2, modules::_std::tag);
  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::Vec3, modules::_std::tag);
  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::Vec4, modules::_std::tag);
}

YAVE_DECL_TYPE(yave::FVec2DataProperty, "0b29b7c3-3c2f-4119-867a-d739b39964f0");