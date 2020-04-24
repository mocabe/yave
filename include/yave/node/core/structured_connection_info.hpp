//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/connection_info.hpp>

namespace yave {

  /// connection_info for managed node graph
  class structured_connection_info
  {
  public:
    structured_connection_info()                                  = delete;
    structured_connection_info(const structured_connection_info&) = default;
    structured_connection_info(structured_connection_info&&)      = default;

    structured_connection_info(
      const node_handle& src_node,
      const socket_handle& src_socket,
      const node_handle& dst_node,
      const socket_handle& dst_socket)
      : m_src_node {src_node}
      , m_src_socket {src_socket}
      , m_dst_node {dst_node}
      , m_dst_socket {dst_socket}
    {
    }

    /// Get source node.
    [[nodiscard]] auto& src_node() const
    {
      return m_src_node;
    }

    /// Get socket of source node.
    [[nodiscard]] auto& src_socket() const
    {
      return m_src_socket;
    }

    /// Get dest node.
    [[nodiscard]] auto& dst_node() const
    {
      return m_dst_node;
    }

    /// Get socket of dest noed.
    [[nodiscard]] auto& dst_socket() const
    {
      return m_dst_socket;
    }

  private:
    /// source node
    node_handle m_src_node;
    /// source socket
    socket_handle m_src_socket;
    /// destination node
    node_handle m_dst_node;
    /// destination socket
    socket_handle m_dst_socket;
  };
} // namespace yave