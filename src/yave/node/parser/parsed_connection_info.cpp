//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/parser/parsed_connection_info.hpp>

namespace yave {

  parsed_connection_info::parsed_connection_info(
    const parsed_node_handle& src_node,
    const parsed_node_handle& dst_node,
    const std::string& dst_socket)
    : m_src_node {src_node}
    , m_dst_node {dst_node}
    , m_dst_socket {dst_socket}
  {
  }

  const parsed_node_handle& parsed_connection_info::src_node() const
  {
    return m_src_node;
  }

  const parsed_node_handle& parsed_connection_info::dst_node() const
  {
    return m_dst_node;
  }

  const std::string& parsed_connection_info::dst_socket() const
  {
    return m_dst_socket;
  }
}