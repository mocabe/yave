//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/get_info.hpp>
#include <yave/module/std/config.hpp>

namespace yave {

  namespace node {
    struct ShapeTranslate;
    struct ShapeRotate;
    struct ShapeScale;
  } // namespace node

  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::ShapeTranslate);
  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::ShapeRotate);
  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::ShapeScale);

  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::ShapeTranslate, modules::_std::tag);
  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::ShapeRotate, modules::_std::tag);
  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::ShapeScale, modules::_std::tag);
}