//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/get_info.hpp>
#include <yave/module/std/config.hpp>

namespace yave {

  namespace node::Time {
    /// ReTime
    struct ReTime;
    /// Delay time
    struct Delay;
    /// Scale time
    struct Scale;
  } // namespace node

  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::Time::ReTime);
  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::Time::Delay);
  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::Time::Scale);

  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::Time::ReTime, modules::_std::tag);
  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::Time::Delay, modules::_std::tag);
  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::Time::Scale, modules::_std::tag);
}