//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/get_info.hpp>
#include <yave/module/std/config.hpp>

namespace yave {

  namespace node {
    class Rect2;
    class Rect3;
  } // namespace node

  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::Rect2);
  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::Rect3);

  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::Rect2, modules::_std::tag);
  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::Rect3, modules::_std::tag);
}