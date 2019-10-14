//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/core/connection_info.hpp>

namespace yave {

  connection_info::connection_info(
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

  connection_info::connection_info(
    const node_handle& src_node,
    const socket_handle& src_socket,
    const node_handle& dst_node,
    const socket_handle& dst_socket,
    const std::vector<node_handle>& src_interfaces,
    const std::vector<node_handle>& dst_interfaces)
    : m_src_node {src_node}
    , m_src_socket {src_socket}
    , m_dst_node {dst_node}
    , m_dst_socket {dst_socket}
    , m_src_interfaces {src_interfaces}
    , m_dst_interfaces {dst_interfaces}
  {
  }

  auto connection_info::src_node() const -> const node_handle&
  {
    return m_src_node;
  }

  auto connection_info::src_socket() const -> const socket_handle&
  {
    return m_src_socket;
  }

  auto connection_info::dst_node() const -> const node_handle&
  {
    return m_dst_node;
  }

  auto connection_info::dst_socket() const -> const socket_handle&
  {
    return m_dst_socket;
  }

  auto connection_info::src_interfaces() const
    -> const std::vector<node_handle>&
  {
    return m_src_interfaces;
  }

  auto connection_info::dst_interfaces() const
    -> const std::vector<node_handle>&
  {
    return m_dst_interfaces;
  }

} // namespace yave