//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/core/basic_socket_info.hpp>

namespace yave {

  basic_socket_info::basic_socket_info(
    std::string name,
    basic_socket_type type,
    node_handle node,
    std::vector<node_handle> interfaces)
    : m_name {std::move(name)}
    , m_type {std::move(type)}
    , m_node {std::move(node)}
    , m_interfaces {std::move(interfaces)}
  {
  }

  auto basic_socket_info::name() const -> const std::string&
  {
    return m_name;
  }

  auto basic_socket_info::type() const -> basic_socket_type
  {
    return m_type;
  }

  auto basic_socket_info::node() const -> const node_handle&
  {
    return m_node;
  }

  auto basic_socket_info::interfaces() const -> const std::vector<node_handle>&
  {
    return m_interfaces;
  }

} // namespace yave