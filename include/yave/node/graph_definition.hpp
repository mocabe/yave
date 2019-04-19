//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/config/config.hpp>
#include <yave/graph/graph.hpp>
#include <yave/node/node_property.hpp>
#include <yave/node/socket_property.hpp>
#include <yave/node/edge_property.hpp>

namespace yave {

  /// Graph type used in node tree
  using graph_t = Graph<NodeProperty, SocketProperty, EdgeProperty>;

}