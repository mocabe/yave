//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/config/config.hpp>
#include <yave/node/core/node_handle.hpp>

namespace yave {

  /// Layer composition
  struct layer_composition
  {
    /// Handle to compositor node
    node_handle compositor;
    /// Image input node
    node_handle input_node;
    /// Image input socket
    std::string input_socket;
    /// Image blender node
    node_handle blender_node;
    /// Image blender socket
    std::string blender_socket;
  };
}