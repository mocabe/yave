//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/node_graph.hpp>
#include <yave/node/core/shared_node_handle.hpp>
#include <yave/node/core/node_info_manager.hpp>
#include <yave/node/core/bind_info_manager.hpp>
#include <yave/node/core/node_initializer.hpp>

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

  public: /* node info registration */
    /// register new node info
    [[nodiscard]] bool register_node_info(const node_info& info);

    /// register new node info
    [[nodiscard]] bool register_node_info(const std::vector<node_info>& info);

    /// unregister node info
    void unregister_node_info(const node_info& info);

    /// unregister node info
    void unregister_node_info(const std::vector<node_info>& info);

  public: /* node initializer registration */
    /// register new initializer
    [[nodiscard]] bool register_node_initializer(
      const node_initializer& initializer);

    /// register new initializer
    [[nodiscard]] bool register_node_initializer(
      const std::vector<node_initializer>& initializer);

    /// unregister initializer
    void unregister_node_initializer(const node_initializer& initializer);

    /// unregister initizlier
    void unregister_node_initializer(
      const std::vector<node_initializer>& initializer);

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
    bool add_group_input_socket(
      const node_handle& group,
      const std::string& socket,
      size_t index = -1);

    bool add_group_output_socket(
      const node_handle& group,
      const std::string& socket,
      size_t index = -1);

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

  public:
    /// exist?
    [[nodiscard]] bool exists(const node_handle& node) const;
    /// exist?
    [[nodiscard]] bool exists(const connection_handle& connection) const;
    /// exists?
    [[nodiscard]] bool exists(const socket_handle& socket) const;

    /// get node info
    [[nodiscard]] auto get_info(const node_handle& node) const
      -> std::optional<node_info>;
    /// get socket info
    [[nodiscard]] auto get_info(const socket_handle& socket) const
      -> std::optional<socket_info>;
    /// get connection info
    [[nodiscard]] auto get_info(const connection_handle& connection) const
      -> std::optional<connection_info>;

    /// get name
    [[nodiscard]] auto get_name(const node_handle& node) const
      -> std::optional<std::string>;
    /// get name
    [[nodiscard]] auto get_name(const socket_handle& socket) const
      -> std::optional<std::string>;

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

    /// Get node graph copy
    [[nodiscard]] auto get_node_graph() const -> const node_graph&;

    /// clear
    void clear();

  private:
    struct node_group;

  private:
    void _init();
    [[nodiscard]] auto _find_parent_group(const node_handle& node)
      -> node_group*;
    [[nodiscard]] auto _find_parent_group(const node_handle& node) const
      -> const node_group*;
    [[nodiscard]] bool _add_group_socket(
      node_group* group,
      socket_type type,
      const std::string& socket,
      size_t index);
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
    node_graph m_ng;
    node_info_manager m_nim;

  private:
    std::map<node_handle, node_group> m_groups;
    node_handle m_root_group;
  };

} // namespace yave