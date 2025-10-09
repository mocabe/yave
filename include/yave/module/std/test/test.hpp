//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/get_info.hpp>
#include <yave/module/std/config.hpp>

namespace yave {

  namespace node::Test {

    class EventLogPaths;
  }

  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::Test::EventLogPaths);
  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::Test::EventLogPaths, modules::_std::tag);

} 
