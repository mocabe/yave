//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/node_handle.hpp>
#include <yave/node/core/socket_handle.hpp>

#include <vector>

namespace yave {

  /// Socket connect info
  class basic_connection_info
  {
  public:
    basic_connection_info()                             = delete;
    basic_connection_info(const basic_connection_info&) = default;
    basic_connection_info(basic_connection_info&&)      = default;
    basic_connection_info(
      const node_handle& src_node,
      const socket_handle& src_socket,
      const node_handle& dst_node,
      const socket_handle& dst_socket);

    basic_connection_info(
      const node_handle& src_node,
      const socket_handle& src_socket,
      const node_handle& dst_node,
      const socket_handle& dst_socket,
      std::vector<node_handle> src_interfaces,
      std::vector<node_handle> dst_interfaces);

    /// Get source node.
    [[nodiscard]] auto src_node() const -> const node_handle&;

    /// Get socket of source node.
    [[nodiscard]] auto src_socket() const -> const socket_handle&;

    /// Get dest node.
    [[nodiscard]] auto dst_node() const -> const node_handle&;

    /// Get socket of dest noed.
    [[nodiscard]] auto dst_socket() const -> const socket_handle&;

    /// Get interface nodes on src socket
    [[nodiscard]] auto src_interfaces() const
      -> const std::vector<node_handle>&;

    /// Get interface nodes on dst socket
    [[nodiscard]] auto dst_interfaces() const
      -> const std::vector<node_handle>&;

  private:
    /// source node
    node_handle m_src_node;
    /// source socket
    socket_handle m_src_socket;
    /// destination node
    node_handle m_dst_node;
    /// destination socket
    socket_handle m_dst_socket;

  private:
    /// interface nodes sharing src socket
    std::vector<node_handle> m_src_interfaces;
    /// interface nodes sharing dst socket
    std::vector<node_handle> m_dst_interfaces;
  };

} // namespace yave