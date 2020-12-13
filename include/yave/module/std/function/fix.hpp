//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/get_info.hpp>
#include <yave/module/std/config.hpp>

namespace yave {

  namespace node::Functional {
    /// Fix
    struct Fix;
  } // namespace node::Functional

  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::Functional::Fix);
  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::Functional::Fix, modules::_std::tag);

} // namespace yave
