//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/get_info.hpp>
#include <yave/module/std/config.hpp>

namespace yave {

  namespace node {
    /// Translate
    struct Translate;
  } // namespace node

  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::Translate);
  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::Translate, modules::_std::tag);

} // namespace yave
