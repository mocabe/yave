//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/node_handle.hpp>

#include <functional>

namespace yave {

  // fwd
  class managed_node_graph;

  /// Initializer procedure of syntax sugar
  class node_initializer
  {
  public:
    /// Ctor
    node_initializer(
      const std::string& name,
      std::function<node_handle(managed_node_graph&, const node_handle&)> func);

    /// Copy ctor
    node_initializer(const node_initializer&) = default;

    /// Get name
    [[nodiscard]] auto name() const -> const std::string&;

    /// Initialize node
    [[nodiscard]] auto initialize(
      managed_node_graph& graph,
      const node_handle& group) const -> node_handle;

  private:
    const std::string m_name;
    const std::function<node_handle(managed_node_graph&, const node_handle&)>
      m_initializer_func;
  };

} // namespace yave