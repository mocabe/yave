//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/node_graph.hpp>

namespace yave {

  NodeGraph::NodeGraph()
    : m_g {}
    , m_mtx {}
  {
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
    return h.has_value() && m_g.exists(h.descriptor());
  }

  bool NodeGraph::exists(const ConnectionHandle& h) const
  {
    return h.has_value() && m_g.exists(h.descriptor());
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

    // attach sockets
    auto _attach_sockets = [&](auto&& names, auto io) {
      // attach sockets
      for (auto&& name : names) {
        auto socket = m_g.add_socket(name, io);
        if (!m_g.attach_socket(node, socket)) {
          if (node)
            m_g.remove_node(node);
          throw std::runtime_error("Failed to add node to NodeGraph");
        }
      }
    };

    _attach_sockets(info.input_sockets(), SocketProperty::input);
    _attach_sockets(info.output_sockets(), SocketProperty::output);

    return NodeHandle(node);
  }

  void NodeGraph::remove_node(const NodeHandle& h)
  {
    if (!exists(h))
      return;
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
    // check handler
    if (!exists(info.src_node()) || !exists(info.dst_node()))
      return nullptr;
    // should not connect itself
    if (info.src_node() == info.dst_node())
      return nullptr;
    // check socket name
    for (auto&& s : m_g.sockets(info.src_node().descriptor())) {
      if (m_g[s].name() == info.src_socket() && m_g[s].is_output()) {
        for (auto&& d : m_g.sockets(info.dst_node().descriptor())) {
          if (m_g[d].name() == info.dst_socket() && m_g[d].is_input()) {
            // already exists
            for (auto&& e : m_g.dst_edges(d))
              if (m_g.src(e) == s)
                return ConnectionHandle(e);
            // input edge cannot have multiple inputs
            if (m_g.n_dst_edges(d) != 0)
              return nullptr;
            // add
            return m_g.add_edge(s, d);
          }
        }
      }
    }
    // fail
    return nullptr;
  }

  void NodeGraph::disconnect(const ConnectionHandle& h)
  {
    // check handler
    if (!exists(h))
      return;
    // remove edge
    m_g.remove_edge(h.descriptor());
  }

  std::vector<NodeHandle> NodeGraph::nodes(const std::string& name) const
  {
    std::vector<NodeHandle> ret;
    for (auto&& n : m_g.nodes())
      if (m_g[n].name() == name)
        ret.emplace_back(n);
    return ret;
  }

  std::vector<NodeHandle> NodeGraph::nodes() const
  {
    std::vector<NodeHandle> ret;
    for (auto&& n : m_g.nodes()) ret.emplace_back(n);
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
        for (auto&& e : dst_edges) ret.emplace_back(e);
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
        for (auto&& e : src_edges) ret.emplace_back(e);
      }
    }
    return ret;
  }

  [[nodiscard]] bool
    NodeGraph::is_connected(const NodeHandle& h, const std::string& s) const
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
      for (auto&& e : src_edges) ret.emplace_back(e);
      for (auto&& e : dst_edges) ret.emplace_back(e);
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
        for (auto&& e : src_edges) ret.emplace_back(e);
        for (auto&& e : dst_edges) ret.emplace_back(e);
      }
    }
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
        ret.emplace_back(e);
      }
    }
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
      for (auto&& e : src_edges) ret.emplace_back(e);
    }
    return ret;
  }

  std::vector<ConnectionHandle> NodeGraph::connections() const
  {
    std::vector<ConnectionHandle> ret;
    for (auto&& n : m_g.nodes()) {
      for (auto&& s : m_g.sockets(n)) {
        for (auto&& e : m_g.src_edges(s)) ret.emplace_back(e);
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
    return ConnectionInfo {src_nodes[0],     //
                           m_g[src].name(),  //
                           dst_nodes[0],     //
                           m_g[dst].name()}; //
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
    assert(is_primitive(h));
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

} // namespace yave