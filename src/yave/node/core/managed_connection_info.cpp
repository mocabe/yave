//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/core/managed_connection_info.hpp>

namespace yave{

  managed_connection_info::managed_connection_info(
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

  auto managed_connection_info::src_node() const -> const node_handle&
  {
    return m_src_node;
  }

  auto managed_connection_info::src_socket() const -> const socket_handle&
  {
    return m_src_socket;
  }

  auto managed_connection_info::dst_node() const -> const node_handle&
  {
    return m_dst_node;
  }

  auto managed_connection_info::dst_socket() const -> const socket_handle&
  {
    return m_dst_socket;
  }

} // namespace yave