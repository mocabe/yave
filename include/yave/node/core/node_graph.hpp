//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/graph_definition.hpp>
#include <yave/node/core/node_handle.hpp>
#include <yave/node/core/connection_handle.hpp>
#include <yave/node/core/node_info.hpp>
#include <yave/node/core/connection_info.hpp>

#include <optional>
#include <mutex>

namespace yave {

  /// Node graph.
  class node_graph
  {
  public:
    /// Constructor
    node_graph();

    /// Destructor
    ~node_graph() noexcept;

    /// Copy
    node_graph(const node_graph&);

    /// Move constructor
    node_graph(node_graph&&) noexcept;

    /// Copy assignment
    node_graph& operator=(const node_graph& other);

    /// Move assignment
    node_graph& operator=(node_graph&& other) noexcept;

    /// exists
    [[nodiscard]] bool exists(const node_handle& node) const;

    /// exists
    [[nodiscard]] bool exists(const connection_handle& connection) const;

    /// Add new node.
    /// \returns Non-null handle of new node.
    /// \throws std::bad_alloc, std::runtime_error and other exceptions which
    /// can be thrown from edge_property constructors.
    [[nodiscard]] node_handle
      add(const node_info& info, const primitive_t& prim = {});

    /// Remove node from graph.
    /// Destroy the node, all sockets connected to the node, and all edges
    /// connected to these sockets.
    /// \throws should not throw exception.
    void remove(const node_handle& node);

    /// Connect sockets.
    /// When connection already exists, return handle of existing connection.
    /// If the connection to establish is invalid (loop, etc.), return
    /// null handle.
    /// \returns Handle of connection (can be null handle).
    /// \throws std::bad_alloc
    [[nodiscard]] connection_handle connect(const connection_info& info);

    /// Connect sockets.
    /// See `connect(const connection_info&)`.
    [[nodiscard]] connection_handle connect(
      const node_handle& src_node,
      const std::string& src_socket,
      const node_handle& dst_node,
      const std::string& dst_socket);

    /// Disconnect sockets.
    /// \throws should not throw exception.
    void disconnect(const connection_handle& connection);

    /// Find nodes from name.
    /// \returns Empty vector when not found.
    [[nodiscard]] std::vector<node_handle> nodes(const std::string& name) const;

    /// Get list of nodes.
    [[nodiscard]] std::vector<node_handle> nodes() const;

    /// Get node info from handle.
    /// \returns std::nullopt when the node did not exist.
    [[nodiscard]] std::optional<node_info>
      get_info(const node_handle& node) const;

    /// Get node name from handle.
    /// \returns std::nullopt when the node did not exist.
    [[nodiscard]] std::optional<std::string>
      get_name(const node_handle& node) const;

    /// Check if the node has a socket with the name.
    [[nodiscard]] bool
      has_socket(const node_handle& node, const std::string& socket) const;

    /// Check if socket type is input.
    /// \returns false when the socket does not exist.
    [[nodiscard]] bool
      is_input_socket(const node_handle& node, const std::string& socket) const;

    /// Check if socket type is output.
    /// \returns false when the socket does not exist.
    [[nodiscard]] bool is_output_socket(
      const node_handle& node,
      const std::string& socket) const;

    /// List all input connections.
    /// \returns Result should be same to connections(), but order of handles
    /// may differ.
    [[nodiscard]] std::vector<connection_handle> input_connections() const;

    /// Get list of input connections to the node.
    [[nodiscard]] std::vector<connection_handle>
      input_connections(const node_handle& node) const;

    /// Find input connection to the node.
    [[nodiscard]] std::vector<connection_handle> input_connections(
      const node_handle& node,
      const std::string& socket) const;

    /// List all output connections.
    /// \returns Result should be same to connections(), but order of handles
    /// may differ.
    [[nodiscard]] std::vector<connection_handle> output_connections() const;

    /// Get list of output connections from the node.
    [[nodiscard]] std::vector<connection_handle>
      output_connections(const node_handle& node) const;

    /// Find output connection from the node.
    [[nodiscard]] std::vector<connection_handle> output_connections(
      const node_handle& node,
      const std::string& socket) const;

    /// Get all connections.
    [[nodiscard]] std::vector<connection_handle> connections() const;

    /// Get list of all connections from/to the node.
    [[nodiscard]] std::vector<connection_handle>
      connections(const node_handle& node) const;

    /// Get list of input/output connections from specific socket of the node.
    [[nodiscard]] std::vector<connection_handle>
      connections(const node_handle& node, const std::string& socket) const;

    /// Check if connection exists.
    [[nodiscard]] bool
      has_connection(const node_handle& node, const std::string& socket) const;

    /// Get connection info of all connections from/to the node.
    [[nodiscard]] std::optional<connection_info>
      get_info(const connection_handle& node) const;

    /// Get list of input sockets attached to the node.
    [[nodiscard]] std::vector<std::string>
      input_sockets(const node_handle& node) const;

    /// Get list of output sockets attached to the node.
    [[nodiscard]] std::vector<std::string>
      output_sockets(const node_handle& node) const;

    /// Primitive node?
    [[nodiscard]] bool is_primitive(const node_handle& node) const;

    /// Get primitive value.
    /// \returns std::nullopt when `node` is not primitive node.
    [[nodiscard]] std::optional<primitive_t>
      get_primitive(const node_handle& node) const;

    /// Set primitive value.
    /// When `is_primitive(prim_node) == false`, no effect.
    void set_primitive(const node_handle& prim_node, const primitive_t& prim);

    /// Get shared primitive object
    object_ptr<PrimitiveContainer>
      get_primitive_container(const node_handle& node) const;

    /// Get list of root nodes.
    [[nodiscard]] std::vector<node_handle> roots() const;

    /// Get root of ancestor tree of specific node.
    [[nodiscard]] std::vector<node_handle>
      root_of(const node_handle& node) const;

    /// DFS until return true.
    /// \param node starting node
    /// \param on_visit function object which takes current node as `const
    /// node_handle&`, current path as `const std::vector<node_handle>&`, and
    /// returns bool to stop/continue traversing. will be called once on each
    /// node found in tree.
    /// \param on_visited
    template <class Lambda1>
    void depth_first_search_until(const node_handle& node, Lambda1&& on_visit)
      const;

    /// DFS
    template <class Lambda>
    void depth_first_search(const node_handle& node, Lambda&& on_visit) const;

    /// clear graph
    void clear();

  private: /* non locking helpers */
    bool _exists(const node_handle&) const;
    bool _exists(const connection_handle&) const;
    std::vector<node_handle> _find_loop(const node_handle&) const;
    std::optional<connection_info> _get_info(const connection_handle&) const;
    std::optional<node_info> _get_info(const node_handle&) const;
    std::vector<node_handle> _root_of(const node_handle&) const;
    std::unique_lock<std::mutex> _lock() const;
    template <class Lambda1, class Lambda2>
    void _depth_first_search_until(
      const node_handle& node,
      Lambda1&& on_visit,
      Lambda2&& on_visited) const;

  private:
    /// graph
    graph_t m_g;

  private:
    /// mutex
    mutable std::mutex m_mtx;
  };

  /* impl */

  template <class Lambda1, class Lambda2>
  void node_graph::_depth_first_search_until(
    const yave::node_handle& node,
    Lambda1&& on_visit,
    Lambda2&& on_visited) const
  {
    if (!_exists(node))
      return;

    for (auto&& n : m_g.nodes()) {
      assert(m_g[n].is_unvisited());
    }

    // * stack variables *
    // n_stack: path from start to current node.
    // i_stack: extra info to store next index to visit.
    auto n_stack = std::vector<node_handle> {};
    auto i_stack = std::vector<size_t> {};

    auto visit = [&](const node_handle& n) {
      m_g[n.descriptor()].set_visited();
      i_stack.back() += 1;
      n_stack.push_back(n);
      i_stack.push_back(0);
    };

    auto visited = [&](const node_handle& n) {
      return m_g[n.descriptor()].is_visited();
    };

    auto call_lambda = [](
                         auto&& lambda,
                         const node_handle& node,
                         const std::vector<node_handle>& path) {
      return lambda(node, path);
    };

    // visit first node, init stacks.
    {
      m_g[node.descriptor()].set_visited();
      n_stack.push_back(node);
      i_stack.push_back(0);
      if (call_lambda(on_visit, node, n_stack)) {
        m_g[node.descriptor()].set_unvisited();
        return;
      }
    }

    // main loop
    while (!n_stack.empty()) {

      auto current_node  = n_stack.back();
      auto current_index = i_stack.back();

      bool stop = [&] {
        auto sockets = m_g.sockets(current_node.descriptor());
        for (size_t i = current_index; i < sockets.size(); ++i) {
          auto& s = sockets[i];
          for (auto&& e : m_g.dst_edges(s)) {
            assert(m_g.nodes(m_g.src(e)).size() == 1);
            auto n    = m_g.nodes(m_g.src(e))[0];
            auto next = node_handle(n, uid {m_g.id(n)});
            // not visited yet
            if (!visited(next)) {
              visit(next);
              return call_lambda(on_visit, next, n_stack);
            }
            // already visited
            if (call_lambda(on_visited, next, n_stack))
              return true;
          }
        }
        n_stack.pop_back();
        i_stack.pop_back();
        return false;
      }();

      if (stop)
        break;
    }

    // clear mark
    for (auto&& n : m_g.nodes()) {
      m_g[n].set_unvisited();
    }
  }

  template <class Lambda1>
  void node_graph::depth_first_search_until(
    const node_handle& node,
    Lambda1&& on_visit) const
  {
    auto lck = _lock();
    return _depth_first_search_until(
      node, std::forward<Lambda1>(on_visit), [](auto&&, auto&&) {
        return false;
      });
  }

  template <class Lambda>
  void node_graph::depth_first_search(
    const yave::node_handle& node,
    Lambda&& lambda) const
  {
    auto lck = _lock();
    return _depth_first_search_until(
      node,
      [&](const node_handle& n, const std::vector<node_handle>& path) {
        std::forward<Lambda>(lambda)(n, path);
        return false;
      },
      [](auto&&, auto&&) { return false; });
  }

} // namespace yave

namespace yave {

  struct NodesDiff
  {
    /// not changed
    std::vector<node_handle> not_changed;
    /// removed
    std::vector<node_handle> removed;
    /// added
    std::vector<node_handle> added;
  };

  struct ConnectionsDiff
  {
    /// not changed
    std::vector<connection_handle> not_changed;
    /// removed
    std::vector<connection_handle> removed;
    /// added
    std::vector<connection_handle> added;
  };

  /// Calculate diff
  [[nodiscard]] NodesDiff nodes_diff(
    const std::vector<node_handle>& prev_nodes,
    const std::vector<node_handle>& nodes);

  /// Calculate diff
  [[nodiscard]] ConnectionsDiff connections_diff(
    const std::vector<connection_handle>& prev_connections,
    const std::vector<connection_handle>& connections);

} // namespace yave