//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/get_info.hpp>
#include <yave/module/std/config.hpp>

namespace yave {

  namespace node {
    /// ReTime
    struct ReTime;
    /// DelayTime
    struct DelayTime;
    /// ScaleTime
    struct ScaleTime;
  } // namespace node

  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::ReTime);
  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::DelayTime);
  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::ScaleTime);

  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::ReTime, modules::_std::tag);
  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::DelayTime, modules::_std::tag);
  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::ScaleTime, modules::_std::tag);
}