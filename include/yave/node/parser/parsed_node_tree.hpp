//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core.hpp>
#include <yave/core/graph.hpp>

#include <yave/node/parser/parsed_node_property.hpp>
#include <yave/node/parser/parsed_socket_property.hpp>
#include <yave/node/parser/parsed_edge_property.hpp>

namespace yave {

  class parsed_node_tree
  {
    friend class node_parser;

  public:
  private:
    graph::
      graph<parsed_node_property, parsed_socket_property, parsed_edge_property>
        m_graph;
  };

} // namespace yave