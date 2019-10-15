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

  bool node_graph::_exists(const socket_handle& h) const
  {
    return h.has_value() && m_g.exists(h.descriptor()) &&
           h.id() == uid {m_g.id(h.descriptor())};
  }

  bool node_graph::exists(const socket_handle& h) const
  {
    auto lck = _lock();
    return _exists(h);
  }

  auto node_graph::_get_info(const node_handle& h) const -> node_info
  {
    assert(_exists(h));

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

    auto ret = node_info(n.name(), input_sockets, output_sockets, n.get_type());

    return ret;
  }

  auto node_graph::get_info(const node_handle& h) const
    -> std::optional<node_info>
  {
    auto lck = _lock();
    if (!_exists(h))
      return std::nullopt;
    return _get_info(h);
  }

  auto node_graph::_get_info(const socket_handle& h) const -> socket_info
  {
    assert(_exists(h));
    assert(!m_g.nodes(h.descriptor()).empty());

    const auto& s = m_g[h.descriptor()];
    const auto& n = m_g.nodes(h.descriptor())[0];

    return socket_info(s.name(), s.get_type(), node_handle {n, {m_g.id(n)}});
  }

  auto node_graph::get_info(const socket_handle& h) const
    -> std::optional<socket_info>
  {
    auto lck = _lock();
    if (!_exists(h))
      return std::nullopt;
    return _get_info(h);
  }

  auto node_graph::_get_info(const connection_handle& h) const
    -> connection_info
  {
    assert(_exists(h));

    auto src = m_g.src(h.descriptor());
    auto dst = m_g.dst(h.descriptor());

    auto src_nodes = m_g.nodes(src);
    auto dst_nodes = m_g.nodes(dst);

    assert(src_nodes.size() >= 1);
    assert(dst_nodes.size() >= 1);

    assert(m_g[src_nodes[0]].is_normal());
    assert(m_g[dst_nodes[0]].is_normal());

    std::vector<node_handle> src_interfaces;
    std::vector<node_handle> dst_interfaces;

    for (size_t i = 1; i < src_nodes.size(); ++i) {
      assert(m_g[src_nodes[i]].is_interface());
      src_interfaces.emplace_back(src_nodes[i], uid {m_g.id(src_nodes[i])});
    }

    for (size_t i = 1; i < dst_nodes.size(); ++i) {
      assert(m_g[dst_nodes[i]].is_interface());
      dst_interfaces.emplace_back(dst_nodes[i], uid {m_g.id(dst_nodes[i])});
    }

    return connection_info {node_handle(src_nodes[0], {m_g.id(src_nodes[0])}),
                            socket_handle(src, {m_g.id(src)}),
                            node_handle(dst_nodes[0], {m_g.id(dst_nodes[0])}),
                            socket_handle(dst, {m_g.id(dst)}),
                            src_interfaces,
                            dst_interfaces};
  }

  auto node_graph::get_info(const connection_handle& h) const
    -> std::optional<connection_info>
  {
    auto lck = _lock();
    if (!_exists(h))
      return std::nullopt;
    return _get_info(h);
  }

  auto node_graph::_get_name(const node_handle& node) const -> std::string
  {
    assert(_exists(node));
    return m_g[node.descriptor()].name();
  }

  auto node_graph::get_name(const node_handle& node) const
    -> std::optional<std::string>
  {
    auto lck = _lock();
    if (!_exists(node))
      return std::nullopt;
    return _get_name(node);
  }

  auto node_graph::_get_name(const socket_handle& socket) const -> std::string
  {
    assert(_exists(socket));
    return m_g[socket.descriptor()].name();
  }

  auto node_graph::get_name(const socket_handle& socket) const
    -> std::optional<std::string>
  {
    auto lck = _lock();
    if (!_exists(socket))
      return std::nullopt;
    return _get_name(socket);
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
      switch (info.type()) {

        case node_type::normal:
          return m_g.add_node_with_id(
            id.data, node_property::normal_construct_t {}, info.name());

        case node_type::primitive:
          return m_g.add_node_with_id(
            id.data,
            node_property::primitive_construct_t {},
            info.name(),
            prim);

        case node_type::interface:
          if (!info.input_sockets().empty() || !info.output_sockets().empty()) {
            throw std::runtime_error(
              "Failed to create node: Interface node cannot have sockets");
          }
          return m_g.add_node_with_id(
            id.data, node_property::interface_construct_t {}, info.name());

        default:
          throw std::runtime_error("Invalid node type");
      }
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
      _attach_sockets(info.input_sockets(), socket_type::input);
      _attach_sockets(info.output_sockets(), socket_type::output);
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

  bool node_graph::attach_interface(
    const node_handle& interface,
    const socket_handle& socket)
  {
    auto lck = _lock();

    if (!_exists(interface) || !_exists(socket))
      return false;

    if (!m_g[interface.descriptor()].is_interface()) {
      Error(g_logger, "attach_interface(): Not interface node");
      return false;
    }

    auto info = _get_info(socket);

    for (auto&& s : m_g.sockets(interface.descriptor())) {
      if (socket.id().data == m_g.id(s)) {
        Warning(g_logger, "attach_interface(): already attached");
        return true; // already attached
      }
    }

    if (!m_g.attach_socket(interface.descriptor(), socket.descriptor())) {
      Error(g_logger, "attach_interface(): Failed to attach socket!");
      return false;
    }

    Info(
      g_logger,
      "Attached interface: node={}({}), socket={}({})",
      _get_name(info.node()),
      to_string(info.node().id()),
      info.name(),
      to_string(socket.id()));

    return true;
  }

  void node_graph::detach_interface(
    const node_handle& interface,
    const socket_handle& socket)
  {
    auto lck = _lock();

    if (!_exists(interface) || !_exists(socket))
      return;

    auto info = _get_info(socket);

    for (auto&& s : m_g.sockets(interface.descriptor())) {

      if (socket.id().data == m_g.id(s)) {

        // detach
        m_g.detach_socket(interface.descriptor(), s);

        Info(
          g_logger,
          "Detached interface: node={}({}), socket={}({})",
          _get_name(info.node()),
          to_string(info.node().id()),
          info.name(),
          to_string(socket.id()));
        return;
      }
    }
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
      // detach
      m_g.detach_socket(node.descriptor(), s);
      // delete socket if needed
      for (auto&& n : m_g.nodes(s)) {
        if (!m_g[n].is_interface())
          continue;
      }
      m_g.remove_socket(s);
    }

    m_g.remove_node(node.descriptor());
  }

  auto node_graph::_connect(
    const node_handle& src_node,
    const socket_handle& src_socket,
    const node_handle& dst_node,
    const socket_handle& dst_socket) -> connection_handle
  {
    // socket descriptors
    auto s = src_socket.descriptor();
    auto d = dst_socket.descriptor();

    // check socket type
    if (!m_g[s].is_input() || !m_g[d].is_output()) {
      Error(g_logger, "Failed to connect sockets: Invalid socket type");
      return nullptr;
    }

    // already exists
    for (auto&& e : m_g.dst_edges(d)) {
      if (m_g.src(e) == s) {
        Warning(
          g_logger,
          "Connection already exists. Return existing connection "
          "handle.");
        return connection_handle(e, {m_g.id(e)});
      }
    }

    // input edge cannot have multiple inputs
    if (m_g.n_dst_edges(d) != 0) {
      Error(
        g_logger,
        "Failed to connect: src='{}'({})::{}, dst='{}'({})::{}",
        _get_name(src_node),
        to_string(src_node.id()),
        _get_name(src_socket),
        _get_name(dst_node),
        to_string(dst_node.id()),
        _get_name(dst_socket));
      Error(g_logger, "Multiple input is not allowed");
      return nullptr;
    }

    // add new edge to graph
    auto new_edge = m_g.add_edge(s, d);
    assert(new_edge);

    // closed loop check
    if (!_find_loop(src_node).empty()) {
      Error(
        g_logger,
        "Failed to connect: src='{}'({})::{}, dst='{}'({})::{}",
        _get_name(src_node),
        to_string(src_node.id()),
        _get_name(src_socket),
        _get_name(dst_node),
        to_string(dst_node.id()),
        _get_name(dst_socket));
      Error(g_logger, "Closed loop is not allowed");
      m_g.remove_edge(new_edge);
      return nullptr;
    }

    Info(
      g_logger,
      "Connected socket: src=\"{}\"({})::{}, dst=\"{}\"({})::{}",
      _get_name(src_node),
      to_string(src_node.id()),
      _get_name(src_socket),
      _get_name(dst_node),
      to_string(dst_node.id()),
      _get_name(dst_socket));

    return connection_handle(new_edge, {m_g.id(new_edge)});
  }

  auto node_graph::connect(
    const socket_handle& src_socket,
    const socket_handle& dst_socket) -> connection_handle
  {
    auto lck = _lock();

    if (!_exists(src_socket) || !_exists(dst_socket)) {
      Error(g_logger, "Failed to connect sockets: Invalid socket descriptor");
      return nullptr;
    }

    auto s  = src_socket.descriptor();
    auto d  = dst_socket.descriptor();
    auto sn = m_g.nodes(s)[0];
    auto dn = m_g.nodes(d)[0];

    // unchecked connect
    return _connect(
      node_handle(sn, {m_g.id(sn)}),
      src_socket,
      node_handle(dn, {m_g.id(dn)}),
      dst_socket);
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
      _get_name(info.src_node()),
      to_string(info.src_node().id()),
      _get_name(info.src_socket()),
      _get_name(info.dst_node()),
      to_string(info.dst_node().id()),
      _get_name(info.dst_socket()));

    // remove edge
    m_g.remove_edge(h.descriptor());
  }

  auto node_graph::node(const uid& id) const -> node_handle
  {
    auto lck = _lock();

    auto dsc = m_g.node(id.data);

    if (!dsc)
      return {nullptr};

    return node_handle(dsc, id);
  }

  auto node_graph::socket(const uid& id) const -> socket_handle
  {
    auto lck = _lock();
    auto dsc = m_g.socket(id.data);

    if (!dsc)
      return {nullptr};

    return socket_handle(dsc, id);
  }

  auto node_graph::connection(const uid& id) const -> connection_handle
  {
    auto lck = _lock();

    auto dsc = m_g.edge(id.data);

    if (!dsc)
      return {nullptr};

    return connection_handle(dsc, id);
  }

  auto node_graph::nodes() const -> std::vector<node_handle>
  {
    auto lck = _lock();

    std::vector<node_handle> ret;
    for (auto&& n : m_g.nodes())
      ret.emplace_back(n, uid {m_g.id(n)});
    return ret;
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

  auto node_graph::sockets() const -> std::vector<socket_handle>
  {
    auto lck = _lock();

    std::vector<socket_handle> ret;
    for (auto&& s : m_g.sockets()) 
      ret.emplace_back(s, uid {m_g.id(s)});
    return ret;
  }

  auto node_graph::sockets(const node_handle& node) const
    -> std::vector<socket_handle>
  {
    auto lck = _lock();

    if (!_exists(node))
      return {};

    std::vector<socket_handle> ret;
    for (auto&& s : m_g.sockets(node.descriptor())) 
      ret.emplace_back(s, uid {m_g.id(s)});
    return ret;
  }

  auto node_graph::connections() const -> std::vector<connection_handle>
  {
    auto lck = _lock();

    std::vector<connection_handle> ret;
    for (auto&& e : m_g.edges()) 
      ret.emplace_back(e, uid {m_g.id(e)});
    return ret;
  }

  auto node_graph::connections(const node_handle& node) const
    -> std::vector<connection_handle>
  {
    auto lck = _lock();

    if (!_exists(node))
      return {};

    std::vector<connection_handle> ret;
    for (auto&& s : m_g.sockets(node.descriptor())) {
      for (auto&& e : m_g.src_edges(s)) {
        ret.emplace_back(e, uid {m_g.id(e)});
      }
      for (auto&& e : m_g.dst_edges(s)) {
        ret.emplace_back(e, uid {m_g.id(e)});
      }
    }

    assert(std::unique(ret.begin(), ret.end()) == ret.end());
    return ret;
  }

  auto node_graph::connections(const socket_handle& socket) const
    -> std::vector<connection_handle>
  {
    if (!_exists(socket))
      return {};

    std::vector<connection_handle> ret;

    for (auto&& e : m_g.src_edges(socket.descriptor()))
      ret.emplace_back(e, uid {m_g.id(e)});

    for (auto&& e : m_g.dst_edges(socket.descriptor()))
      ret.emplace_back(e, uid {m_g.id(e)});

    assert(std::unique(ret.begin(), ret.end()) == ret.end());
    return ret;
  }

  bool node_graph::is_input_socket(const socket_handle& h) const
  {
    auto lck = _lock();

    if (!_exists(h)) {
      Warning(g_logger, "node_graph::is_input_socket on invalid handle.");
      return false;
    }

    return m_g[h.descriptor()].is_input();
  }

  bool node_graph::is_output_socket(const socket_handle& h) const
  {
    auto lck = _lock();

    if (!_exists(h)) {
      Warning(g_logger, "node_graph::is_output_socket on invalid handle.");
      return false;
    }

    return m_g[h.descriptor()].is_output();
  }

  auto node_graph::input_connections(const node_handle& node) const
    -> std::vector<connection_handle>
  {
    if (!_exists(node))
      return {};

    std::vector<connection_handle> ret;
    for (auto&& s : m_g.sockets(node.descriptor())) {
      if (m_g[s].is_input()) {
        assert(m_g.src_edges(s).empty());
        for (auto&& e : m_g.dst_edges(s)) {
          ret.emplace_back(e, uid {m_g.id(s)});
        }
      }
    }
    return ret;
  }

  auto node_graph::output_connections(const node_handle& node) const
    -> std::vector<connection_handle>
  {
    if (!_exists(node))
      return {};

    std::vector<connection_handle> ret;
    for (auto&& s : m_g.sockets(node.descriptor())) {
      assert(m_g.dst_edges(s).empty());
      for (auto&& e : m_g.src_edges(s)) {
        ret.emplace_back(e, uid {m_g.id(s)});
      }
    }
    return ret;
  }

  bool node_graph::has_connection(const socket_handle& h) const
  {
    auto lck = _lock();

    if (!_exists(h)) {
      Warning(g_logger, "node_graph::has_connection() on invalid node handle.");
      return false;
    }

    if (!m_g.src_edges(h.descriptor()).empty()) {
      assert(m_g[h.descriptor()].is_output());
      return true;
    }

    if (!m_g.dst_edges(h.descriptor()).empty()) {
      assert(m_g[h.descriptor()].is_output());
      return true;
    }

    return false;
  }

  auto node_graph::get_interfaces(const socket_handle& socket) const
    -> std::vector<node_handle>
  {
    auto lck = _lock();

    if (!_exists(socket))
      return {};

    std::vector<node_handle> ret;
    auto ns = m_g.nodes(socket.descriptor());
    for (size_t i = 1; i < ns.size(); ++i) {
      assert(m_g[ns[i]].get_type() == node_type::interface);
      ret.emplace_back(ns[i], uid {m_g.id(ns[i])});
    }
    return ret;
  }

  auto node_graph::get_owner(const socket_handle& socket) const -> node_handle
  {
    auto lck = _lock();

    if (!_exists(socket))
      return {nullptr};

    assert(!m_g.nodes(socket.descriptor()).empty());
    auto n = m_g.nodes(socket.descriptor())[0];
    return node_handle(n, {m_g.id(n)});
  }

  auto node_graph::input_sockets(const node_handle& h) const
    -> std::vector<socket_handle>
  {
    auto lck = _lock();

    if (!_exists(h)) {
      Warning(g_logger, "node_graph::input_sockets() on invalid node handle.");
      return {};
    }

    auto&& sockets = m_g.sockets(h.descriptor());
    std::vector<socket_handle> ret;
    for (auto&& s : sockets) {
      if (m_g[s].is_input())
        ret.emplace_back(s, uid {m_g.id(s)});
    }
    return ret;
  }

  auto node_graph::output_sockets(const node_handle& h) const
    -> std::vector<socket_handle>
  {
    auto lck = _lock();

    if (!_exists(h)) {
      Warning(g_logger, "node_graph::outupt_sockets() on invalid node handle.");
      return {};
    }

    auto&& sockets = m_g.sockets(h.descriptor());
    std::vector<socket_handle> ret;
    for (auto&& s : sockets) {
      if (m_g[s].is_output())
        ret.emplace_back(s, uid {m_g.id(s)});
    }
    return ret;
  }

  bool node_graph::is_normal(const node_handle& h) const
  {
    auto lck = _lock();

    if (!_exists(h)) {
      Warning(g_logger, "node_graph::is_primitive() on invalid node handle.");
      return false;
    }

    return m_g[h.descriptor()].is_normal();
  }

  bool node_graph::is_primitive(const node_handle& h) const
  {
    auto lck = _lock();

    if (!_exists(h)) {
      Warning(g_logger, "node_graph::is_primitive() on invalid node handle.");
      return false;
    }

    return m_g[h.descriptor()].is_primitive();
  }

  bool node_graph::is_interface(const node_handle& node) const
  {
    auto lck = _lock();

    if (!_exists(node))
      return false;

    return m_g[node.descriptor()].is_interface();
  }

  auto node_graph::get_primitive(const node_handle& h) const
    -> std::optional<primitive_t>
  {
    auto lck = _lock();

    if (!_exists(h)) {
      Warning(g_logger, "node_graph::get_primitive() on invalid node handle.");
      return std::nullopt;
    }

    if (m_g[h.descriptor()].is_primitive())
      return m_g[h.descriptor()].get_primitive();
    return std::nullopt;
  }

  void node_graph::set_primitive(const node_handle& h, const primitive_t& prim)
  {
    auto lck = _lock();

    if (!_exists(h)) {
      Warning(g_logger, "node_graph::set_primitive() on invalid node handle.");
      return;
    }

    if (!m_g[h.descriptor()].is_primitive()) {
      Error(
        g_logger, "Cannot set primitive value to non-primitive node. Ignored.");
      return;
    }
    m_g[h.descriptor()].set_primitive(prim);
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

    if (m_g[node.descriptor()].is_primitive())
      return m_g[node.descriptor()].get_shared_primitive();

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