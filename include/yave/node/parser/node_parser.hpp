//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/node_graph.hpp>
#include <yave/node/support/bind_info_manager.hpp>
#include <yave/node/parser/errors.hpp>
#include <yave/node/parser/parsed_node_graph.hpp>
#include <yave/support/error.hpp>

namespace yave {

  class socket_instance_manager;

  /// node_graph parser
  class node_parser
  {
  public:
    /// Ctor
    node_parser(const node_graph& node, const bind_info_manager& binds);

    /// Parse isoated tree.
    /// \returns true on success.
    std::pair<bool, error_list> parse_isolated_tree(
      const node_handle& node,
      const std::string& socket) const;

    /// Type check check isoated tree.
    /// \requires Tree is already parsed by parse_isolated_tree() successfully.
    std::pair<object_ptr<const Type>, error_list> type_isolated_tree(
      const node_handle& node,
      const std::string& socket) const;

    /// Parse prime tree.
    /// \returns true on success.
    std::pair<bool, error_list> parse_prime_tree(
      const node_handle& node,
      const std::string& socket) const;

    /// Parse prime tree.
    /// \required Tree is already parsed by parse_prime_tree() successfully.
    std::pair<std::optional<parsed_node_graph>, error_list> type_prime_tree(
      const node_handle& node,
      const std::string& socket,
      socket_instance_manager& sim) const;

    /// Lock.
    /// \notes: Also need to lock node_graph and bind_info_manager.
    std::unique_lock<std::mutex> lock() const;

  private:
    /// reference to node graph
    const node_graph& m_graph;
    /// reference to bind list
    const bind_info_manager& m_binds;

  private:
    /// mutex
    mutable std::mutex m_mtx;
  };

} // namespace yave