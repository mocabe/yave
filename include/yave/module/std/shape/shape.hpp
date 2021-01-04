//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/get_info.hpp>
#include <yave/module/std/config.hpp>

namespace yave {

  namespace node::Shape {
    class Shape;
    class Rect;
    class Circle;
  } // namespace node::Shape

  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::Shape::Shape);
  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::Shape::Rect);
  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::Shape::Circle);

  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::Shape::Shape, modules::_std::tag);
  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::Shape::Rect, modules::_std::tag);
  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::Shape::Circle, modules::_std::tag);
}