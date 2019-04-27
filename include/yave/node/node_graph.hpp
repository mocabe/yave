//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/graph_definition.hpp>
#include <yave/node/node_handle.hpp>
#include <yave/node/connection_handle.hpp>
#include <yave/node/node_info.hpp>
#include <yave/node/connection_info.hpp>

#include <optional>
#include <mutex>

namespace yave {

  /// Node graph.
  class NodeGraph
  {
  public:
    /// Constructor
    NodeGraph();

    /// Copy
    NodeGraph(const NodeGraph&);

    /// Move constructor
    NodeGraph(NodeGraph&&);

    /// Destructor
    ~NodeGraph();

    /// exists
    [[nodiscard]] bool exists(const NodeHandle& node) const;

    /// exists
    [[nodiscard]] bool exists(const ConnectionHandle& connection) const;

    /// Add new node.
    /// \returns Non-null handle of new node.
    /// \throws std::bad_alloc, std::runtime_error and other exceptions which
    /// can be thrown from NodeProperty constructors.
    [[nodiscard]] NodeHandle
      add_node(const NodeInfo& info, const primitive_t& prim = {});

    /// Remove node from graph.
    /// Destroy the node, all sockets connected to the node, and all edges
    /// connected to these sockets.
    /// \throws should not throw exception.
    void remove_node(const NodeHandle& node);

    /// Connect sockets.
    /// When connection already exists, return handle of existing connection.
    /// If the connection to establish is invalid (loop, etc.), return
    /// null handle.
    /// \returns Handle of connection (can be null handle).
    /// \throws std::bad_alloc
    [[nodiscard]] ConnectionHandle connect(const ConnectionInfo& info);

    /// Connect sockets.
    /// See `connect(const ConnectionInfo&)`.
    [[nodiscard]] ConnectionHandle connect(
      const NodeHandle& src_node,
      const std::string& src_socket,
      const NodeHandle& dst_node,
      const std::string& dst_socket);

    /// Disconnect sockets.
    /// \throws should not throw exception.
    void disconnect(const ConnectionHandle& info);

    /// Find nodes from name.
    /// \returns Empty vector when not found.
    [[nodiscard]] std::vector<NodeHandle> nodes(const std::string& name) const;

    /// Get list of nodes.
    [[nodiscard]] std::vector<NodeHandle> nodes() const;

    /// Get node info from handle.
    /// \returns std::nullopt when the node did not exist.
    [[nodiscard]] std::optional<NodeInfo>
      node_info(const NodeHandle& node) const;

    /// Get node name from handle.
    /// \returns std::nullopt when the node did not exist.
    [[nodiscard]] std::optional<std::string>
      node_name(const NodeHandle& node) const;

    /// Check if the node has a socket with the name.
    [[nodiscard]] bool
      has_socket(const NodeHandle& node, const std::string& socket) const;

    /// Check if socket type is input.
    /// \returns false when the socket does not exist.
    [[nodiscard]] bool
      is_input_socket(const NodeHandle& node, const std::string& socket) const;

    /// Check if socket type is output.
    /// \returns false when the socket does not exist.
    [[nodiscard]] bool
      is_output_socket(const NodeHandle& node, const std::string& socket) const;

    /// Find input connection to the node.
    [[nodiscard]] std::vector<ConnectionHandle> input_connections(
      const NodeHandle& node,
      const std::string& socket) const;

    /// Find output connection from the node.
    [[nodiscard]] std::vector<ConnectionHandle> output_connections(
      const NodeHandle& node,
      const std::string& socket) const;

    /// Get list of input connections to the node.
    [[nodiscard]] std::vector<ConnectionHandle>
      input_connections(const NodeHandle& node) const;

    /// Get list of output connections from the node.
    [[nodiscard]] std::vector<ConnectionHandle>
      output_connections(const NodeHandle& node) const;

    /// Get all connections.
    [[nodiscard]] std::vector<ConnectionHandle> connections() const;

    /// Get list of all connections from/to the node.
    [[nodiscard]] std::vector<ConnectionHandle>
      connections(const NodeHandle& node) const;

    /// Get list of input/output connections from specific socket of the node.
    [[nodiscard]] std::vector<ConnectionHandle>
      connections(const NodeHandle& node, const std::string& socket) const;

    /// Check if connection exists.
    [[nodiscard]] bool
      has_connection(const NodeHandle& node, const std::string& socket) const;

    /// Get connection info of all connections from/to the node.
    [[nodiscard]] std::optional<ConnectionInfo>
      connection_info(const ConnectionHandle& node) const;

    /// Get list of input sockets attached to the node.
    [[nodiscard]] std::vector<std::string>
      input_sockets(const NodeHandle& node) const;

    /// Get list of output sockets attached to the node.
    [[nodiscard]] std::vector<std::string>
      output_sockets(const NodeHandle& node) const;

    /// Primitive node?
    [[nodiscard]] bool is_primitive(const NodeHandle& node) const;

    /// Get primitive value.
    /// \returns std::nullopt when `node` is not primitive node.
    [[nodiscard]] std::optional<primitive_t>
      get_primitive(const NodeHandle& node) const;

    /// Set primitive value.
    /// When `is_primitive(prim_node) == false`, no effect.
    void set_primitive(const NodeHandle& prim_node, const primitive_t& prim);

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
} // namespace yave