//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/get_info.hpp>
#include <yave/module/std/config.hpp>

namespace yave {

  namespace node::Random {

    // uniform distribution
    class Uniform;
    // normal distribution
    class Normal;

  } // namespace node::Random

  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::Random::Uniform);
  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::Random::Normal);
  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::Random::Uniform, modules::_std::tag);
  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::Random::Normal, modules::_std::tag);
}