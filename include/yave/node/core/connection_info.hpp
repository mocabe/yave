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

    /// Get source node.
    [[nodiscard]] node_handle src_node() const;

    /// Get socket of source node.
    [[nodiscard]] const std::string& src_socket() const;

    /// Get dest node.
    [[nodiscard]] node_handle dst_node() const;

    /// Get socket of dest noed.
    [[nodiscard]] const std::string& dst_socket() const;

    friend bool
      operator==(const connection_info& lhs, const connection_info& rhs);

    friend bool
      operator!=(const connection_info& lhs, const connection_info& rhs);

  private:
    /// source node
    node_handle m_src_node;
    /// source socket
    std::string m_src_socket;
    /// destination node
    node_handle m_dst_node;
    /// destination socket
    std::string m_dst_socket;
  };

  /// operator==
  [[nodiscard]] bool
    operator==(const connection_info& lhs, const connection_info& rhs);

  /// operator!=
  [[nodiscard]] bool
    operator!=(const connection_info& lhs, const connection_info& rhs);

} // namespace yave