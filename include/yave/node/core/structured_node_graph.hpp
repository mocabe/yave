//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/node_graph.hpp>
#include <yave/node/core/structured_node_info.hpp>
#include <yave/node/core/structured_socket_info.hpp>
#include <yave/node/core/structured_connection_info.hpp>
#include <yave/node/core/node_declaration.hpp>

#include <glm/glm.hpp>

namespace yave {

  /// Node graph PoC implementation v3.
  /// This is the heart of our visual language, and probably one of the most
  /// complex data structure in the codebase. This class is built on top of two
  /// layers of infrastructure: `graph::graph` and `node_graph`.
  ///
  /// This data can be considered as an equivalent of AST in traditional
  /// programming languages, but there're fundamental differences:
  /// 1. User can access and modify node graph freely, and these operations will
  /// not cause any syntax errors like undefined identifiers. For example,
  /// deleting a function definition will also delete all of calls to it
  /// automatically.
  /// 2. Some part of node graph is event driven, it modifies itself depending
  /// on user actions.
  class structured_node_graph
  {
  public:
    /// regex pattern for valid node names
    static constexpr auto node_name_regex = R"(^[^\W\s]+$)";
    /// regex pattern for valid socket names
    static constexpr auto socket_name_regex = R"(^[\w\s]*$)";
    /// regex pattern for valid path
    static constexpr auto path_name_regex = R"(^(\w+)(\.\w+)*$)";
    /// regex pattern for valid path search pattern
    static constexpr auto path_search_regex = R"(^(|\.|(\w+)(\.\w+)*\.?)$)";

  public:
    /// construct empty node graph
    structured_node_graph();
    /// copy ctor (deleted, use clone())
    structured_node_graph(const structured_node_graph&) = delete;
    /// move ctor
    structured_node_graph(structured_node_graph&&) noexcept;
    /// dtor
    ~structured_node_graph() noexcept;
    /// copy assign (deleted, use clone())
    structured_node_graph& operator=(const structured_node_graph&) = delete;
    /// move assign
    structured_node_graph& operator=(structured_node_graph&&) noexcept;

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
      -> std::optional<structured_node_info>;
    /// get socket info
    [[nodiscard]] auto get_info(const socket_handle& socket) const
      -> std::optional<structured_socket_info>;
    /// get connection info
    [[nodiscard]] auto get_info(const connection_handle& connection) const
      -> std::optional<structured_connection_info>;

    /// get node name
    [[nodiscard]] auto get_name(const node_handle& node) const
      -> std::optional<std::string>;
    /// get socket name
    [[nodiscard]] auto get_name(const socket_handle& socket) const
      -> std::optional<std::string>;
    /// set name of node
    void set_name(const node_handle& node, const std::string& name);
    /// set name of socket
    void set_name(const socket_handle& socket, const std::string& name);

    /// get node pos
    [[nodiscard]] auto get_pos(const node_handle& node) const
      -> std::optional<glm::dvec2>;

    /// set node pos
    void set_pos(const node_handle& node, const glm::dvec2& newpos);

    /// get socket data
    [[nodiscard]] auto get_data(const socket_handle& socke) const
      -> object_ptr<Object>;
    /// set socket data
    void set_data(const socket_handle& socket, object_ptr<Object> data);

    /// get socket index
    [[nodiscard]] auto get_index(const socket_handle& socket) const
      -> std::optional<size_t>;

  public:
    /// get path of the node
    /// \note this function can return multiple identical paths for different
    /// node handles.
    [[nodiscard]] auto get_path(const node_handle& node) const
      -> std::optional<std::string>;

    /// search definitions from path
    /// \returns definition node found from argument. if the path ends with '/'
    /// and the node specified in the path is a group, returns list of
    /// definitions in the group.
    [[nodiscard]] auto search_path(const std::string& path) const
      -> std::vector<node_handle>;

  public:
    /// check relation
    [[nodiscard]] bool is_parent_of(
      const node_handle& parent,
      const node_handle& child) const;

    /// check relation
    [[nodiscard]] bool is_child_of(
      const node_handle& child,
      const node_handle& parent) const;

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
    /// Create new declaration
    /// \param decl new function declaration
    /// \note decl should have unique name, otherwise will fail
    /// \returns null handle when failed
    [[nodiscard]] auto create_declaration(
      const std::shared_ptr<node_declaration>& decl) -> node_handle;

    /// Create new group
    /// \param parent_group parent group. null handle with it's global
    /// \param nodes selected nodes to be moved to new group.
    /// \note all nodes should be under parent group.
    [[nodiscard]] auto create_group(
      const node_handle& parent_group,
      const std::vector<node_handle>& nodes,
      const uid& id = uid::random_generate()) -> node_handle;

    /// create new node by shallow copying node call
    /// \param parent parent group
    /// \param source source node call to create
    [[nodiscard]] auto create_copy(
      const node_handle& parent,
      const node_handle& source,
      const uid& id = uid::random_generate()) -> node_handle;

    /// create new node by deep copying node call
    /// \param parent parent group
    /// \param source source node call to create
    /// \note when source is builtin function call, effect is same to
    /// `create_copy`.
    /// \note name of new group will be different when parent group already has
    /// definition node with same name.
    [[nodiscard]] auto create_clone(
      const node_handle& parent,
      const node_handle& source,
      const uid& id = uid::random_generate()) -> node_handle;

    /// destroy node
    /// \note if `node` is group interface, remove all nodes in the node too.
    /// \note i/o handler nodes cannot be destroyed.
    void destroy(const node_handle& node);

  public:
    /// connect sockets
    [[nodiscard]] auto connect(
      const socket_handle& src_socket,
      const socket_handle& dst_socket,
      const uid& id = uid::random_generate()) -> connection_handle;

    /// disconnect sockets
    void disconnect(const connection_handle& handle);

  public:
    /// Definition of function or group?
    [[nodiscard]] bool is_definition(const node_handle& node) const;

    /// Get definition point of node
    [[nodiscard]] auto get_definition(const node_handle& node) const
      -> node_handle;

    /// Call of function or group?
    [[nodiscard]] bool is_call(const node_handle& node) const;

    /// Get (non-definition) calls
    [[nodiscard]] auto get_calls(const node_handle& node) const
      -> std::vector<node_handle>;

  public:
    /// Function?
    [[nodiscard]] bool is_function(const node_handle& node) const;

    /// Macro?
    [[nodiscard]] bool is_macro(const node_handle& node) const;

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

  public:
    /// bring node to from in group member
    void bring_front(const node_handle& node);
    /// bring node to back in group member
    void bring_back(const node_handle& node);

  public:
    /// clone
    [[nodiscard]] auto clone() const -> structured_node_graph;

  public:
    /// clear
    void clear();

  private:
    class impl;
    std::unique_ptr<impl> m_pimpl;
    structured_node_graph(std::unique_ptr<impl>&&);
  };
} // namespace yave
