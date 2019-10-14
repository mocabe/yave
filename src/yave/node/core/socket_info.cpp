//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/core/socket_info.hpp>

namespace yave {

  socket_info::socket_info(std::string name, socket_type type, node_handle node)
    : m_name {std::move(name)}
    , m_type {std::move(type)}
    , m_node {std::move(node)}
  {
  }

  auto socket_info::name() const -> const std::string&
  {
    return m_name;
  }

  auto socket_info::type() const -> socket_type
  {
    return m_type;
  }

  auto socket_info::node() const -> const node_handle&
  {
    return m_node;
  }

} // namespace yave