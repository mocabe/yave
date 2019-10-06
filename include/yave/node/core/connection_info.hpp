//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/node_handle.hpp>

namespace yave {

  /// Socket connect info
  class connection_info
  {
  public:
    connection_info()                       = delete;
    connection_info(const connection_info&) = default;
    connection_info(connection_info&&)      = default;
    connection_info(
      const node_handle& src_node,
      const std::string& src_socket,
      const node_handle& dst_node,
      const std::string& dst_socket);

    connection_info(
      const node_handle& src_node,
      const std::string& src_socket,
      const node_handle& dst_node,
      const std::string& dst_socket,
      const std::vector<node_handle>& src_interfaces,
      const std::vector<node_handle>& dst_interfaces);

    /// Get source node.
    [[nodiscard]] auto src_node() const -> const node_handle&;

    /// Get socket of source node.
    [[nodiscard]] auto src_socket() const -> const std::string&;

    /// Get dest node.
    [[nodiscard]] auto dst_node() const -> const node_handle&;

    /// Get socket of dest noed.
    [[nodiscard]] auto dst_socket() const -> const std::string&;

    /// Get interface nodes on src socket
    [[nodiscard]] auto src_interfaces() const
      -> const std::vector<node_handle>&;

    /// Get interface nodes on dst socket
    [[nodiscard]] auto dst_interfaces() const
      -> const std::vector<node_handle>&;

    friend bool operator==(
      const connection_info& lhs,
      const connection_info& rhs);

    friend bool operator!=(
      const connection_info& lhs,
      const connection_info& rhs);

  private:
    /// source node
    node_handle m_src_node;
    /// source socket
    std::string m_src_socket;
    /// destination node
    node_handle m_dst_node;
    /// destination socket
    std::string m_dst_socket;

  private:
    /// interface nodes sharing src socket
    std::vector<node_handle> m_src_interfaces;
    /// interface nodes sharing dst socket
    std::vector<node_handle> m_dst_interfaces;
  };

  /// operator==
  [[nodiscard]] bool operator==(
    const connection_info& lhs,
    const connection_info& rhs);

  /// operator!=
  [[nodiscard]] bool operator!=(
    const connection_info& lhs,
    const connection_info& rhs);

} // namespace yave