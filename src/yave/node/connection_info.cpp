//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/connection_info.hpp>

namespace yave {

  ConnectionInfo::ConnectionInfo(
    const NodeHandle& src_node,
    const std::string& src_socket,
    const NodeHandle& dst_node,
    const std::string& dst_socket)
    : m_src_node {src_node}
    , m_src_socket {src_socket}
    , m_dst_node {dst_node}
    , m_dst_socket {dst_socket}
  {
  }

  NodeHandle ConnectionInfo::src_node() const
  {
    return m_src_node;
  }

  const std::string& ConnectionInfo::src_socket() const
  {
    return m_src_socket;
  }

  NodeHandle ConnectionInfo::dst_node() const
  {
    return m_dst_node;
  }

  const std::string& ConnectionInfo::dst_socket() const
  {
    return m_dst_socket;
  }

  bool operator==(const ConnectionInfo& lhs, const ConnectionInfo& rhs)
  {
    return lhs.m_src_node == rhs.m_src_node &&
           lhs.m_src_socket == rhs.m_src_socket &&
           lhs.m_dst_node == rhs.m_dst_node &&
           lhs.m_dst_socket == rhs.m_dst_socket;
  }

  bool operator!=(const ConnectionInfo& lhs, const ConnectionInfo& rhs)
  {
    return !(lhs == rhs);
  }

} // namespace yave