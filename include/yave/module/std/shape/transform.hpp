//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/get_info.hpp>
#include <yave/module/std/config.hpp>

namespace yave {

  namespace node::Shape {
    struct Translate;
    struct Rotate;
    struct Scale;
  } // namespace node

  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::Shape::Translate);
  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::Shape::Rotate);
  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::Shape::Scale);

  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::Shape::Translate, modules::_std::tag);
  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::Shape::Rotate, modules::_std::tag);
  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::Shape::Scale, modules::_std::tag);
}