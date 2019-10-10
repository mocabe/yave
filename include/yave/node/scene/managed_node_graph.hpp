//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/node_graph.hpp>
#include <yave/node/core/shared_node_handle.hpp>
#include <yave/node/support/node_info_manager.hpp>
#include <yave/node/support/bind_info_manager.hpp>

#include <memory>

namespace yave {

  class managed_node_graph
  {
  public:
    // Construct empty node tree.
    managed_node_graph();
    /// Copy constructor (deleted).
    managed_node_graph(const managed_node_graph&);
    /// Move constructor.
    managed_node_graph(managed_node_graph&&) noexcept;
    /// Destructor.
    ~managed_node_graph() noexcept;
    /// Copy assignment (deleted).
    managed_node_graph& operator=(const managed_node_graph&);
    /// Move assignment.
    managed_node_graph& operator=(managed_node_graph&&) noexcept;

  public:
    /// register new node info
    [[nodiscard]] bool register_node_info(const node_info& info);

    /// register new node info
    [[nodiscard]] bool register_node_info(const std::vector<node_info>& info);

    /// unregister node info
    void unregister_node_info(const node_info& info);

    /// unregister node info
    void unregister_node_info(const std::vector<node_info>& info);

  public: /* node grouping features */
    /// Create new group
    /// \note All nodes should be in same level.
    /// \note Creating group modifies underlying node_graph not just adding
    /// interface nodes.
    /// \note Nodes in group has level which is current depth of grouping.
    /// \returns handle of interface node which represents new group.
    auto group(const std::vector<node_handle>& nodes) -> node_handle;

    /// Ungroup
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
    auto get_group(const node_handle& node) const -> node_handle;

    /// Get all nodes in group
    auto get_members(const node_handle& node) const -> std::vector<node_handle>;

    /// Get root group
    auto root_group() const -> node_handle;

    /// Add group input socket
    /// \note index -1 treated as number of current sockets
    /// \requires -1 <= index <= number of sockets
    bool add_group_input(const std::string& socket, size_t index = -1);
    /// Add group output socket
    /// \note index -1 treated as number of current sockets
    /// \requires -1 <= index <= number of sockets
    bool add_group_output(const std::string& socket, size_t index = -1);

    /// Change group input socket name
    /// \requires index < number of sockets
    bool set_group_input(const std::string& socket, size_t index);
    /// Change group output socket name
    /// \requires index < number of sockets
    bool set_group_output(const std::string& socket, size_t index);

    /// Remove group input socket
    /// \note destroy all connections to the socket
    void remove_group_input(const std::string& socket);
    /// Remove group output socket
    /// \note destroy all connections to the socket
    void remove_group_output(const std::string& socket);

    /// Remove group input socket
    void move_group_input(const std::string& socket, size_t index);
    /// Remove group output socket
    void move_group_output(const std::string& socket, size_t index);

  public:
    /// exist?
    [[nodiscard]] bool exists(const node_handle& node) const;
    /// exist?
    [[nodiscard]] bool exists(const connection_handle& connection) const;

  public:
    /// create new node
    [[nodiscard]] auto create(const std::string& name) -> node_handle;

    /// create new node
    [[nodiscard]] auto create_shared(const std::string& name)
      -> shared_node_handle;

    /// connect sockets
    [[nodiscard]] auto connect(
      const node_handle& src_n,
      const std::string& src_s,
      const node_handle& dst_n,
      const std::string& dst_s) -> connection_handle;

    /// destroy node
    void destroy(const node_handle& handle);

    /// disconnect sockets
    void disconnect(const connection_handle& handle);

  public:
    /// list nodes
    [[nodiscard]] auto nodes() const -> std::vector<node_handle>;

    /// list connections
    [[nodiscard]] auto connections() const -> std::vector<connection_handle>;

    /// list connections
    [[nodiscard]] auto connections(const node_handle& node) const
      -> std::vector<connection_handle>;

    /// list connections
    [[nodiscard]] auto connections(
      const node_handle& node,
      const std::string& socket) const -> std::vector<connection_handle>;

    /// list input connections
    [[nodiscard]] auto input_connections() const
      -> std::vector<connection_handle>;

    /// list input connections
    [[nodiscard]] auto input_connections(const node_handle& node) const
      -> std::vector<connection_handle>;

    /// list input connections
    [[nodiscard]] auto input_connections(
      const node_handle& node,
      const std::string& socket) const -> std::vector<connection_handle>;

    /// list input connections
    [[nodiscard]] auto output_connections() const
      -> std::vector<connection_handle>;

    /// list input connections
    [[nodiscard]] auto output_connections(const node_handle& node) const
      -> std::vector<connection_handle>;

    /// list input connections
    [[nodiscard]] auto output_connections(
      const node_handle& node,
      const std::string& socket) const -> std::vector<connection_handle>;

    /// get node info
    [[nodiscard]] auto get_info(const node_handle& node) const
      -> std::optional<node_info>;

    /// get connection info
    [[nodiscard]] auto get_info(const connection_handle& connection) const
      -> std::optional<connection_info>;

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
    node_graph m_ng;
    node_info_manager m_nim;
  };

} // namespace yave