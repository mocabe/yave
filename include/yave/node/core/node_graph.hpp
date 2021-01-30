//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/graph_definition.hpp>
#include <yave/node/core/node_handle.hpp>
#include <yave/node/core/socket_handle.hpp>
#include <yave/node/core/connection_handle.hpp>
#include <yave/node/core/node_info.hpp>
#include <yave/node/core/socket_info.hpp>
#include <yave/node/core/connection_info.hpp>

#include <optional>
#include <memory>

namespace yave {

  /// Node graph.
  class node_graph
  {
    /// data
    graph_t g;

    // helpers
    node_graph(graph_t&&) noexcept;
    bool _find_loop(const node_handle&) const;

  public:
    /// Constructor
    node_graph() = default;

    /// Destructor
    ~node_graph() noexcept = default;

    /// Copy
    node_graph(const node_graph&) = delete;

    /// Move constructor
    node_graph(node_graph&&) noexcept = default;

    /// Copy assignment
    node_graph& operator=(const node_graph& other) = delete;

    /// Move assignment
    node_graph& operator=(node_graph&& other) noexcept = default;

    /// exists
    [[nodiscard]] bool exists(const node_handle& node) const;

    /// exists
    [[nodiscard]] bool exists(const socket_handle& socket) const;

    /// exists
    [[nodiscard]] bool exists(const connection_handle& connection) const;

    /// Get node info from handle.
    /// \returns std::nullopt when the node did not exist.
    [[nodiscard]] auto get_info(const node_handle& node) const
      -> std::optional<node_info>;

    /// Get socket info from handle.
    /// \returns std::nullopt when the socket did not exist.
    [[nodiscard]] auto get_info(const socket_handle& node) const
      -> std::optional<socket_info>;

    /// Get connection info of all connections from/to the node.
    /// \returns std::nullopt when connection doesn't exist.
    [[nodiscard]] auto get_info(const connection_handle& node) const
      -> std::optional<connection_info>;

    /// Get node name from handle.
    /// \returns std::nullopt when the node did not exist.
    [[nodiscard]] auto get_name(const node_handle& node) const
      -> std::optional<std::string>;

    /// Get node name for handle.
    /// \returns std::nullopt when the socket did not exist.
    [[nodiscard]] auto get_name(const socket_handle& socket) const
      -> std::optional<std::string>;

    /// Set node name
    void set_name(const node_handle& node, const std::string& name);

    /// Set socket name
    void set_name(const socket_handle& node, const std::string& name);

    /// Add new node.
    /// \returns handle of new node. may return null handle on error.
    /// \throws std::bad_alloc, std::runtime_error and other exceptions which
    /// can be thrown from edge_property constructors.
    [[nodiscard]] auto add(
      const std::string& name,
      const std::vector<std::string>& input_sockets,
      const std::vector<std::string>& output_sockets,
      const node_type& type,
      const uid& id = uid::random_generate()) -> node_handle;

    /// Remove node from graph.
    /// Destroy the node, all sockets connected to the node, and all edges
    /// connected to these sockets.
    /// \throws should not throw exception.
    void remove(const node_handle& node);

    /// Attach interface socket.
    /// When attached socket is destroyed, it will also be removed from
    /// interface node.
    [[nodiscard]] bool attach_interface(
      const node_handle& interface,
      const socket_handle& socket);

    /// Detach interface socket.
    void detach_interface(
      const node_handle& interface,
      const socket_handle& socket);

    /// Connect sockets.
    /// When connection already exists, return handle of existing connection.
    /// If the connection to establish is invalid (loop, etc.), return
    /// null handle.
    /// \returns Handle of connection (can be null handle).
    /// \throws std::bad_alloc
    [[nodiscard]] auto connect(
      const socket_handle& src_socket,
      const socket_handle& dst_socket,
      const uid& id = uid::random_generate()) -> connection_handle;

    /// Disconnect sockets.
    void disconnect(const connection_handle& connection);

    /// Find node from ID.
    /// \param id id
    /// \returns Null handle when not found
    [[nodiscard]] auto node(const uid& id) const -> node_handle;

    /// Get node attached to the socket
    [[nodiscard]] auto node(const socket_handle& socket) const -> node_handle;

    /// Get all nodes.
    [[nodiscard]] auto nodes() const -> std::vector<node_handle>;

    /// Get nodes
    [[nodiscard]] auto nodes(const std::string& name) const
      -> std::vector<node_handle>;

    /// Get list of interfaces nodes attached to the socket
    [[nodiscard]] auto interfaces(const socket_handle& socket) const
      -> std::vector<node_handle>;

    /// Find socket from ID.
    /// \param id id
    /// \returns Null handle when not found.
    [[nodiscard]] auto socket(const uid& id) const -> socket_handle;

    /// Get all sockets.
    [[nodiscard]] auto sockets() const -> std::vector<socket_handle>;

    /// Get sockets
    [[nodiscard]] auto sockets(const node_handle& node) const
      -> std::vector<socket_handle>;

    /// Get sockets
    /// \param type type of sockets to get handles
    [[nodiscard]] auto sockets(const node_handle& node, socket_type type) const
      -> std::vector<socket_handle>;

    /// Find connection handle from ID.
    /// \param id id
    /// \returns Null handle when not found
    [[nodiscard]] auto connection(const uid& id) const -> connection_handle;

    /// Get all connections.
    [[nodiscard]] auto connections() const -> std::vector<connection_handle>;

    /// Get connections
    [[nodiscard]] auto connections(const node_handle& node) const
      -> std::vector<connection_handle>;

    /// Get connections
    [[nodiscard]] auto connections(const socket_handle& socket) const
      -> std::vector<connection_handle>;

    /// Get connections
    /// \param type type of sockets to get connections
    [[nodiscard]] auto connections(const node_handle& socket, socket_type type)
      const -> std::vector<connection_handle>;

    /// Get socket type
    [[nodiscard]] auto type(const socket_handle& h) const
      -> std::optional<socket_type>;

    /// Get node type
    [[nodiscard]] auto type(const node_handle& h) const
      -> std::optional<node_type>;

    /// Check socket type.
    /// \returns false when the socket does not exist.
    [[nodiscard]] bool has_type(const socket_handle& socket, socket_type type)
      const;

    /// Check node type.
    /// \returns false when the node does not exist.
    [[nodiscard]] bool has_type(const node_handle& socket, node_type type)
      const;

    /// Check if connection exists.
    [[nodiscard]] bool has_connection(const socket_handle& socket) const;

    /// Has data?
    [[nodiscard]] bool has_data(const socket_handle& h) const;

    /// Has data?
    [[nodiscard]] bool has_data(const node_handle& h) const;

    /// Get custom data object
    /// \returns nullptr when no data set
    [[nodiscard]] auto get_data(const socket_handle& h) const
      -> object_ptr<Object>;

    /// Get custom data object
    /// \returns nullptr when no data set
    [[nodiscard]] auto get_data(const node_handle& h) const
      -> object_ptr<Object>;

    /// Set custom data
    void set_data(const socket_handle& h, object_ptr<Object> data);

    /// Set custom data
    void set_data(const node_handle& h, object_ptr<Object> data);

    /// Get list of root nodes.
    [[nodiscard]] auto roots() const -> std::vector<node_handle>;

    /// Get root of ancestor tree of specific node.
    [[nodiscard]] auto root_of(const node_handle& node) const
      -> std::vector<node_handle>;

    /// clear graph
    void clear();

    /// empty?
    [[nodiscard]] bool empty() const;

    /// clone node_graph.
    /// \note cloning node does not change IDs of elements, but invalidates
    /// descriptor handles since it changes address of elements.
    [[nodiscard]] auto clone() const -> node_graph;
  };

} // namespace yave
