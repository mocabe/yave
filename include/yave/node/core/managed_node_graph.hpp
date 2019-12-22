//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/node_graph.hpp>
#include <yave/node/core/shared_node_handle.hpp>
#include <yave/node/core/managed_node_info.hpp>
#include <yave/node/core/managed_socket_info.hpp>
#include <yave/node/core/managed_connection_info.hpp>
#include <yave/node/core/node_declaration_store.hpp>

namespace yave {

  /// Warpper interface for managing node info registration and grouping.
  class managed_node_graph
  {
  public:
    // Construct empty node tree.
    managed_node_graph();
    /// Copy constructor (deleted).
    managed_node_graph(const managed_node_graph&) = delete;
    /// Move constructor.
    managed_node_graph(managed_node_graph&&) noexcept;
    /// Destructor.
    ~managed_node_graph() noexcept;
    /// Copy assignment (deleted).
    managed_node_graph& operator=(const managed_node_graph&) = delete;
    /// Move assignment.
    managed_node_graph& operator=(managed_node_graph&&) noexcept;

  public: /* node decl registration */
    /// register new node info
    [[nodiscard]] bool register_node_decl(const node_declaration&);

    /// register new node info
    [[nodiscard]] bool register_node_decl(const std::vector<node_declaration>&);

    /// unregister node info
    void unregister_node_decl(const std::string&);

    /// unregister node info
    void unregister_node_decl(const std::vector<std::string>&);

  public: /* node grouping features */
    /// Create new group
    /// \note All nodes should be under same group.
    /// \note Creating group modifies underlying node_graph not just adding
    /// interface nodes.
    /// \returns handle of interface node which represents new group.
    auto group(
      const node_handle& parent_group,
      const std::vector<node_handle>& nodes) -> node_handle;

    /// Ungroup
    /// \note `node` should be interface node which represents node group.
    /// \note `node` should not be root group.
    void ungroup(const node_handle& node);

    /// Group interface?
    bool is_group(const node_handle& node) const;

    /// Group member?
    bool is_group_member(const node_handle& node) const;

    /// Group output node?
    bool is_group_output(const node_handle& node) const;

    /// Group input node?
    bool is_group_input(const node_handle& node) const;

    /// Get parent group interface handler
    auto get_parent_group(const node_handle& node) const -> node_handle;

    /// Get all nodes in group
    auto get_group_members(const node_handle& node) const
      -> std::vector<node_handle>;

    /// Get group output node
    auto get_group_input(const node_handle& node) const -> node_handle;

    /// Get group input node
    auto get_group_output(const node_handle& node) const -> node_handle;

    /// Get root group
    auto root_group() const -> node_handle;

    /// Change name of group
    void set_group_name(const node_handle& group, const std::string& name);

    /// Add group input socket
    /// \note index -1 treated as size of sockets
    /// \requires -1 <= index <= number of sockets
    auto add_group_input_socket(
      const node_handle& group,
      const std::string& socket,
      size_t index = -1) -> socket_handle;

    /// Add group output socket
    /// \note index -1 treated as size of sockets
    /// \requires -1 <= index <= number of sockets
    auto add_group_output_socket(
      const node_handle& group,
      const std::string& socket,
      size_t index = -1) -> socket_handle;

    /// \requires index < number of sockets
    bool set_group_input_socket(
      const node_handle& group,
      const std::string& socket,
      size_t index);

    /// \requires index < number of sockets
    bool set_group_output_socket(
      const node_handle& group,
      const std::string& socket,
      size_t index);

    /// Remove group output socket
    /// \note destroy all connections to the socket
    void remove_group_input_socket(const node_handle& group, size_t index);

    /// Remove group output socket
    /// \note destroy all connections to the socket
    void remove_group_output_socket(const node_handle& group, size_t index);

    /// Peek in group socket
    /// \param socket Group interface output socket
    [[nodiscard]] auto get_group_socket_inside(
      const socket_handle& socket) const -> socket_handle;

    /// Peek out group socket
    /// \param socket Group input handler socket
    [[nodiscard]] auto get_group_socket_outside(
      const socket_handle& socket) const -> socket_handle;

  public:
    /// exist?
    [[nodiscard]] bool exists(const node_handle& node) const;
    /// exist?
    [[nodiscard]] bool exists(const connection_handle& connection) const;
    /// exists?
    [[nodiscard]] bool exists(const socket_handle& socket) const;

    /// get node info
    [[nodiscard]] auto get_info(const node_handle& node) const
      -> std::optional<managed_node_info>;
    /// get socket info
    [[nodiscard]] auto get_info(const socket_handle& socket) const
      -> std::optional<managed_socket_info>;
    /// get connection info
    [[nodiscard]] auto get_info(const connection_handle& connection) const
      -> std::optional<managed_connection_info>;

    /// get name
    [[nodiscard]] auto get_name(const node_handle& node) const
      -> std::optional<std::string>;
    /// get name
    [[nodiscard]] auto get_name(const socket_handle& socket) const
      -> std::optional<std::string>;

    /// get pos
    [[nodiscard]] auto get_pos(const node_handle& node) const
      -> std::optional<tvec2<float>>;
    /// set pos
    void set_pos(const node_handle& node, const tvec2<float>& new_pos);

  public:
    /// create new node
    [[nodiscard]] auto create(
      const node_handle& parent_group,
      const std::string& name) -> node_handle;

    /// create new node
    [[nodiscard]] auto create_shared(
      const node_handle& parent_group,
      const std::string& name) -> shared_node_handle;

    /// destroy node
    /// \note if `node` is group interface, remove all nodes in the node too.
    /// \note I/O handler nodes cannot be destroyed.
    void destroy(const node_handle& node);

    /// connect sockets
    [[nodiscard]] auto connect(
      const socket_handle& src_socket,
      const socket_handle& dst_socket) -> connection_handle;

    /// disconnect sockets
    void disconnect(const connection_handle& handle);

  public:
    /// Get node
    /// \note get_info(socket)->node()
    [[nodiscard]] auto node(const socket_handle& socket) const -> node_handle;
    /// Get list of all nodes
    /// \note includes I/O handlers
    [[nodiscard]] auto nodes() const -> std::vector<node_handle>;
    /// Get list of nodes in group
    /// \note inclueds I/O handlers
    [[nodiscard]] auto nodes(const node_handle& group) const
      -> std::vector<node_handle>;

  public:
    /// Get list of input sockets attached to the node.
    [[nodiscard]] auto input_sockets(const node_handle& node) const
      -> std::vector<socket_handle>;

    /// Get list of output sockets attached to the node.
    [[nodiscard]] auto output_sockets(const node_handle& node) const
      -> std::vector<socket_handle>;

  public:
    /// list connections
    [[nodiscard]] auto connections() const -> std::vector<connection_handle>;

    /// list connections
    [[nodiscard]] auto connections(const node_handle& node) const
      -> std::vector<connection_handle>;

    /// list connections
    [[nodiscard]] auto connections(const socket_handle& socket) const
      -> std::vector<connection_handle>;

    /// list input connections
    [[nodiscard]] auto input_connections(const node_handle& node) const
      -> std::vector<connection_handle>;

    /// list input connections
    [[nodiscard]] auto output_connections(const node_handle& node) const
      -> std::vector<connection_handle>;

    /// primitive?
    [[nodiscard]] auto get_primitive(const node_handle& node) const
      -> std::optional<primitive_t>;

    /// set primitive
    [[nodiscard]] bool set_primitive(
      const node_handle& node,
      const primitive_t& prim);

    /// acecss underlying node_graph
    [[nodiscard]] auto node_graph() const -> const node_graph&;

    /// clear
    void clear();

    /// clone
    [[nodiscard]] auto clone() const -> managed_node_graph;

  private:
    struct node_group;
    struct extra_info;

  private:
    void _init();
    [[nodiscard]] auto _find_parent_group(const node_handle& node)
      -> node_group*;
    [[nodiscard]] auto _find_parent_group(const node_handle& node) const
      -> const node_group*;
    [[nodiscard]] auto _add_group_socket(
      node_group* group,
      socket_type type,
      const std::string& socket,
      size_t index) -> socket_handle;
    [[nodiscard]] bool _set_group_socket(
      node_group* group,
      socket_type type,
      const std::string& socket,
      size_t index);
    void _remove_group_socket(
      node_group* group,
      socket_type type,
      size_t index);

  private:
    yave::node_graph m_ng;
    node_declaration_store m_nim;

  private:
    std::map<node_handle, node_group> m_groups;
    node_handle m_root_group;

  private:
    std::map<node_handle, extra_info> m_extra_info;
  };

} // namespace yave