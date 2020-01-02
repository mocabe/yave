//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/core/node_graph.hpp>
#include <yave/rts/unit.hpp>
#include <yave/support/log.hpp>

#include <range/v3/algorithm.hpp>
#include <range/v3/view.hpp>
#include <range/v3/action.hpp>

YAVE_DECL_G_LOGGER(node_graph)

using namespace ranges;

namespace yave {

  constexpr auto to_node_handles = [](const graph_t& graph) {
    return views::transform(
             [&](auto&& n) { return node_handle(n, graph.id(n)); })
           | to_vector;
  };

  constexpr auto to_socket_handles = [](const graph_t& graph) {
    return views::transform(
             [&](auto&& s) { return socket_handle(s, graph.id(s)); })
           | to_vector;
  };

  constexpr auto to_connection_handles = [](const graph_t& graph) {
    return views::transform(
             [&](auto&& e) { return connection_handle(e, graph.id(e)); })
           | to_vector;
  };

  node_graph::node_graph()
    : m_g {}
    , m_mtx {}
  {
    init_logger();
  }

  node_graph::~node_graph() noexcept
  {
  }

  node_graph::node_graph(node_graph&& other) noexcept
    : m_mtx {}
  {
    auto lck = other._lock();
    m_g      = std::move(other.m_g);
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

    auto&& ss = m_g.sockets(h.descriptor());

    auto iss = ss //
               | views::filter([&](auto s) { return m_g[s].is_input(); })
               | to_socket_handles(m_g);

    auto oss = ss //
               | views::filter([&](auto s) { return m_g[s].is_output(); })
               | to_socket_handles(m_g);

    auto&& n = m_g[h.descriptor()];

    return node_info(n.name(), iss, oss, n.type());
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

    auto&& ns = m_g.nodes(h.descriptor());

    auto nodes      = ns | to_node_handles(m_g);
    auto node       = nodes.front();
    auto interfaces = nodes | move | actions::drop(1);

    auto s = m_g[h.descriptor()];

    return socket_info(s.name(), s.type(), node, interfaces);
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

    assert(!m_g[src_nodes[0]].is_interface());
    assert(!m_g[dst_nodes[0]].is_interface());

    auto src_handles    = src_nodes | to_node_handles(m_g);
    auto src_node       = src_handles.front();
    auto src_interfaces = src_handles | move | actions::drop(1);

    auto dst_handles    = dst_nodes | to_node_handles(m_g);
    auto dst_node       = dst_handles.front();
    auto dst_interfaces = dst_handles | move | actions::drop(1);

    return connection_info(
      src_node,
      socket_handle(src, m_g.id(src)),
      dst_node,
      socket_handle(dst, m_g.id(dst)),
      src_interfaces,
      dst_interfaces);
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

  void node_graph::set_name(const node_handle& h, const std::string& name)
  {
    auto lck = _lock();

    if (!_exists(h))
      return;

    m_g[h.descriptor()].set_name(name);
  }

  void node_graph::set_name(const socket_handle& h, const std::string& name)
  {
    auto lck = _lock();

    if (!_exists(h))
      return;

    m_g[h.descriptor()].set_name(name);
  }

  auto node_graph::add(
    const std::string& name,
    const std::vector<std::string>& input_sockets,
    const std::vector<std::string>& output_sockets,
    node_type type) -> node_handle
  {
    auto lck = _lock();

    // validate parameters
    {
      if (name == "")
        return {nullptr};

      for (auto&& s : input_sockets)
        if (s == "")
          return {nullptr};

      for (auto&& s : output_sockets)
        if (s == "")
          return {nullptr};

      auto _has_unique_names = [](auto names) {
        sort(names);
        return unique(names) == names.end();
      };

      if (
        !_has_unique_names(input_sockets) || !_has_unique_names(output_sockets))
        return {nullptr};
    }

    auto id = uid::random_generate();

    assert(type == node_type::normal || type == node_type::interface);

    // add node
    auto node = m_g.add_node_with_id(id.data, name, type);

    if (!node)
      return {nullptr};

    auto _add_attach_sockets = [&](auto&& names, auto io) {
      for (auto&& name : names) {
        auto socket = m_g.add_socket(name, io);
        if (!m_g.attach_socket(node, socket)) {
          throw std::runtime_error(
            "Failed to add node to node_graph: Failed to attach sockets.");
        }
      }
    };

    try {
      _add_attach_sockets(input_sockets, socket_type::input);
      _add_attach_sockets(output_sockets, socket_type::output);
    } catch (...) {
      for (auto&& s : m_g.sockets(node)) {
        m_g.remove_socket(s);
      }
      m_g.remove_node(node);
      throw;
    }

    auto handle = node_handle(node, m_g.id(node));

    Info(
      g_logger,
      "Created Node: name=\"{}\", id={}",
      name,
      to_string(handle.id()));

    return handle;
  }

  auto node_graph::add_copy(const node_graph& other, const node_handle& node)
    -> node_handle
  {
    auto lck1 = _lock();
    auto lck2 = other._lock();

    if (!other._exists(node)) {
      Error(g_logger, "copy_add(): Invalid node handle");
      return {nullptr};
    }

    auto is   = other._input_sockets(node);
    auto os   = other._output_sockets(node);
    auto info = other._get_info(node);

    if (!info.is_normal()) {
      Error(g_logger, "copy_add(): Invalid node type");
      return {nullptr};
    }

    auto cpy_n =
      m_g.add_node_with_id(node.id().data, other.m_g[node.descriptor()]);

    assert(cpy_n);

    [[maybe_unused]] bool b;

    for (auto&& s : is) {
      auto cpy_s =
        m_g.add_socket_with_id(s.id().data, other.m_g[s.descriptor()]);
      b = m_g.attach_socket(cpy_n, cpy_s);
      assert(b);
    }

    for (auto&& s : os) {
      auto si = other._get_info(s);
      auto cpy_s =
        m_g.add_socket_with_id(s.id().data, other.m_g[s.descriptor()]);
      b = m_g.attach_socket(cpy_n, cpy_s);
      assert(b);
    }

    Info(
      g_logger,
      "Copied node from other graph: name={},id={}",
      info.name(),
      to_string(node.id()));

    return node_handle(cpy_n, m_g.id(cpy_n));
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
    if (!m_g[s].is_output() || !m_g[d].is_input()) {
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
        return connection_handle(e, m_g.id(e));
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

    return connection_handle(new_edge, m_g.id(new_edge));
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
      node_handle(sn, m_g.id(sn)),
      src_socket,
      node_handle(dn, m_g.id(dn)),
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

  auto node_graph::node(const socket_handle& socket) const -> node_handle
  {
    auto lck = _lock();

    if (!_exists(socket))
      return {nullptr};

    auto dsc = socket.descriptor();
    auto n   = m_g.nodes(dsc)[0];
    return node_handle(n, m_g.id(n));
  }

  auto node_graph::interfaces(const socket_handle& socket) const
    -> std::vector<node_handle>
  {
    auto lck = _lock();

    if (!_exists(socket))
      return {};

    auto&& ns = m_g.nodes(socket.descriptor());
    return ns | views::drop(1) | to_node_handles(m_g);
  }

  auto node_graph::nodes() const -> std::vector<node_handle>
  {
    auto lck = _lock();

    auto&& ns = m_g.nodes();
    return ns | to_node_handles(m_g);
  }

  auto node_graph::nodes(const std::string& name) const
    -> std::vector<node_handle>
  {
    auto lck = _lock();

    auto&& ns = m_g.nodes();

    return ns //
           | views::filter([&](auto n) { return m_g[n].name() == name; })
           | to_node_handles(m_g);
  }

  auto node_graph::sockets() const -> std::vector<socket_handle>
  {
    auto lck = _lock();

    auto&& ss = m_g.sockets();
    return ss | to_socket_handles(m_g);
  }

  auto node_graph::sockets(const node_handle& node) const
    -> std::vector<socket_handle>
  {
    auto lck = _lock();

    if (!_exists(node))
      return {};

    auto&& ss = m_g.sockets(node.descriptor());
    return ss | to_socket_handles(m_g);
  }

  auto node_graph::connections() const -> std::vector<connection_handle>
  {
    auto lck = _lock();

    auto&& es = m_g.edges();
    return es | to_connection_handles(m_g);
  }

  auto node_graph::connections(const node_handle& node) const
    -> std::vector<connection_handle>
  {
    auto lck = _lock();

    if (!_exists(node))
      return {};

    auto&& ss = m_g.sockets(node.descriptor());

    auto es = views::concat(
                ss | views::transform([&](auto s) { return m_g.src_edges(s); }),
                ss | views::transform([&](auto s) { return m_g.dst_edges(s); }))
              | actions::join;

    return es | to_connection_handles(m_g);
  }

  auto node_graph::connections(const socket_handle& socket) const
    -> std::vector<connection_handle>
  {
    auto lck = _lock();

    if (!_exists(socket))
      return {};

    auto&& se = m_g.src_edges(socket.descriptor());
    auto&& de = m_g.dst_edges(socket.descriptor());

    return views::concat(se, de) | to_connection_handles(m_g);
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
    auto lck = _lock();

    if (!_exists(node))
      return {};

    auto&& ss = m_g.sockets(node.descriptor());

    auto es = ss //
              | views::filter([&](auto s) { return m_g[s].is_input(); })
              | views::transform([&](auto s) { return m_g.dst_edges(s); })
              | actions::join;

    return es | to_connection_handles(m_g);
  }

  auto node_graph::output_connections(const node_handle& node) const
    -> std::vector<connection_handle>
  {
    auto lck = _lock();

    if (!_exists(node))
      return {};

    auto&& ss = m_g.sockets(node.descriptor());

    auto es = ss //
              | views::filter([&](auto s) { return m_g[s].is_output(); })
              | views::transform([&](auto s) { return m_g.src_edges(s); })
              | actions::join;

    return es | to_connection_handles(m_g);
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
      assert(m_g[h.descriptor()].is_input());
      return true;
    }

    return false;
  }

  auto node_graph::get_owner(const socket_handle& socket) const -> node_handle
  {
    auto lck = _lock();

    if (!_exists(socket))
      return {nullptr};

    assert(!m_g.nodes(socket.descriptor()).empty());
    auto n = m_g.nodes(socket.descriptor())[0];
    return node_handle(n, m_g.id(n));
  }

  auto node_graph::_input_sockets(const node_handle& h) const
    -> std::vector<socket_handle>
  {
    auto&& sockets = m_g.sockets(h.descriptor());

    return sockets //
           | views::filter([&](auto s) { return m_g[s].is_input(); })
           | to_socket_handles(m_g);
  }

  auto node_graph::_output_sockets(const node_handle& h) const
    -> std::vector<socket_handle>
  {
    auto&& sockets = m_g.sockets(h.descriptor());

    return sockets //
           | views::filter([&](auto s) { return m_g[s].is_output(); })
           | to_socket_handles(m_g);
  }

  auto node_graph::input_sockets(const node_handle& h) const
    -> std::vector<socket_handle>
  {
    auto lck = _lock();

    if (!_exists(h)) {
      Warning(g_logger, "node_graph::input_sockets() on invalid node handle.");
      return {};
    }

    return _input_sockets(h);
  }

  auto node_graph::output_sockets(const node_handle& h) const
    -> std::vector<socket_handle>
  {
    auto lck = _lock();

    if (!_exists(h)) {
      Warning(g_logger, "node_graph::outupt_sockets() on invalid node handle.");
      return {};
    }

    return _output_sockets(h);
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

  bool node_graph::is_interface(const node_handle& node) const
  {
    auto lck = _lock();

    if (!_exists(node))
      return false;

    return m_g[node.descriptor()].is_interface();
  }

  bool node_graph::has_data(const socket_handle& h) const
  {
    auto lck = _lock();

    if (!_exists(h))
      return false;

    return m_g[h.descriptor()].has_data();
  }

  bool node_graph::has_data(const node_handle& h) const
  {
    auto lck = _lock();

    if (!_exists(h))
      return false;

    return m_g[h.descriptor()].has_data();
  }

  auto node_graph::get_data(const socket_handle& h) const -> object_ptr<Object>
  {
    auto lck = _lock();

    if (!_exists(h))
      return nullptr;

    auto data = m_g[h.descriptor()].get_data();
    return data ? *data : nullptr;
  }

  auto node_graph::get_data(const node_handle& h) const -> object_ptr<Object>
  {
    auto lck = _lock();

    if (!_exists(h))
      return nullptr;

    auto data = m_g[h.descriptor()].get_data();
    return data ? *data : nullptr;
  }

  void node_graph::set_data(const socket_handle& h, object_ptr<Object> data)
  {
    auto lck = _lock();

    if (!_exists(h))
      return;

    if (!data)
      return;

    if (!m_g[h.descriptor()].has_data())
      Info(g_logger, "Enable custom data on socket: id={}", to_string(h.id()));

    m_g[h.descriptor()].set_data(std::move(data));
  }

  void node_graph::set_data(const node_handle& h, object_ptr<Object> data)
  {
    auto lck = _lock();

    if (!_exists(h))
      return;

    if (!data)
      return;

    if (!m_g[h.descriptor()].has_data())
      Info(g_logger, "Enable custom data on node: id={}", to_string(h.id()));

    m_g[h.descriptor()].set_data(std::move(data));
  }

  auto node_graph::roots() const -> std::vector<node_handle>
  {
    auto lck = _lock();

    std::vector<node_handle> ret;

    // TODO: Improve performance.
    for (auto&& n : m_g.nodes()) {
      for (auto&& root : _root_of(node_handle(n, m_g.id(n)))) {
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

  bool node_graph::empty() const
  {
    auto lck = _lock();
    return m_g.empty();
  }

  auto node_graph::clone() const -> node_graph
  {
    node_graph ret;

    auto lck = _lock();
    ret.m_g  = m_g.clone();

    return ret;
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
        auto iter = ranges::find(pth, n);

        if (iter == pth.end())
          return false;

        ret = {iter, pth.end()};
        return true;
      });

    return ret;
  }

  auto node_graph::_lock() const -> std::unique_lock<std::mutex>
  {
    return std::unique_lock(m_mtx);
  }

} // namespace yave