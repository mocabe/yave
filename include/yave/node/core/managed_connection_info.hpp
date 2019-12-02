//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/connection_info.hpp>

namespace yave {

  /// connection_info for managed node graph
  class managed_connection_info
  {
  public:
    managed_connection_info()                               = delete;
    managed_connection_info(const managed_connection_info&) = default;
    managed_connection_info(managed_connection_info&&)      = default;

    managed_connection_info(
      const node_handle& src_node,
      const socket_handle& src_socket,
      const node_handle& dst_node,
      const socket_handle& dst_socket);

    /// Get source node.
    [[nodiscard]] auto src_node() const -> const node_handle&;

    /// Get socket of source node.
    [[nodiscard]] auto src_socket() const -> const socket_handle&;

    /// Get dest node.
    [[nodiscard]] auto dst_node() const -> const node_handle&;

    /// Get socket of dest noed.
    [[nodiscard]] auto dst_socket() const -> const socket_handle&;

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
}