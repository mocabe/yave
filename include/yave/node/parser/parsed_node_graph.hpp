//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core.hpp>
#include <yave/core/graph.hpp>

#include <yave/node/parser/parsed_node_info.hpp>
#include <yave/node/parser/parsed_connection_info.hpp>

namespace yave {

  /// Intermediate representation of node graph after parse stage.
  /// Most of member functions has similar functionality of node_grpah's member
  /// functions. Changing topology of graph should only be done by node_parser's
  /// functions, thus has friend declaration for that.
  class parsed_node_graph
  {
  public:
    friend class node_parser;

    parsed_node_graph();
    parsed_node_graph(const parsed_node_graph&);
    parsed_node_graph(parsed_node_graph&&);
    ~parsed_node_graph();

    /// Check node handle.
    [[nodiscard]] bool exists(const parsed_node_handle& node) const;

    /// Check connection handle.
    [[nodiscard]] bool exists(const parsed_connection_handle& connection) const;

    /// Get all nodes.
    [[nodiscard]] std::vector<parsed_node_handle> nodes() const;

    /// Get info of node.
    [[nodiscard]] std::optional<parsed_node_info>
      get_info(const parsed_node_handle& node) const;

    /// Get info of connection.
    [[nodiscard]] std::optional<parsed_connection_info>
      get_info(const parsed_connection_handle& connection) const;

    /// Get input connection on specific input socket.
    [[nodiscard]] std::vector<parsed_connection_handle> input_connections(
      const parsed_node_handle& node,
      const std::string& socket) const;

    /// List input connections.
    /// \notes Size of input connections should be same to the number of input
    /// sockets when success.
    [[nodiscard]] std::vector<parsed_connection_handle>
      input_connections(const parsed_node_handle& node) const;

    /// Get output connection.
    /// \returns Single output connection or empty on error.
    [[nodiscard]] std::vector<parsed_connection_handle>
      output_connection(const parsed_node_handle& node) const;

    /// Get output socket.
    [[nodiscard]] std::string
      output_socket(const parsed_node_handle& node) const;

    /// Get list of input sockets.
    [[nodiscard]] std::vector<std::string>
      input_sockets(const parsed_node_handle& node) const;

    /// Get list of root nodes.
    [[nodiscard]] std::vector<parsed_node_handle> roots() const;

    /// Lock this graph.
    [[nodiscard]] std::unique_lock<std::mutex> lock() const;

  private:
    [[nodiscard]] parsed_node_handle add(
      const object_ptr<const Object>& instance,
      const object_ptr<const Type>& type,
      const std::shared_ptr<const bind_info>& bind_info,
      bool is_root = false);

    void remove(const parsed_node_handle& node);

    void remove_subtree(const parsed_node_handle& node);

    [[nodiscard]] parsed_connection_handle
      connect(const parsed_connection_info& info);

    [[nodiscard]] parsed_connection_handle connect(
      const parsed_node_handle& src,
      const parsed_node_handle& dst_node,
      const std::string& dst_socket);

    void disconnect(const parsed_connection_handle& connection);

  private:
    parsed_graph_t m_graph;

  private:
    mutable std::mutex m_mtx;
  };

} // namespace yave