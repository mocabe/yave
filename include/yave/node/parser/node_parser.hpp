//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core.hpp>
#include <yave/node/support.hpp>
#include <yave/support/error.hpp>

#include <yave/node/parser/errors.hpp>

namespace yave {

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
    std::pair<object_ptr<const Type>, error_list>
      type_prime_tree(const node_handle& node, const std::string& socket) const;

  private:
    /// reference to node graph
    const node_graph& m_graph;
    /// reference to bind list
    const bind_info_manager& m_binds;
  };

} // namespace yave