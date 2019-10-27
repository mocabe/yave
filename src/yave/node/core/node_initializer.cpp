//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/core/node_initializer.hpp>

namespace yave {

  node_initializer::node_initializer(
    const std::string& name,
    std::function<node_handle(managed_node_graph&, const node_handle&)> func)
    : m_name {name}
    , m_initializer_func {std::move(func)}
  {
  }

  auto node_initializer::name() const -> const std::string&
  {
    return m_name;
  }

  auto node_initializer::initialize(
    managed_node_graph& graph,
    const node_handle& group) const -> node_handle
  {
    return m_initializer_func(graph, group);
  }
} // namespace yave