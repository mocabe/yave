//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/get_info.hpp>
#include <yave/module/std/config.hpp>

namespace yave {

  namespace node::Logic {
    /// If
    struct If;
  } // namespace node

  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::Logic::If);
  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::Logic::If, modules::_std::tag);

} // namespace yave
