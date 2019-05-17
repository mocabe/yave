//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/core/connection_info.hpp>

namespace yave {

  connection_info::connection_info(
    const node_handle& src_node,
    const std::string& src_socket,
    const node_handle& dst_node,
    const std::string& dst_socket)
    : m_src_node {src_node}
    , m_src_socket {src_socket}
    , m_dst_node {dst_node}
    , m_dst_socket {dst_socket}
  {
  }

  node_handle connection_info::src_node() const
  {
    return m_src_node;
  }

  const std::string& connection_info::src_socket() const
  {
    return m_src_socket;
  }

  node_handle connection_info::dst_node() const
  {
    return m_dst_node;
  }

  const std::string& connection_info::dst_socket() const
  {
    return m_dst_socket;
  }

  bool operator==(const connection_info& lhs, const connection_info& rhs)
  {
    return lhs.m_src_node == rhs.m_src_node &&
           lhs.m_src_socket == rhs.m_src_socket &&
           lhs.m_dst_node == rhs.m_dst_node &&
           lhs.m_dst_socket == rhs.m_dst_socket;
  }

  bool operator!=(const connection_info& lhs, const connection_info& rhs)
  {
    return !(lhs == rhs);
  }

} // namespace yave