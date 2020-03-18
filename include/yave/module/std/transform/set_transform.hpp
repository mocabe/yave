//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/get_info.hpp>
#include <yave/module/std/config.hpp>

namespace yave {

  namespace node {
    /// SetTransform
    struct SetTransform;
  } // namespace node

  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::SetTransform);
  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::SetTransform, modules::_std::tag);
}