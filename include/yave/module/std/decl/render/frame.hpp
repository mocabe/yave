//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/get_info.hpp>

namespace yave {

  namespace node {
    /// frame buffer
    class Frame;
  } // namespace node

  template <>
  struct node_declaration_traits<node::Frame>
  {
    static auto get_node_declaration()
    {
      return node_declaration(
        "Frame", "std::render", "Create new frame buffer", {}, {"value"});
    }
  };
}