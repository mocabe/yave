//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core.hpp>
#include <yave/node/parser.hpp>
#include <yave/node/compiler.hpp>

#include <memory>

namespace yave {

  // clang-format off

  class node_tree
  {
  public:
    /// Construct empty node tree.
    node_tree();
    /// Copy constructor (deleted).
    node_tree(const node_tree&) = delete;
    /// Move constructor.
    node_tree(node_tree&&);

    /// Copy assignment (deleted).
    node_tree& operator=(const node_tree&) = delete;
    /// Move assignment.
    node_tree& operator=(node_tree&&);

    /// register new node info
    [[nodiscard]]
    bool register_node_info(const node_info& info);
    /// unregister node info
    void unregister_node_info(const node_info& info);
    /// register new bind info
    [[nodiscard]]
    bool register_bind_info(const bind_info& info);
    /// register bind info
    void unregister_bind_info(const bind_info& info);

    /// exist?
    [[nodiscard]]
    bool exists(const node_handle& node) const; [[nodiscard]]
    /// exist?
    [[nodiscard]]
    bool exists(const connection_handle& connection) const;

    /// create new node
    [[nodiscard]]
    node_handle create(const std::string& name);
    /// destroy node
    void destroy(const node_handle& handle);

    /// connect sockets
    [[nodiscard]]
    connection_handle connect(
      const node_handle& src_n, const std::string& src_s,
      const node_handle& dst_n, const std::string& dst_s);
    /// disconnect sockets
    void disconnect(const connection_handle& handle);

    /// list nodes
    [[nodiscard]]
    std::vector<node_handle> nodes() const;

    /// list connections
    [[nodiscard]]
    std::vector<connection_handle> connections() const;
    /// list connections
    [[nodiscard]]
    std::vector<connection_handle> connections(const node_handle& node) const;
    /// list connections
    [[nodiscard]]
    std::vector<connection_handle> connections(const node_handle& node, const std::string& socket) const;

    /// list input connections
    [[nodiscard]]
    std::vector<connection_handle> input_connections() const;
    /// list input connections
    [[nodiscard]]
    std::vector<connection_handle> input_connections(const node_handle& node) const;
    /// list input connections
    [[nodiscard]]
    std::vector<connection_handle> input_connections(const node_handle& node, const std::string& socket) const;

    /// list input connections
    [[nodiscard]]
    std::vector<connection_handle> output_connections() const;
    /// list input connections
    [[nodiscard]]
    std::vector<connection_handle> output_connections(const node_handle& node) const;
    /// list input connections
    [[nodiscard]]
    std::vector<connection_handle> output_connections(const node_handle& node, const std::string& socket) const;

    /// get node info
    [[nodiscard]]
    std::optional<node_info> get_info(const node_handle& node) const;

    /// get connection info
    [[nodiscard]]
    std::optional<connection_info> get_info(const connection_handle& node) const;

    /// primitive?
    [[nodiscard]]
    std::optional<primitive_t> get_primitive(const node_handle& node) const;
    /// set primitive
    void set_primitive(const node_handle& node, const primitive_t& prim);

    /// Check
    void run_check();

    /// Get last errors
    [[nodiscard]]
    error_list get_errors();

    /// Get last warnings
    [[nodiscard]]
    error_list get_warnings();

    /// clear
    void clear();

  private:
    struct impl;
    std::unique_ptr<impl> m_pimpl;
  };

  // clang-format on
}