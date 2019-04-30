//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/node_graph.hpp>
#include <yave/tools/log.hpp>

namespace yave {
  namespace {

    /// Traverse nodes until return true.
    template <class Lambda>
    void depth_first_search_until(
      const NodeGraph& ng,
      const NodeHandle& node,
      Lambda&& lambda);

    /// Traverse nodes.
    template <class Lambda>
    void depth_first_search(
      const NodeGraph& ng,
      const NodeHandle& node,
      Lambda&& lambda);

    /// Find a closed loop in node graph.
    std::vector<NodeHandle>
      find_loop(const NodeGraph& ng, const NodeHandle& node);

  } // namespace

  NodeGraph::NodeGraph()
    : m_g {}
    , m_mtx {}
  {
    [[maybe_unused]] static auto init_logger = []() {
      return add_logger("NodeGraph");
    }();
  }

  NodeGraph::~NodeGraph()
  {
  }

  NodeGraph::NodeGraph(const NodeGraph& other)
    : m_g {other.m_g.clone()}
    , m_mtx {}
  {
  }

  NodeGraph::NodeGraph(NodeGraph&& other)
    : m_g {std::move(other.m_g)}
    , m_mtx {}
  {
  }

  bool NodeGraph::exists(const NodeHandle& h) const
  {
    return h.has_value() && m_g.exists(h.descriptor()) &&
           h.id() == m_g.id(h.descriptor());
  }

  bool NodeGraph::exists(const ConnectionHandle& h) const
  {
    return h.has_value() && m_g.exists(h.descriptor()) &&
           h.id() == m_g.id(h.descriptor());
  }

  NodeHandle NodeGraph::add_node(const NodeInfo& info, const primitive_t& prim)
  {
    // add node
    auto node = [&]() {
      if (info.is_prim())
        return m_g.add_node(info.name(), prim);
      else
        return m_g.add_node(info.name(), std::monostate {});
    }();

    if (!node)
      throw std::runtime_error(
        "Failed to add node to NodeGraph: Failed to create new node.");

    // attach sockets
    auto _attach_sockets = [&](auto&& names, auto io) {
      // attach sockets
      for (auto&& name : names) {
        auto socket = m_g.add_socket(name, io);
        if (!m_g.attach_socket(node, socket)) {
          if (node)
            m_g.remove_node(node);
          throw std::runtime_error(
            "Failed to add node to NodeGraph: Failed to attach sockets.");
        }
      }
    };

    _attach_sockets(info.input_sockets(), SocketProperty::input);
    _attach_sockets(info.output_sockets(), SocketProperty::output);

    auto handle = NodeHandle(node, m_g.id(node));

    Info(
      get_logger("NodeGraph"),
      "Created Node: name=\"{}\", id={}",
      info.name(),
      handle.id());

    return handle;
  }

  void NodeGraph::remove_node(const NodeHandle& h)
  {
    if (!exists(h))
      return;

    Info(
      get_logger("NodeGraph"),
      "Removing Node: name=\"{}\", id={}",
      m_g[h.descriptor()].name(),
      h.id());

    for (auto&& s : m_g.sockets(h.descriptor())) {
      m_g.remove_socket(s);
    }
    m_g.remove_node(h.descriptor());
  }

  ConnectionHandle NodeGraph::connect(
    const NodeHandle& src_node,
    const std::string& src_socket,
    const NodeHandle& dst_node,
    const std::string& dst_socket)
  {
    return connect(ConnectionInfo {src_node, src_socket, dst_node, dst_socket});
  }

  ConnectionHandle NodeGraph::connect(const ConnectionInfo& info)
  {
    auto logger = get_logger("NodeGraph");
    assert(logger);

    // check handler
    if (!exists(info.src_node()) || !exists(info.dst_node())) {
      Error(logger, "Failed to connect sockets: Invalid node descriptor");
      return nullptr;
    }
    // check socket name
    for (auto&& s : m_g.sockets(info.src_node().descriptor())) {
      if (m_g[s].name() == info.src_socket() && m_g[s].is_output()) {
        for (auto&& d : m_g.sockets(info.dst_node().descriptor())) {
          if (m_g[d].name() == info.dst_socket() && m_g[d].is_input()) {
            // already exists
            for (auto&& e : m_g.dst_edges(d))
              if (m_g.src(e) == s) {
                Warning(
                  logger,
                  "Connection already exists. Return existing connection "
                  "handle.");
                return ConnectionHandle(e, m_g.id(e));
              }
            // input edge cannot have multiple inputs
            if (m_g.n_dst_edges(d) != 0) {
              Error(
                logger,
                "Failed to connect sockets: Multiple input is not allowed");
              return nullptr;
            }

            // add new edge to graph
            auto new_edge = m_g.add_edge(s, d);
            assert(new_edge);

            // closed loop check
            if (!find_loop(*this, info.src_node()).empty()) {
              Error(
                logger,
                "Failed to connect sockets: Closed loop is not allowed.");
              m_g.remove_edge(new_edge);
              return nullptr;
            }

            Info(
              logger,
              "Connected socket: src=\"{}\"({})::{}, dst=\"{}\"({})::{}",
              m_g[info.src_node().descriptor()].name(),
              info.src_node().id(),
              info.src_socket(),
              m_g[info.dst_node().descriptor()].name(),
              info.dst_node().id(),
              info.dst_socket());

            return ConnectionHandle(new_edge, m_g.id(new_edge));
          }
        }
      }
    }

    // fail
    Error(logger, "Failed to connect sockets: Invalid argument.");
    return nullptr;
  }

  void NodeGraph::disconnect(const ConnectionHandle& h)
  {
    // check handler
    if (!exists(h))
      return;

    auto info = connection_info(h);
    Info(
      get_logger("NodeGraph"),
      "Disconnecting: src={}::{} dst={}::{}",
      info->src_node().id(),
      info->src_socket(),
      info->dst_node().id(),
      info->dst_socket());

    // remove edge
    m_g.remove_edge(h.descriptor());
  }

  std::vector<NodeHandle> NodeGraph::nodes(const std::string& name) const
  {
    std::vector<NodeHandle> ret;
    for (auto&& n : m_g.nodes())
      if (m_g[n].name() == name)
        ret.emplace_back(n, m_g.id(n));
    return ret;
  }

  std::vector<NodeHandle> NodeGraph::nodes() const
  {
    std::vector<NodeHandle> ret;
    for (auto&& n : m_g.nodes()) ret.emplace_back(n, m_g.id(n));
    return ret;
  }

  std::optional<NodeInfo> NodeGraph::node_info(const NodeHandle& h) const
  {
    // check handler
    if (!exists(h))
      return std::nullopt;

    // node
    auto& n = m_g[h.descriptor()];

    std::vector<std::string> input_sockets;
    std::vector<std::string> output_sockets;

    for (auto&& s : m_g.sockets(h.descriptor())) {
      if (m_g[s].is_input())
        input_sockets.emplace_back(m_g[s].name());
      else
        output_sockets.emplace_back(m_g[s].name());
    }

    return NodeInfo {n.name(), input_sockets, output_sockets, n.is_prim()};
  }

  std::optional<std::string> NodeGraph::node_name(const NodeHandle& node) const
  {
    if (!exists(node))
      return std::nullopt;
    return m_g[node.descriptor()].name();
  }

  bool
    NodeGraph::has_socket(const NodeHandle& h, const std::string& socket) const
  {
    if (!exists(h))
      return false;
    auto sockets = m_g.sockets(h.descriptor());
    for (auto&& s : sockets)
      if (m_g[s].name() == socket)
        return true;
    return false;
  }

  bool NodeGraph::is_input_socket(
    const NodeHandle& h,
    const std::string& socket) const
  {
    if (!exists(h))
      return false;
    auto sockets = m_g.sockets(h.descriptor());
    for (auto&& s : sockets) {
      if (m_g[s].name() == socket && m_g[s].is_input())
        return true;
    }
    return false;
  }

  bool NodeGraph::is_output_socket(
    const NodeHandle& h,
    const std::string& socket) const
  {
    if (!exists(h))
      return false;
    auto sockets = m_g.sockets(h.descriptor());
    for (auto&& s : sockets) {
      if (m_g[s].name() == socket && m_g[s].is_output())
        return true;
    }
    return false;
  }

  std::vector<ConnectionHandle> NodeGraph::input_connections(
    const NodeHandle& h,
    const std::string& socket) const
  {
    if (!exists(h))
      return {};

    std::vector<ConnectionHandle> ret;
    auto sockets = m_g.sockets(h.descriptor());
    for (auto&& s : sockets) {
      if (m_g[s].name() == socket) {
        auto dst_edges = m_g.dst_edges(s);
        if (m_g[s].is_output())
          assert(dst_edges.empty());
        for (auto&& e : dst_edges) ret.emplace_back(e, m_g.id(e));
      }
    }
    return ret;
  }

  std::vector<ConnectionHandle> NodeGraph::output_connections(
    const NodeHandle& h,
    const std::string& socket) const
  {
    if (!exists(h))
      return {};

    std::vector<ConnectionHandle> ret;
    auto sockets = m_g.sockets(h.descriptor());
    for (auto&& s : sockets) {
      if (m_g[s].name() == socket) {
        auto src_edges = m_g.src_edges(s);
        if (m_g[s].is_input())
          assert(src_edges.empty());
        for (auto&& e : src_edges) ret.emplace_back(e, m_g.id(e));
      }
    }
    return ret;
  }

  [[nodiscard]] bool
    NodeGraph::has_connection(const NodeHandle& h, const std::string& s) const
  {
    return !(
      input_connections(h, s).empty() && output_connections(h, s).empty());
  }

  std::vector<ConnectionHandle>
    NodeGraph::connections(const NodeHandle& h) const
  {
    if (!exists(h))
      return {};

    std::vector<ConnectionHandle> ret;
    auto sockets = m_g.sockets(h.descriptor());
    for (auto&& s : sockets) {
      auto src_edges = m_g.src_edges(s);
      auto dst_edges = m_g.dst_edges(s);
      assert(
        (m_g[s].is_input() && src_edges.empty()) ||
        (m_g[s].is_output() && dst_edges.empty()));
      for (auto&& e : src_edges) ret.emplace_back(e, m_g.id(e));
      for (auto&& e : dst_edges) ret.emplace_back(e, m_g.id(e));
    }
    return ret;
  }

  std::vector<ConnectionHandle>
    NodeGraph::connections(const NodeHandle& h, const std::string& socket) const
  {
    if (!exists(h))
      return {};
    std::vector<ConnectionHandle> ret;
    auto sockets = m_g.sockets(h.descriptor());
    for (auto&& s : sockets) {
      if (m_g[s].name() == socket) {
        auto src_edges = m_g.src_edges(s);
        auto dst_edges = m_g.dst_edges(s);
        for (auto&& e : src_edges) ret.emplace_back(e, m_g.id(e));
        for (auto&& e : dst_edges) ret.emplace_back(e, m_g.id(e));
      }
    }
    if (ret.empty())
      Warning(
        get_logger("NodeGraph"), "Call for connections() with invalid socket name.");
    return ret;
  }

  std::vector<ConnectionHandle>
    NodeGraph::input_connections(const NodeHandle& h) const
  {
    if (!exists(h))
      return {};
    std::vector<ConnectionHandle> ret;
    auto sockets = m_g.sockets(h.descriptor());
    for (auto&& s : sockets) {
      auto dst_edges = m_g.dst_edges(s);
      assert(m_g[s].is_input() || (m_g[s].is_output() && dst_edges.empty()));
      for (auto&& e : dst_edges) {
        ret.emplace_back(e, m_g.id(e));
      }
    }
    if (ret.empty())
      Warning(
        get_logger("NodeGraph"),
        "Call for input_connections() with invalid socket name.");
    return ret;
  }

  std::vector<ConnectionHandle>
    NodeGraph::output_connections(const NodeHandle& h) const
  {
    if (!exists(h))
      return {};
    std::vector<ConnectionHandle> ret;
    auto sockets = m_g.sockets(h.descriptor());
    for (auto&& s : sockets) {
      auto src_edges = m_g.src_edges(s);
      assert((m_g[s].is_input() && src_edges.empty()) || m_g[s].is_output());
      for (auto&& e : src_edges) ret.emplace_back(e, m_g.id(e));
    }
    if (ret.empty())
      Warning(
        get_logger("NodeGraph"),
        "Call for output_connections() with invalid socket name.");
    return ret;
  }

  std::vector<ConnectionHandle> NodeGraph::connections() const
  {
    std::vector<ConnectionHandle> ret;
    for (auto&& n : m_g.nodes()) {
      for (auto&& s : m_g.sockets(n)) {
        for (auto&& e : m_g.src_edges(s)) ret.emplace_back(e, m_g.id(e));
      }
    }
    return ret;
  }

  std::optional<ConnectionInfo>
    NodeGraph::connection_info(const ConnectionHandle& h) const
  {
    if (!exists(h))
      return std::nullopt;
    auto src = m_g.src(h.descriptor());
    auto dst = m_g.dst(h.descriptor());

    auto src_nodes = m_g.nodes(src);
    auto dst_nodes = m_g.nodes(dst);
    assert(src_nodes.size() == 1);
    assert(dst_nodes.size() == 1);
    return ConnectionInfo {NodeHandle(src_nodes[0], m_g.id(src_nodes[0])), //
                           m_g[src].name(),                                //
                           NodeHandle(dst_nodes[0], m_g.id(dst_nodes[0])), //
                           m_g[dst].name()};                               //
  }

  std::vector<std::string> NodeGraph::input_sockets(const NodeHandle& h) const
  {
    if (!exists(h))
      return {};
    auto&& sockets = m_g.sockets(h.descriptor());
    std::vector<std::string> ret;
    for (auto&& s : sockets) {
      if (m_g[s].is_input())
        ret.push_back(m_g[s].name());
    }
    return ret;
  }

  std::vector<std::string> NodeGraph::output_sockets(const NodeHandle& h) const
  {
    if (!exists(h))
      return {};
    auto&& sockets = m_g.sockets(h.descriptor());
    std::vector<std::string> ret;
    for (auto&& s : sockets) {
      if (m_g[s].is_output())
        ret.push_back(m_g[s].name());
    }
    return ret;
  }

  bool NodeGraph::is_primitive(const NodeHandle& h) const
  {
    if (!exists(h))
      return false;
    return m_g[h.descriptor()].is_prim();
  }

  std::optional<primitive_t> NodeGraph::get_primitive(const NodeHandle& h) const
  {
    if (!exists(h))
      return std::nullopt;
    if (m_g[h.descriptor()].is_prim())
      return m_g[h.descriptor()].prim();
    return std::nullopt;
  }

  void NodeGraph::set_primitive(const NodeHandle& h, const primitive_t& prim)
  {
    if (!is_primitive(h)) {
      Error(
        get_logger("NodeGraph"),
        "Cannot set primitive value to non-primitive node. Ignored.");
      return;
    }
    m_g[h.descriptor()].set_prim(prim);
  }

  std::unique_lock<std::mutex> NodeGraph::lock() const
  {
    return std::unique_lock(m_mtx);
  }

  void NodeGraph::clear()
  {
    m_g.clear();
  }

  namespace {
    template <class Lambda>
    void depth_first_search_until(
      const NodeGraph& ng,
      const NodeHandle& node,
      Lambda&& lambda)
    {
      std::vector<NodeHandle> visited_nodes;
      std::vector<NodeHandle> stack;

      auto visit = [&](const NodeHandle& node) {
        visited_nodes.push_back(node);
        stack.push_back(node);
      };

      auto visited = [&](const NodeHandle& node) {
        for (auto&& n : visited_nodes)
          if (n == node)
            return true;
        return false;
      };

      // visit first node
      visit(node);
      if (std::forward<Lambda>(lambda)(node))
        return;

      // main loop
      while (!stack.empty()) {

        auto current = stack.back();

        /// visit unvisited child node.
        /// pop when all childs have been visited.
        bool stop = [&] {
          for (auto&& c : ng.input_connections(current)) {
            auto next = ng.connection_info(c)->src_node();
            if (!visited(next)) {
              visit(next);
              return std::forward<Lambda>(lambda)(node);
            }
          }
          stack.pop_back();
          return false;
        }();

        if (stop)
          break;
      }
    }

    template <class Lambda>
    void depth_first_search(
      const NodeGraph& ng,
      const NodeHandle& node,
      Lambda&& lambda)
    {
      return depth_first_search_until(ng, node, [&](auto&& n) {
        std::forward<Lambda>(lambda)(n);
        return false;
      });
    }

    std::vector<NodeHandle>
      find_loop(const NodeGraph& ng, const NodeHandle& node)
    {
      std::vector<NodeHandle> visited_nodes;
      std::vector<NodeHandle> stack;

      auto visit = [&](const NodeHandle& node) {
        visited_nodes.push_back(node);
        stack.push_back(node);
      };

      auto visited = [&](const NodeHandle& node) {
        for (auto&& n : visited_nodes)
          if (n == node)
            return true;
        return false;
      };

      visit(node);

      // loop
      std::vector<NodeHandle> ret;

      while (!stack.empty()) {

        auto current = stack.back();

        bool stop = [&] {
          for (auto&& c : ng.input_connections(current)) {
            auto next = ng.connection_info(c)->src_node();
            if (visited(next)) {
              // find closed loop
              for (auto iter = stack.begin(); iter != stack.end(); ++iter) {
                if (*iter == next) {
                  ret = {iter, stack.end()};
                  return true;
                }
              }
            } else {
              visit(next);
              return false;
            }
          }
          stack.pop_back();
          return false;
        }();

        if (stop)
          break;
      }

      return ret;
    }
  } // namespace
} // namespace yave