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
    /// unregister node info
    void unregister_node_info(const node_info& info);

    /// register new node info
    [[nodiscard]] bool register_node_info(const std::vector<node_info>& info);
    /// unregister node info
    void unregister_node_info(const std::vector<node_info>& info);

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
    /// destroy node
    void destroy(const node_handle& handle);

    /// connect sockets
    [[nodiscard]] auto connect(
      const node_handle& src_n,
      const std::string& src_s,
      const node_handle& dst_n,
      const std::string& dst_s) -> connection_handle;

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

    /// clear
    void clear();

    /// Get node graph copy
    [[nodiscard]] auto get_node_graph() const -> const node_graph&;

  private:
    node_graph m_ng;
    node_info_manager m_nim;
  };

} // namespace yave