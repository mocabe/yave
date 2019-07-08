//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/scene/layered_node_graph.hpp>

namespace yave {

  layer_info::layer_info(
    const std::string& name,
    const layer_handle& parent,
    const std::vector<layer_handle>& sublayers,
    const std::vector<layer_resource>& resources)
    : m_name {name}
    , m_parent {parent}
    , m_sublayers {sublayers}
    , m_resources {resources}
  {
  }

  auto layer_info::name() const -> std::string
  {
    return m_name;
  }

  auto layer_info::parent() const -> layer_handle
  {
    return m_parent;
  }

  auto layer_info::sublayers() const -> std::vector<layer_handle>
  {
    return m_sublayers;
  }

  auto layer_info::resources() const -> std::vector<layer_resource>
  {
    return m_resources;
  }

} // namespace yave