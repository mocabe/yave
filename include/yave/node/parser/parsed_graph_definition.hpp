//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core.hpp>

#include <yave/node/parser/parsed_node_property.hpp>
#include <yave/node/parser/parsed_socket_property.hpp>
#include <yave/node/parser/parsed_edge_property.hpp>

namespace yave {

  /// Internal graph data for parsed_node_graph
  using parsed_graph_t = graph::
    graph<parsed_node_property, parsed_socket_property, parsed_edge_property>;

  /// Handle of node in parsed_node_graph
  using parsed_node_handle =
    descriptor_handle<typename parsed_graph_t::node_descriptor_type>;

  /// Handle of connection in parsed_node_graph
  using parsed_connection_handle =
    descriptor_handle<typename parsed_graph_t::edge_descriptor_type>;

} // namespace yave
