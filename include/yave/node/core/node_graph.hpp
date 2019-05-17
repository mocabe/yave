//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/graph_definition.hpp>
#include <yave/node/core/node_handle.hpp>
#include <yave/node/core/connection_handle.hpp>
#include <yave/node/core/node_info.hpp>
#include <yave/node/core/connection_info.hpp>

#include <optional>
#include <mutex>

namespace yave {

  /// Node graph.
  class node_graph
  {
  public:
    /// Constructor
    node_graph();

    /// Copy
    node_graph(const node_graph&);

    /// Move constructor
    node_graph(node_graph&&);

    /// Destructor
    ~node_graph();

    /// exists
    [[nodiscard]] bool exists(const node_handle& node) const;

    /// exists
    [[nodiscard]] bool exists(const connection_handle& connection) const;

    /// Add new node.
    /// \returns Non-null handle of new node.
    /// \throws std::bad_alloc, std::runtime_error and other exceptions which
    /// can be thrown from edge_property constructors.
    [[nodiscard]] node_handle
      add(const node_info& info, const primitive_t& prim = {});

    /// Remove node from graph.
    /// Destroy the node, all sockets connected to the node, and all edges
    /// connected to these sockets.
    /// \throws should not throw exception.
    void remove(const node_handle& node);

    /// Connect sockets.
    /// When connection already exists, return handle of existing connection.
    /// If the connection to establish is invalid (loop, etc.), return
    /// null handle.
    /// \returns Handle of connection (can be null handle).
    /// \throws std::bad_alloc
    [[nodiscard]] connection_handle connect(const connection_info& info);

    /// Connect sockets.
    /// See `connect(const connection_info&)`.
    [[nodiscard]] connection_handle connect(
      const node_handle& src_node,
      const std::string& src_socket,
      const node_handle& dst_node,
      const std::string& dst_socket);

    /// Disconnect sockets.
    /// \throws should not throw exception.
    void disconnect(const connection_handle& info);

    /// Find nodes from name.
    /// \returns Empty vector when not found.
    [[nodiscard]] std::vector<node_handle> nodes(const std::string& name) const;

    /// Get list of nodes.
    [[nodiscard]] std::vector<node_handle> nodes() const;

    /// Get node info from handle.
    /// \returns std::nullopt when the node did not exist.
    [[nodiscard]] std::optional<node_info>
      get_info(const node_handle& node) const;

    /// Get node name from handle.
    /// \returns std::nullopt when the node did not exist.
    [[nodiscard]] std::optional<std::string>
      get_name(const node_handle& node) const;

    /// Check if the node has a socket with the name.
    [[nodiscard]] bool
      has_socket(const node_handle& node, const std::string& socket) const;

    /// Check if socket type is input.
    /// \returns false when the socket does not exist.
    [[nodiscard]] bool
      is_input_socket(const node_handle& node, const std::string& socket) const;

    /// Check if socket type is output.
    /// \returns false when the socket does not exist.
    [[nodiscard]] bool is_output_socket(
      const node_handle& node,
      const std::string& socket) const;

    /// Find input connection to the node.
    [[nodiscard]] std::vector<connection_handle> input_connections(
      const node_handle& node,
      const std::string& socket) const;

    /// Find output connection from the node.
    [[nodiscard]] std::vector<connection_handle> output_connections(
      const node_handle& node,
      const std::string& socket) const;

    /// Get list of input connections to the node.
    [[nodiscard]] std::vector<connection_handle>
      input_connections(const node_handle& node) const;

    /// Get list of output connections from the node.
    [[nodiscard]] std::vector<connection_handle>
      output_connections(const node_handle& node) const;

    /// Get all connections.
    [[nodiscard]] std::vector<connection_handle> connections() const;

    /// Get list of all connections from/to the node.
    [[nodiscard]] std::vector<connection_handle>
      connections(const node_handle& node) const;

    /// Get list of input/output connections from specific socket of the node.
    [[nodiscard]] std::vector<connection_handle>
      connections(const node_handle& node, const std::string& socket) const;

    /// Check if connection exists.
    [[nodiscard]] bool
      has_connection(const node_handle& node, const std::string& socket) const;

    /// Get connection info of all connections from/to the node.
    [[nodiscard]] std::optional<connection_info>
      get_info(const connection_handle& node) const;

    /// Get list of input sockets attached to the node.
    [[nodiscard]] std::vector<std::string>
      input_sockets(const node_handle& node) const;

    /// Get list of output sockets attached to the node.
    [[nodiscard]] std::vector<std::string>
      output_sockets(const node_handle& node) const;

    /// Primitive node?
    [[nodiscard]] bool is_primitive(const node_handle& node) const;

    /// Get primitive value.
    /// \returns std::nullopt when `node` is not primitive node.
    [[nodiscard]] std::optional<primitive_t>
      get_primitive(const node_handle& node) const;

    /// Set primitive value.
    /// When `is_primitive(prim_node) == false`, no effect.
    void set_primitive(const node_handle& prim_node, const primitive_t& prim);

    /// Get list of root nodes.
    [[nodiscard]] std::vector<node_handle> roots() const;

    /// Get root of ancestor tree of specific node.
    [[nodiscard]] std::vector<node_handle>
      root_of(const node_handle& node) const;

    /// Lock
    [[nodiscard]] std::unique_lock<std::mutex> lock() const;

    /// clear graph
    void clear();

  private:
    /// graph
    graph_t m_g;
    /// mutex
    mutable std::mutex m_mtx;
  };

  struct NodesDiff
  {
    /// not changed
    std::vector<node_handle> not_changed;
    /// removed
    std::vector<node_handle> removed;
    /// added
    std::vector<node_handle> added;
  };

  struct ConnectionsDiff
  {
    /// not changed
    std::vector<connection_handle> not_changed;
    /// removed
    std::vector<connection_handle> removed;
    /// added
    std::vector<connection_handle> added;
  };

  /// Calculate diff
  [[nodiscard]] NodesDiff nodes_diff(
    const std::vector<node_handle>& prev_nodes,
    const std::vector<node_handle>& nodes);

  /// Calculate diff
  [[nodiscard]] ConnectionsDiff connections_diff(
    const std::vector<connection_handle>& prev_connections,
    const std::vector<connection_handle>& connections);

} // namespace yave