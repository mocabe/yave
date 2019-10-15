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

    /// Add new node.
    /// \returns Non-null handle of new node.
    /// \throws std::bad_alloc, std::runtime_error and other exceptions which
    /// can be thrown from edge_property constructors.
    [[nodiscard]] auto add(const node_info& info, const primitive_t& prim = {})
      -> node_handle;

    /// Add new node which has specified ID value.
    /// Mainly for copying exising node from other instance of node_graph
    /// without changing ID.
    /// \note If `info.is_normal()`, parameter `prim` will be ignored.
    /// \note If `info.is_primitive()`, parameter `prim` will be used to
    /// create internal primitive value.
    /// \note If `info.is_interface()`, input and output sockets i `info` should
    /// be empty.
    [[nodiscard]] auto add_with_id(
      const node_info& info,
      const uid& id,
      const primitive_t& prim = {}) -> node_handle;

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
    [[nodiscard]] auto connect(const connection_info& info)
      -> connection_handle;

    /// Connect sockets.
    /// See `connect(const connection_info&)`.
    [[nodiscard]] auto connect(
      const node_handle& src_node,
      const socket_handle& src_socket,
      const node_handle& dst_node,
      const socket_handle& dst_socket) -> connection_handle;

    /// Disconnect sockets.
    /// \throws should not throw exception.
    void disconnect(const connection_handle& connection);

    /// Find node from ID.
    /// \param id id
    /// \returns Null handle when not found
    [[nodiscard]] auto node(const uid& id) const -> node_handle;

    /// Find socket from ID.
    /// \param id id
    /// \returns Null handle when not found.
    [[nodiscard]] auto socket(const uid& id) const -> socket_handle;

    /// Find connection handle from ID.
    /// \param id id
    /// \returns Null handle when not found
    [[nodiscard]] auto connection(const uid& id) const -> connection_handle;

    /// Get all nodes.
    [[nodiscard]] auto nodes() const -> std::vector<node_handle>;

    /// Get nodes
    [[nodiscard]] auto nodes(const std::string& name) const
      -> std::vector<node_handle>;

    /// Get all sockets.
    [[nodiscard]] auto sockets() const -> std::vector<socket_handle>;

    /// Get sockets
    [[nodiscard]] auto sockets(const node_handle& node)
      -> std::vector<socket_handle>;

    /// Get all connections.
    [[nodiscard]] auto connections() const -> std::vector<connection_handle>;

    /// Get connections
    [[nodiscard]] auto connections(const socket_handle& socket) const
      -> std::vector<connection_handle>;

    /// Check if socket type is input.
    /// \returns false when the socket does not exist.
    [[nodiscard]] bool is_input_socket(const socket_handle& socket) const;

    /// Check if socket type is output.
    /// \returns false when the socket does not exist.
    [[nodiscard]] bool is_output_socket(const socket_handle& socket) const;

    /// Get list of input connections to the node.
    [[nodiscard]] auto input_connections(const node_handle& node) const
      -> std::vector<connection_handle>;

    /// Get list of output connections from the node.
    [[nodiscard]] auto output_connections(const node_handle& node) const
      -> std::vector<connection_handle>;

    /// Check if connection exists.
    [[nodiscard]] bool has_connection(const socket_handle& socket) const;

    /// Get list of input sockets attached to the node.
    [[nodiscard]] auto input_sockets(const node_handle& node) const
      -> std::vector<socket_handle>;

    /// Get list of output sockets attached to the node.
    [[nodiscard]] auto output_sockets(const node_handle& node) const
      -> std::vector<socket_handle>;

    /// Normal node?
    [[nodiscard]] bool is_normal(const node_handle& node) const;

    /// Primitive node?
    [[nodiscard]] bool is_primitive(const node_handle& node) const;

    /// Interface node?
    [[nodiscard]] bool is_interface(const node_handle& node) const;

    /// Get primitive value.
    /// \returns std::nullopt when `node` is not primitive node.
    [[nodiscard]] auto get_primitive(const node_handle& node) const
      -> std::optional<primitive_t>;

    /// Set primitive value.
    /// When `is_primitive(prim_node) == false`, no effect.
    void set_primitive(const node_handle& prim_node, const primitive_t& prim);

    /// Get primitive container object.
    /// If `node` is not primitive, retuens pointer to default initialized
    /// primitive container.
    [[nodiscard]] auto get_primitive_container(const node_handle& node) const
      -> object_ptr<const PrimitiveContainer>;

    /// Get list of interfaces nodes attached to the socket
    [[nodiscard]] auto get_interfaces(const socket_handle& socket) const
      -> std::vector<node_handle>;

    /// Get owner node of socket.
    [[nodiscard]] auto get_owner(const socket_handle& socket) const
      -> node_handle;

    /// Get list of root nodes.
    [[nodiscard]] auto roots() const -> std::vector<node_handle>;

    /// Get root of ancestor tree of specific node.
    [[nodiscard]] auto root_of(const node_handle& node) const
      -> std::vector<node_handle>;

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
    [[nodiscard]] bool _exists(const node_handle&) const;
    [[nodiscard]] bool _exists(const socket_handle&) const;
    [[nodiscard]] bool _exists(const connection_handle&) const;
    [[nodiscard]] auto _get_info(const node_handle&) const -> node_info;
    [[nodiscard]] auto _get_info(const socket_handle&) const -> socket_info;
    [[nodiscard]] auto _get_info(const connection_handle&) const
      -> connection_info;
    [[nodiscard]] auto _get_name(const node_handle&) const -> std::string;
    [[nodiscard]] auto _get_name(const socket_handle&) const -> std::string;
    [[nodiscard]] auto _root_of(const node_handle&) const
      -> std::vector<node_handle>;
    [[nodiscard]] auto _find_loop(const node_handle&) const
      -> std::vector<node_handle>;
    [[nodiscard]] auto _lock() const -> std::unique_lock<std::mutex>;

    template <class Lambda1, class Lambda2>
    void _depth_first_search_until(
      const node_handle& node,
      Lambda1&& on_visit,
      Lambda2&& on_visited) const;

  private:
    /// graph type
    graph_t m_g;

  private:
    /// default initialized PrimitiveContainer
    object_ptr<const PrimitiveContainer> m_default_container;

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
                         const node_handle& n,
                         const std::vector<node_handle>& p) {
      return lambda(n, p);
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
