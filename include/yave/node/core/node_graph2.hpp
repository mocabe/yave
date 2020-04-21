//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/node_graph.hpp>
#include <yave/node/core/node_info2.hpp>
#include <yave/node/core/socket_info2.hpp>
#include <yave/node/core/connection_info2.hpp>
#include <yave/node/core/node_declaration.hpp>

#include <yave/lib/vec/vec.hpp>

namespace yave {

  /// NodeGraph v2 PoC implementation
  class node_graph2
  {
  public:
    node_graph2();
    ~node_graph2() noexcept;

  public:
    /// exist?
    [[nodiscard]] bool exists(const node_handle& node) const;
    /// exist?
    [[nodiscard]] bool exists(const connection_handle& connection) const;
    /// exists?
    [[nodiscard]] bool exists(const socket_handle& socket) const;

    /// Find node fron ID
    [[nodiscard]] auto node(const uid& id) const -> node_handle;
    /// Find socket from ID
    [[nodiscard]] auto socket(const uid& id) const -> socket_handle;
    /// Find connection from ID
    [[nodiscard]] auto connection(const uid& id) const -> connection_handle;

    /// get node info
    [[nodiscard]] auto get_info(const node_handle& node) const
      -> std::optional<node_info2>;
    /// get socket info
    [[nodiscard]] auto get_info(const socket_handle& socket) const
      -> std::optional<socket_info2>;
    /// get connection info
    [[nodiscard]] auto get_info(const connection_handle& connection) const
      -> std::optional<connection_info2>;

    /// get node name
    [[nodiscard]] auto get_name(const node_handle& node) const
      -> std::optional<std::string>;
    /// get socket name
    [[nodiscard]] auto get_name(const socket_handle& socket) const
      -> std::optional<std::string>;

    /// get node pos
    [[nodiscard]] auto get_pos(const node_handle& node) const
      -> std::optional<fvec2>;
    /// set node pos
    void set_pos(const node_handle& node, const fvec2& newpos);

    /// get socket data
    [[nodiscard]] auto get_data(const socket_handle& socke) const
      -> object_ptr<Object>;
    /// set socket data
    void set_data(const socket_handle& socket, object_ptr<Object> data);

  public:
    /// Get input sockets
    [[nodiscard]] auto input_sockets(const node_handle& node) const
      -> std::vector<socket_handle>;
    /// Get output sockets
    [[nodiscard]] auto output_sockets(const node_handle& node) const
      -> std::vector<socket_handle>;

    /// Get input connections
    [[nodiscard]] auto input_connections(const node_handle& node) const
      -> std::vector<connection_handle>;
    /// Get output connections
    [[nodiscard]] auto output_connections(const node_handle& node) const
      -> std::vector<connection_handle>;

    /// Get parent node of socket
    [[nodiscard]] auto node(const socket_handle& socket) const -> node_handle;

    /// Get connections
    [[nodiscard]] auto connections(const socket_handle& socket) const
      -> std::vector<connection_handle>;

  public:
    /// Create new builtin function
    /// \param decl new function declaration
    /// \note decl should have unique name, otherwise will fail
    /// \returns null handle when failed
    [[nodiscard]] auto create_function(const node_declaration& decl)
      -> node_handle;

    /// Create new group
    /// \param parent_group parent group. null handle with it's global
    /// \param nodes selected nodes to be moved to new group.
    /// \note all nodes should be under parent group.
    [[nodiscard]] auto create_group(
      const node_handle& parent_group,
      const std::vector<node_handle>& nodes = {}) -> node_handle;

    /// create new node by shallow copying node call
    /// \param parent parent group
    /// \param source source node call to create
    [[nodiscard]] auto create_copy(
      const node_handle& parent,
      const node_handle& source) -> node_handle;

    /// create new node by deep copying node call
    /// \param parent parent group
    /// \param source source node call to create
    /// \note when source is builtin function call, effect is same to
    /// `create_copy`.
    [[nodiscard]] auto create_clone(
      const node_handle& parent,
      const node_handle& source) -> node_handle;

    /// destroy node
    /// \note if `node` is group interface, remove all nodes in the node too.
    /// \note i/o handler nodes cannot be destroyed.
    void destroy(const node_handle& node);

  public:
    /// connect sockets
    [[nodiscard]] auto connect(
      const socket_handle& src_socket,
      const socket_handle& dst_socket) -> connection_handle;

    /// disconnect sockets
    void disconnect(const connection_handle& handle);

  public:
    /// Definition of function or group?
    [[nodiscard]] bool is_definition(const node_handle& node) const;

    /// Get definition point of node
    [[nodiscard]] auto get_definition(const node_handle& node) const
      -> node_handle;

  public:
    /// Function?
    [[nodiscard]] bool is_function(const node_handle& node) const;

    /// Group?
    [[nodiscard]] bool is_group(const node_handle& node) const;

  public:
    /// Group member?
    [[nodiscard]] bool is_group_member(const node_handle& node) const;

    /// Group output node?
    [[nodiscard]] bool is_group_output(const node_handle& node) const;

    /// Group input node?
    [[nodiscard]] bool is_group_input(const node_handle& node) const;

    /// Get all nodes in group
    [[nodiscard]] auto get_group_members(const node_handle& node) const
      -> std::vector<node_handle>;

    /// Get group output node
    [[nodiscard]] auto get_group_input(const node_handle& node) const
      -> node_handle;

    /// Get group input node
    [[nodiscard]] auto get_group_output(const node_handle& node) const
      -> node_handle;

    /// Get group nodes
    /// \returns group members + IO handlers
    [[nodiscard]] auto get_group_nodes(const node_handle& group) const
      -> std::vector<node_handle>;

    /// Get parent group
    [[nodiscard]] auto get_parent_group(const node_handle& node) const
      -> node_handle;

    /// Add group input socket
    /// \note index -1 treated as size of sockets
    /// \requires -1 <= index <= number of sockets
    [[nodiscard]] auto add_input_socket(
      const node_handle& node,
      const std::string& socket,
      size_t index = -1) -> socket_handle;

    /// Add group output socket
    /// \note index -1 treated as size of sockets
    /// \requires -1 <= index <= number of sockets
    [[nodiscard]] auto add_output_socket(
      const node_handle& node,
      const std::string& socket,
      size_t index = -1) -> socket_handle;

    /// Remove group output socket
    /// \note destroy all connections to the socket
    void remove_socket(const socket_handle& socket);

    /// Change name of group
    void set_group_name(const node_handle& group, const std::string& name);

    /// Change name of socket
    void set_socket_name(const socket_handle& socket, const std::string& name);

  public:
    /// bring node to from in group member
    void bring_front(const node_handle& node);
    /// bring node to back in group member
    void bring_back(const node_handle& node);

  public:
    /// clone
    [[nodiscard]] auto clone() -> node_graph2;

  public:
    /// clear
    void clear();

  private:
    class impl;
    std::unique_ptr<impl> m_pimpl;
    node_graph2(std::unique_ptr<impl>&&);
  };
} // namespace yave
