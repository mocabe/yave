//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/core/node_graph.hpp>
#include <yave/support/log.hpp>

namespace {

  // logger
  std::shared_ptr<spdlog::logger> g_logger;

  // init
  void init_logger()
  {
    [[maybe_unused]] static auto init_logger = []() {
      g_logger = yave::add_logger("node_graph");
      return 1;
    }();
  }

} // namespace

namespace yave {

  node_graph::node_graph()
    : m_g {}
    , m_mtx {}
  {
    init_logger();
    m_default_container = make_object<PrimitiveContainer>();
  }

  node_graph::~node_graph() noexcept
  {
  }

  node_graph::node_graph(const node_graph& other)
    : m_mtx {}
  {
    auto lck            = other._lock();
    m_g                 = other.m_g.clone();
    m_default_container = other.m_default_container;
  }

  node_graph::node_graph(node_graph&& other) noexcept
    : m_mtx {}
  {
    auto lck            = other._lock();
    m_g                 = std::move(other.m_g);
    m_default_container = std::move(other.m_default_container);
  }

  node_graph& node_graph::operator=(const node_graph& other)
  {
    auto lck1 = _lock();
    auto lck2 = other._lock();

    m_g = other.m_g.clone();
    return *this;
  }

  node_graph& node_graph::operator=(node_graph&& other) noexcept
  {
    auto lck1 = _lock();
    auto lck2 = other._lock();

    m_g = std::move(other.m_g);
    return *this;
  }

  bool node_graph::_exists(const node_handle& h) const
  {
    return h.has_value() && m_g.exists(h.descriptor()) &&
           h.id() == uid {m_g.id(h.descriptor())};
  }

  bool node_graph::exists(const node_handle& h) const
  {
    auto lck = _lock();
    return _exists(h);
  }

  bool node_graph::_exists(const connection_handle& h) const
  {
    return h.has_value() && m_g.exists(h.descriptor()) &&
           h.id() == uid {m_g.id(h.descriptor())};
  }

  bool node_graph::exists(const connection_handle& h) const
  {
    auto lck = _lock();
    return _exists(h);
  }

  auto node_graph::add(const yave::node_info& info, const primitive_t& prim)
    -> node_handle
  {
    return add_with_id(info, uid::random_generate(), prim);
  }

  auto node_graph::add_with_id(
    const yave::node_info& info,
    const uid& id,
    const primitive_t& prim) -> node_handle
  {
    auto lck = _lock();

    // add node
    auto node = [&]() {
      if (info.is_prim())
        return m_g.add_node_with_id(id.data, info.name(), prim);
      else
        return m_g.add_node_with_id(id.data, info.name(), std::monostate {});
    }();

    if (!node)
      throw std::runtime_error(
        "Failed to add node to node_graph: Failed to create new node.");

    auto _attach_sockets = [&](auto&& names, auto io) {
      for (auto&& name : names) {
        auto socket = m_g.add_socket(name, io);
        if (!m_g.attach_socket(node, socket)) {
          throw std::runtime_error(
            "Failed to add node to node_graph: Failed to attach sockets.");
        }
      }
    };

    try {
      _attach_sockets(info.input_sockets(), socket_property::input);
      _attach_sockets(info.output_sockets(), socket_property::output);
    } catch (...) {
      for (auto&& s : m_g.sockets(node)) {
        m_g.remove_socket(s);
      }
      m_g.remove_node(node);
      throw;
    }

    auto handle = node_handle(node, {m_g.id(node)});

    Info(
      g_logger,
      "Created Node: name=\"{}\", id={}",
      info.name(),
      to_string(handle.id()));

    return handle;
  }

  void node_graph::remove(const node_handle& node)
  {
    auto lck = _lock();

    if (!_exists(node))
      return;

    Info(
      g_logger,
      "Removing Node: name=\"{}\", id={}",
      m_g[node.descriptor()].name(),
      to_string(node.id()));

    for (auto&& s : m_g.sockets(node.descriptor())) {
      m_g.remove_socket(s);
    }

    m_g.remove_node(node.descriptor());
  }

  auto node_graph::connect(
    const node_handle& src_node,
    const std::string& src_socket,
    const node_handle& dst_node,
    const std::string& dst_socket) -> connection_handle
  {
    return connect(
      yave::connection_info {src_node, src_socket, dst_node, dst_socket});
  }

  auto node_graph::connect(const connection_info& info) -> connection_handle
  {
    auto lck = _lock();

    // check handler
    if (!_exists(info.src_node()) || !_exists(info.dst_node())) {
      Error(g_logger, "Failed to connect sockets: Invalid node descriptor.");
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
                  g_logger,
                  "Connection already exists. Return existing connection "
                  "handle.");
                return connection_handle(e, {m_g.id(e)});
              }
            // input edge cannot have multiple inputs
            if (m_g.n_dst_edges(d) != 0) {
              Error(
                g_logger,
                "Failed to connect: src='{}'({})::{}, dst='{}'({})::{}",
                m_g[info.src_node().descriptor()].name(),
                to_string(info.src_node().id()),
                info.src_socket(),
                m_g[info.dst_node().descriptor()].name(),
                to_string(info.dst_node().id()),
                info.dst_socket());
              Error(g_logger, "Multiple input is not allowed.");
              return nullptr;
            }

            // add new edge to graph
            auto new_edge = m_g.add_edge(s, d);
            assert(new_edge);

            // closed loop check
            if (!_find_loop(info.src_node()).empty()) {
              Error(
                g_logger,
                "Failed to connect: src='{}'({})::{}, dst='{}'({})::{}",
                m_g[info.src_node().descriptor()].name(),
                to_string(info.src_node().id()),
                info.src_socket(),
                m_g[info.dst_node().descriptor()].name(),
                to_string(info.dst_node().id()),
                info.dst_socket());
              Error(g_logger, "Closed loop is not allowed.");
              m_g.remove_edge(new_edge);
              return nullptr;
            }

            Info(
              g_logger,
              "Connected socket: src=\"{}\"({})::{}, dst=\"{}\"({})::{}",
              m_g[info.src_node().descriptor()].name(),
              to_string(info.src_node().id()),
              info.src_socket(),
              m_g[info.dst_node().descriptor()].name(),
              to_string(info.dst_node().id()),
              info.dst_socket());

            return connection_handle(new_edge, {m_g.id(new_edge)});
          }
        }
      }
    }

    // fail
    Error(g_logger, "Failed to connect sockets: Invalid argument.");
    return nullptr;
  }

  void node_graph::disconnect(const connection_handle& h)
  {
    auto lck = _lock();

    // check handler
    if (!_exists(h)) {
      Warning(
        g_logger,
        "node_graph::disconnect() on invalid node handle: id={}",
        to_string(h.id()));
      return;
    }

    auto info = _get_info(h);
    Info(
      g_logger,
      "Disconnecting: src='{}'({})::{} dst='{}'({})::{}",
      m_g[info->src_node().descriptor()].name(),
      to_string(info->src_node().id()),
      info->src_socket(),
      m_g[info->dst_node().descriptor()].name(),
      to_string(info->dst_node().id()),
      info->dst_socket());

    // remove edge
    m_g.remove_edge(h.descriptor());
  }

  auto node_graph::node(const uid& id) const -> node_handle
  {
    auto lck = _lock();

    auto dsc = m_g.node(id.data);

    if (!dsc)
      return nullptr;

    return node_handle(dsc, id);
  }

  auto node_graph::nodes(const std::string& name) const
    -> std::vector<node_handle>
  {
    auto lck = _lock();

    std::vector<node_handle> ret;
    for (auto&& n : m_g.nodes())
      if (m_g[n].name() == name)
        ret.emplace_back(n, uid {m_g.id(n)});
    return ret;
  }

  auto node_graph::nodes() const -> std::vector<node_handle>
  {
    auto lck = _lock();

    std::vector<node_handle> ret;
    for (auto&& n : m_g.nodes()) ret.emplace_back(n, uid {m_g.id(n)});
    return ret;
  }

  auto node_graph::_get_info(const node_handle& h) const
    -> std::optional<node_info>
  {
    // check handler
    if (!_exists(h)) {
      Warning(g_logger, "node_graph::node_info() on invalid node handle.");
      return std::nullopt;
    }

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

    auto ret = node_info(n.name(), input_sockets, output_sockets, n.is_prim());

    return ret;
  }

  auto node_graph::get_info(const node_handle& h) const
    -> std::optional<node_info>
  {
    auto lck = _lock();
    return _get_info(h);
  }

  auto node_graph::get_name(const node_handle& node) const
    -> std::optional<std::string>
  {
    auto lck = _lock();

    if (!_exists(node)) {
      Warning(g_logger, "NodeGrpah::node_name() on invalid node handle.");
      return std::nullopt;
    }
    return m_g[node.descriptor()].name();
  }

  bool node_graph::has_socket(const node_handle& h, const std::string& socket)
    const
  {
    auto lck = _lock();

    if (!_exists(h)) {
      Warning(g_logger, "node_graph::has_socket() on invalid node handle.");
      return false;
    }
    auto sockets = m_g.sockets(h.descriptor());
    for (auto&& s : sockets)
      if (m_g[s].name() == socket)
        return true;
    return false;
  }

  bool node_graph::is_input_socket(
    const node_handle& h,
    const std::string& socket) const
  {
    auto lck = _lock();

    if (!_exists(h)) {
      Warning(g_logger, "node_graph::is_input_socket on invalid node handle.");
      return false;
    }
    auto sockets = m_g.sockets(h.descriptor());
    for (auto&& s : sockets) {
      if (m_g[s].name() == socket && m_g[s].is_input())
        return true;
    }
    return false;
  }

  bool node_graph::is_output_socket(
    const node_handle& h,
    const std::string& socket) const
  {
    auto lck = _lock();

    if (!_exists(h)) {
      Warning(g_logger, "node_graph::is_output_socket on invalid node handle.");
      return false;
    }
    auto sockets = m_g.sockets(h.descriptor());
    for (auto&& s : sockets) {
      if (m_g[s].name() == socket && m_g[s].is_output())
        return true;
    }
    return false;
  }

  auto node_graph::input_connections() const -> std::vector<connection_handle>
  {
    auto lck = _lock();

    std::vector<connection_handle> ret;
    for (auto&& node : m_g.nodes()) {
      for (auto&& socket : m_g.sockets(node)) {
        if (m_g[socket].is_input()) {
          assert(m_g.src_edges(socket).empty());
          for (auto&& edge : m_g.dst_edges(socket)) {
            ret.emplace_back(edge, uid {m_g.id(edge)});
          }
        }
      }
    }
    return ret;
  }

  auto node_graph::output_connections() const -> std::vector<connection_handle>
  {
    auto lck = _lock();

    std::vector<connection_handle> ret;
    for (auto&& node : m_g.nodes()) {
      for (auto&& socket : m_g.sockets(node)) {
        if (m_g[socket].is_output()) {
          assert(m_g.dst_edges(socket).empty());
          for (auto&& edge : m_g.src_edges(socket)) {
            ret.emplace_back(edge, uid {m_g.id(edge)});
          }
        }
      }
    }
    return ret;
  }

  auto node_graph::input_connections(
    const node_handle& h,
    const std::string& socket) const -> std::vector<connection_handle>
  {
    auto lck = _lock();

    if (!_exists(h)) {
      Warning(
        g_logger, "node_graph::input_connections on invalid node handle.");
      return {};
    }

    std::vector<connection_handle> ret;
    auto sockets = m_g.sockets(h.descriptor());
    for (auto&& s : sockets) {
      if (m_g[s].name() == socket) {
        auto dst_edges = m_g.dst_edges(s);
        if (m_g[s].is_output())
          assert(dst_edges.empty());
        for (auto&& e : dst_edges) ret.emplace_back(e, uid {m_g.id(e)});
      }
    }
    return ret;
  }

  auto node_graph::output_connections(
    const node_handle& h,
    const std::string& socket) const -> std::vector<connection_handle>
  {
    auto lck = _lock();

    if (!_exists(h)) {
      Warning(
        g_logger, "node_graph::output_connections on invalid node handle.");
      return {};
    }

    std::vector<connection_handle> ret;
    auto sockets = m_g.sockets(h.descriptor());
    for (auto&& s : sockets) {
      if (m_g[s].name() == socket) {
        auto src_edges = m_g.src_edges(s);
        if (m_g[s].is_input())
          assert(src_edges.empty());
        for (auto&& e : src_edges) ret.emplace_back(e, uid {m_g.id(e)});
      }
    }
    return ret;
  }

  bool node_graph::has_connection(const node_handle& h, const std::string& s)
    const
  {
    auto lck = _lock();

    if (!_exists(h)) {
      Warning(g_logger, "node_graph::has_connection() on invalid node handle.");
      return false;
    }

    auto sockets = m_g.sockets(h.descriptor());

    for (auto&& socket : sockets) {
      if (m_g[socket].name() == s) {
        if (m_g.n_src_edges(socket) != 0)
          return true;
        if (m_g.n_dst_edges(socket) != 0)
          return true;
        return false;
      }
    }
    return false;
  }

  auto node_graph::connections(const node_handle& h) const
    -> std::vector<connection_handle>
  {
    auto lck = _lock();

    if (!_exists(h)) {
      Warning(g_logger, "node_graph::connections() on invalid node handle.");
      return {};
    }

    std::vector<connection_handle> ret;
    auto sockets = m_g.sockets(h.descriptor());
    for (auto&& s : sockets) {
      auto src_edges = m_g.src_edges(s);
      auto dst_edges = m_g.dst_edges(s);
      assert(
        (m_g[s].is_input() && src_edges.empty()) ||
        (m_g[s].is_output() && dst_edges.empty()));
      for (auto&& e : src_edges) ret.emplace_back(e, uid {m_g.id(e)});
      for (auto&& e : dst_edges) ret.emplace_back(e, uid {m_g.id(e)});
    }
    return ret;
  }

  auto node_graph::connections(const node_handle& h, const std::string& socket)
    const -> std::vector<connection_handle>
  {
    auto lck = _lock();

    if (!_exists(h)) {
      Warning(g_logger, "node_graph::connections() on invalid node handle.");
      return {};
    }

    std::vector<connection_handle> ret;
    auto sockets = m_g.sockets(h.descriptor());
    for (auto&& s : sockets) {
      if (m_g[s].name() == socket) {
        auto src_edges = m_g.src_edges(s);
        auto dst_edges = m_g.dst_edges(s);
        for (auto&& e : src_edges) ret.emplace_back(e, uid {m_g.id(e)});
        for (auto&& e : dst_edges) ret.emplace_back(e, uid {m_g.id(e)});
      }
    }
    if (ret.empty())
      Warning(g_logger, "Call for connections() with invalid socket name.");
    return ret;
  }

  auto node_graph::input_connections(const node_handle& h) const
    -> std::vector<connection_handle>
  {
    auto lck = _lock();

    if (!_exists(h)) {
      Warning(
        g_logger, "node_graph::input_connections() on invalid node handle.");
      return {};
    }

    std::vector<connection_handle> ret;
    auto sockets = m_g.sockets(h.descriptor());
    for (auto&& s : sockets) {
      auto dst_edges = m_g.dst_edges(s);
      assert(m_g[s].is_input() || (m_g[s].is_output() && dst_edges.empty()));
      for (auto&& e : dst_edges) {
        ret.emplace_back(e, uid {m_g.id(e)});
      }
    }
    return ret;
  }

  auto node_graph::output_connections(const node_handle& h) const
    -> std::vector<connection_handle>
  {
    auto lck = _lock();

    if (!_exists(h)) {
      Warning(
        g_logger, "node_graph::outupt_connections() on invalid node handle.");
      return {};
    }

    std::vector<connection_handle> ret;
    auto sockets = m_g.sockets(h.descriptor());
    for (auto&& s : sockets) {
      auto src_edges = m_g.src_edges(s);
      assert((m_g[s].is_input() && src_edges.empty()) || m_g[s].is_output());
      for (auto&& e : src_edges) ret.emplace_back(e, uid {m_g.id(e)});
    }
    if (ret.empty())
      Warning(
        g_logger, "Call for output_connections() with invalid socket name.");
    return ret;
  }

  auto node_graph::connection(const uid& id) const -> connection_handle
  {
    auto lck = _lock();

    auto dsc = m_g.edge(id.data);

    if (!dsc)
      return nullptr;

    return connection_handle(dsc, id);
  }

  auto node_graph::connections() const -> std::vector<connection_handle>
  {
    auto lck = _lock();

    std::vector<connection_handle> ret;
    for (auto&& n : m_g.nodes()) {
      for (auto&& s : m_g.sockets(n)) {
        for (auto&& e : m_g.src_edges(s)) ret.emplace_back(e, uid {m_g.id(e)});
      }
    }
    return ret;
  }

  auto node_graph::_get_info(const connection_handle& h) const
    -> std::optional<connection_info>
  {
    if (!_exists(h)) {
      Warning(
        g_logger,
        "node_graph::connection_info() on invalid connection handle.");
      return std::nullopt;
    }

    auto src = m_g.src(h.descriptor());
    auto dst = m_g.dst(h.descriptor());

    auto src_nodes = m_g.nodes(src);
    auto dst_nodes = m_g.nodes(dst);
    assert(src_nodes.size() == 1);
    assert(dst_nodes.size() == 1);
    return connection_info {
      node_handle(src_nodes[0], {m_g.id(src_nodes[0])}), //
      m_g[src].name(),                                   //
      node_handle(dst_nodes[0], {m_g.id(dst_nodes[0])}), //
      m_g[dst].name()};                                  //
  }

  auto node_graph::get_info(const connection_handle& h) const
    -> std::optional<connection_info>
  {
    auto lck = _lock();
    return _get_info(h);
  }

  auto node_graph::input_sockets(const node_handle& h) const
    -> std::vector<std::string>
  {
    auto lck = _lock();

    if (!_exists(h)) {
      Warning(g_logger, "node_graph::input_sockets() on invalid node handle.");
      return {};
    }

    auto&& sockets = m_g.sockets(h.descriptor());
    std::vector<std::string> ret;
    for (auto&& s : sockets) {
      if (m_g[s].is_input())
        ret.push_back(m_g[s].name());
    }
    return ret;
  }

  auto node_graph::output_sockets(const node_handle& h) const
    -> std::vector<std::string>
  {
    auto lck = _lock();

    if (!_exists(h)) {
      Warning(g_logger, "node_graph::outupt_sockets() on invalid node handle.");
      return {};
    }

    auto&& sockets = m_g.sockets(h.descriptor());
    std::vector<std::string> ret;
    for (auto&& s : sockets) {
      if (m_g[s].is_output())
        ret.push_back(m_g[s].name());
    }
    return ret;
  }

  bool node_graph::is_primitive(const node_handle& h) const
  {
    auto lck = _lock();

    if (!_exists(h)) {
      Warning(g_logger, "node_graph::is_primitive() on invalid node handle.");
      return false;
    }

    return m_g[h.descriptor()].is_prim();
  }

  auto node_graph::get_primitive(const node_handle& h) const
    -> std::optional<primitive_t>
  {
    auto lck = _lock();

    if (!_exists(h)) {
      Warning(g_logger, "node_graph::get_primitive() on invalid node handle.");
      return std::nullopt;
    }

    if (m_g[h.descriptor()].is_prim())
      return m_g[h.descriptor()].get_prim();
    return std::nullopt;
  }

  void node_graph::set_primitive(const node_handle& h, const primitive_t& prim)
  {
    auto lck = _lock();

    if (!_exists(h)) {
      Warning(g_logger, "node_graph::set_primitive() on invalid node handle.");
      return;
    }

    if (!m_g[h.descriptor()].is_prim()) {
      Error(
        g_logger, "Cannot set primitive value to non-primitive node. Ignored.");
      return;
    }
    m_g[h.descriptor()].set_prim(prim);
  }

  auto node_graph::get_primitive_container(const node_handle& node) const
    -> object_ptr<const PrimitiveContainer>
  {
    auto lck = _lock();

    assert(m_default_container);

    if (!_exists(node)) {
      Warning(
        g_logger,
        "node_graph::get_primitive_container() on invalid node handle.");
      return m_default_container;
    }

    if (m_g[node.descriptor()].is_prim())
      return m_g[node.descriptor()].get_shared_prim();

    return m_default_container;
  }

  auto node_graph::roots() const -> std::vector<node_handle>
  {
    auto lck = _lock();

    std::vector<node_handle> ret;

    // TODO: Improve performance.
    for (auto&& n : m_g.nodes()) {
      for (auto&& root : _root_of(node_handle(n, {m_g.id(n)}))) {
        [&] {
          for (auto&& r : ret) {
            if (root == r)
              return;
          }
          ret.push_back(root);
        }();
      }
    }

    return ret;
  }

  auto node_graph::_root_of(const node_handle& node) const
    -> std::vector<node_handle>
  {
    if (!_exists(node)) {
      Warning(g_logger, "node_graph::root_of() on invalid node handle.");
      return {};
    }

    std::vector<node_handle> ret;
    std::vector<node_handle> stack;

    stack.push_back(node);

    while (!stack.empty()) {
      // pop
      auto n = stack.back();
      stack.pop_back();
      // current stack size
      auto size = stack.size();
      // push parent
      for (auto&& s : m_g.sockets(n.descriptor())) {
        for (auto&& e : m_g.src_edges(s)) {
          auto dst_s = m_g.dst(e);
          auto dst_n = m_g.nodes(dst_s);
          assert(dst_n.size() == 1);
          stack.emplace_back(dst_n[0], uid {m_g.id(dst_n[0])});
        }
      }
      // node is root
      if (size == stack.size()) {
        // avoid duplicate
        [&] {
          for (auto&& r : ret) {
            if (r == n)
              return;
          }
          ret.push_back(n);
        }();
      }
    }
    return ret;
  }

  auto node_graph::root_of(const node_handle& node) const
    -> std::vector<node_handle>
  {
    auto lck = _lock();
    return _root_of(node);
  }

  void node_graph::clear()
  {
    auto lck = _lock();
    m_g.clear();
  }

  auto node_graph::_find_loop(const node_handle& node) const
    -> std::vector<node_handle>
  {
    if (!_exists(node))
      return {};

    std::vector<node_handle> ret;

    _depth_first_search_until(
      node,
      [](auto&&, auto&&) { return false; },
      [&ret](const node_handle& n, const std::vector<node_handle>& pth) {
        // find closed loop
        for (auto iter = pth.begin(); iter != pth.end(); ++iter) {
          if (*iter == n) {
            ret = {iter, pth.end()};
            return true;
          }
        }
        return false;
      });

    return ret;
  }

  auto node_graph::_lock() const -> std::unique_lock<std::mutex>
  {
    return std::unique_lock(m_mtx);
  }

} // namespace yave