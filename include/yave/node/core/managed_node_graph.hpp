//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/node_graph.hpp>
#include <yave/node/support/node_info_manager.hpp>
#include <yave/node/support/bind_info_manager.hpp>

#include <memory>

namespace yave {

  // clang-format off

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
    [[nodiscard]]
    bool register_node_info(const node_info& info);
    /// unregister node info
    void unregister_node_info(const node_info& info);

    /// register new node info
    [[nodiscard]]
    bool register_node_info(const std::vector<node_info>& info);
    /// unregister node info
    void unregister_node_info(const std::vector<node_info>& info);

  public:
    /// exist?
    [[nodiscard]]
    bool exists(const node_handle& node) const;
    /// exist?
    [[nodiscard]]
    bool exists(const connection_handle& connection) const;

  public:
    /// create new node
    [[nodiscard]]
    node_handle create(const std::string& name);
    /// destroy node
    void        destroy(const node_handle& handle);

    /// connect sockets
    [[nodiscard]]
    connection_handle connect(const node_handle& src_n, const std::string& src_s, const node_handle& dst_n, const std::string& dst_s);
    /// disconnect sockets
    void              disconnect(const connection_handle& handle);

  public:
    /// list nodes
    [[nodiscard]]
    std::vector<node_handle>       nodes() const;
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
    std::optional<node_info>       get_info(const node_handle& node) const;

    /// get connection info
    [[nodiscard]]
    std::optional<connection_info> get_info(const connection_handle& connection) const;

    /// primitive?
    [[nodiscard]]
    std::optional<primitive_t>     get_primitive(const node_handle& node) const;

    /// set primitive
    [[nodiscard]] 
    bool                           set_primitive(const node_handle& node, const primitive_t& prim);

    /// clear
    void                           clear();

  private:
    node_graph        m_ng;
    node_info_manager m_nim;
  };

  // clang-format on
}