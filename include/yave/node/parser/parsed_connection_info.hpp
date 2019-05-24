//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/parser/parsed_graph_definition.hpp>

namespace yave {

  /// Connection info of parsed node graph.
  class parsed_connection_info
  {
  public:
    parsed_connection_info()                              = default;
    parsed_connection_info(const parsed_connection_info&) = default;
    parsed_connection_info(parsed_connection_info&&)      = default;
    parsed_connection_info& operator=(const parsed_connection_info&) = default;
    parsed_connection_info& operator=(parsed_connection_info&&) = default;

    parsed_connection_info(
      const parsed_node_handle& src_node,
      const parsed_node_handle& dst_node,
      const std::string& dst_socket)
      : m_src_node {src_node}
      , m_dst_node {dst_node}
      , m_dst_socket {dst_socket}
    {
    }

    /// Get src node.
    const parsed_node_handle& src_node() const
    {
      return m_src_node;
    }

    /// Get dst node.
    const parsed_node_handle& dst_node() const
    {
      return m_dst_node;
    }

    /// Get dst socket.
    const std::string& dst_socket() const
    {
      return m_dst_socket;
    }

  private:
    parsed_node_handle m_src_node;
    parsed_node_handle m_dst_node;
    std::string m_dst_socket;
  };
} // namespace yave