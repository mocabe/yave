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
  }

  node_graph::~node_graph() noexcept
  {
  }

  node_graph::node_graph(const node_graph& other)
    : m_mtx {}
  {
    auto lck = other._lock();
    m_g      = other.m_g.clone();
  }

  node_graph::node_graph(node_graph&& other) noexcept
    : m_mtx {}
  {
    auto lck = other._lock();
    m_g      = std::move(other.m_g);
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

  node_handle
    node_graph::add(const yave::node_info& info, const primitive_t& prim)
  {
    auto lck = _lock();

    // add node
    auto node = [&]() {
      if (info.is_prim())
        return m_g.add_node(info.name(), prim);
      else
        return m_g.add_node(info.name(), std::monostate {});
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

  connection_handle node_graph::connect(
    const node_handle& src_node,
    const std::string& src_socket,
    const node_handle& dst_node,
    const std::string& dst_socket)
  {
    return connect(
      yave::connection_info {src_node, src_socket, dst_node, dst_socket});
  }

  connection_handle node_graph::connect(const connection_info& info)
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
        h.id().data);
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

  std::vector<node_handle> node_graph::nodes(const std::string& name) const
  {
    auto lck = _lock();

    std::vector<node_handle> ret;
    for (auto&& n : m_g.nodes())
      if (m_g[n].name() == name)
        ret.emplace_back(n, uid {m_g.id(n)});
    return ret;
  }

  std::vector<node_handle> node_graph::nodes() const
  {
    auto lck = _lock();

    std::vector<node_handle> ret;
    for (auto&& n : m_g.nodes()) ret.emplace_back(n, uid {m_g.id(n)});
    return ret;
  }

  std::optional<node_info> node_graph::_get_info(const node_handle& h) const
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

  std::optional<node_info> node_graph::get_info(const node_handle& h) const
  {
    auto lck = _lock();
    return _get_info(h);
  }

  std::optional<std::string> node_graph::get_name(const node_handle& node) const
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

  std::vector<connection_handle> node_graph::input_connections() const
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

  std::vector<connection_handle> node_graph::output_connections() const
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

  std::vector<connection_handle> node_graph::input_connections(
    const node_handle& h,
    const std::string& socket) const
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

  std::vector<connection_handle> node_graph::output_connections(
    const node_handle& h,
    const std::string& socket) const
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

  bool
    node_graph::has_connection(const node_handle& h, const std::string& s) const
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

  std::vector<connection_handle>
    node_graph::connections(const node_handle& h) const
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

  std::vector<connection_handle> node_graph::connections(
    const node_handle& h,
    const std::string& socket) const
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

  std::vector<connection_handle>
    node_graph::input_connections(const node_handle& h) const
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

  std::vector<connection_handle>
    node_graph::output_connections(const node_handle& h) const
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

  std::vector<connection_handle> node_graph::connections() const
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

  std::optional<connection_info>
    node_graph::_get_info(const connection_handle& h) const
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

  std::optional<connection_info>
    node_graph::get_info(const connection_handle& h) const
  {
    auto lck = _lock();
    return _get_info(h);
  }

  std::vector<std::string> node_graph::input_sockets(const node_handle& h) const
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

  std::vector<std::string>
    node_graph::output_sockets(const node_handle& h) const
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

  std::optional<primitive_t>
    node_graph::get_primitive(const node_handle& h) const
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

  object_ptr<PrimitiveContainer>
    node_graph::get_primitive_container(const node_handle& node) const
  {
    auto lck = _lock();

    if (!_exists(node)) {
      Warning(
        g_logger,
        "node_graph::get_primitive_container() on invalid node handle.");
      return nullptr;
    }

    if (m_g[node.descriptor()].is_prim())
      return m_g[node.descriptor()].get_shared_prim();
    return nullptr;
  }

  std::vector<node_handle> node_graph::roots() const
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

  std::vector<node_handle> node_graph::_root_of(const node_handle& node) const
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

  std::vector<node_handle> node_graph::root_of(const node_handle& node) const
  {
    auto lck = _lock();
    return _root_of(node);
  }

  void node_graph::clear()
  {
    auto lck = _lock();
    m_g.clear();
  }

  std::vector<node_handle> node_graph::_find_loop(const node_handle& node) const
  {
    if (!_exists(node))
      return {};

    for (auto&& n : m_g.nodes()) {
      assert(m_g[n].is_unvisited());
    }

    std::vector<node_handle> stack;

    auto visit = [&](const node_handle& n) {
      m_g[n.descriptor()].set_visited();
      stack.push_back(n);
    };

    auto visited = [&](const node_handle& n) {
      return m_g[n.descriptor()].is_visited();
    };

    // loop
    std::vector<node_handle> ret;

    visit(node);

    while (!stack.empty()) {

      auto current = stack.back();

      bool stop = [&] {
        std::vector<connection_handle> inputs;
        for (auto&& s : m_g.sockets(current.descriptor())) {
          for (auto&& e : m_g.dst_edges(s)) {
            inputs.emplace_back(e, uid {m_g.id(e)});
          }
        }

        for (auto&& c : inputs) {
          auto next = _get_info(c)->src_node();
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

    // clear mark
    for (auto&& n : m_g.nodes()) {
      m_g[n].set_unvisited();
    }

    return ret;
  }

  std::unique_lock<std::mutex> node_graph::_lock() const
  {
    return std::unique_lock(m_mtx);
  }

  namespace {

    template <class T, class Diff>
    Diff diff_impl(const std::vector<T>& prev, const std::vector<T>& next)
    {
      auto l = prev;
      auto r = next;

      std::sort(l.begin(), l.end());
      std::sort(r.begin(), r.end());

      auto li = l.begin();
      auto ri = r.begin();

      auto le = l.end();
      auto re = r.end();

      Diff ret;

      for (;;) {
        if (li == le || ri == re) {
          for (auto i = li; i != le; ++i) ret.removed.push_back(*i);
          for (auto i = ri; i != re; ++i) ret.added.push_back(*i);
          break;
        }

        if (*li == *ri) {
          ret.not_changed.push_back(*ri);
          ++li;
          ++ri;
          continue;
        }

        if (*li < *ri) {
          ret.removed.push_back(*li);
          ++li;
          continue;
        }

        if (*li > *ri) {
          ret.added.push_back(*ri);
          ++ri;
          continue;
        }
      }
      return ret;
    }
  } // namespace

  NodesDiff nodes_diff(
    const std::vector<node_handle>& prev_nodes,
    const std::vector<node_handle>& nodes)
  {
    return diff_impl<node_handle, NodesDiff>(prev_nodes, nodes);
  }

  ConnectionsDiff connections_diff(
    const std::vector<connection_handle>& prev_connections,
    const std::vector<connection_handle>& connections)
  {
    return diff_impl<connection_handle, ConnectionsDiff>(
      prev_connections, connections);
  }

} // namespace yave