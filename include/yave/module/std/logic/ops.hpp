//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/get_info.hpp>
#include <yave/module/std/config.hpp>

namespace yave {

  namespace node::Logic {

    struct And;
    struct Or;
    struct Xor;
    struct Not;

  } // namespace node::Logic

  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::Logic::And);
  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::Logic::Or);
  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::Logic::Xor);
  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::Logic::Not);

  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::Logic::And, modules::_std::tag);
  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::Logic::Or, modules::_std::tag);
  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::Logic::Xor, modules::_std::tag);
  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::Logic::Not, modules::_std::tag);
}