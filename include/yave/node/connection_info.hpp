//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/node_handle.hpp>

namespace yave {

  /// Socket connect info
  class ConnectionInfo
  {
  public:
    ConnectionInfo()                      = delete;
    ConnectionInfo(const ConnectionInfo&) = default;
    ConnectionInfo(ConnectionInfo&&)      = default;
    ConnectionInfo(
      const NodeHandle& src_node,
      const std::string& src_socket,
      const NodeHandle& dst_node,
      const std::string& dst_socket);

    /// Get source node.
    [[nodiscard]] NodeHandle src_node() const;

    /// Get socket of source node.
    [[nodiscard]] const std::string& src_socket() const;

    /// Get dest node.
    [[nodiscard]] NodeHandle dst_node() const;

    /// Get socket of dest noed.
    [[nodiscard]] const std::string& dst_socket() const;

    friend bool
      operator==(const ConnectionInfo& lhs, const ConnectionInfo& rhs);

    friend bool
      operator!=(const ConnectionInfo& lhs, const ConnectionInfo& rhs);

  private:
    /// source node
    NodeHandle m_src_node;
    /// source socket
    std::string m_src_socket;
    /// destination node
    NodeHandle m_dst_node;
    /// destination socket
    std::string m_dst_socket;
  };

  /// operator==
  [[nodiscard]] bool
    operator==(const ConnectionInfo& lhs, const ConnectionInfo& rhs);

  /// operator!=
  [[nodiscard]] bool
    operator!=(const ConnectionInfo& lhs, const ConnectionInfo& rhs);

} // namespace yave