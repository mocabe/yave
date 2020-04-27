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


namespace yave {

  using namespace ranges;

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

  class node_graph::impl
  {
  public:
    /// Internal graph representation
    graph_t g;

  public:
    impl()
      : g {}
    {
      init_logger();
    }

    impl(graph_t&& gg) noexcept
      : g {std::move(gg)}
    {
    }

  public:
    bool exists(const node_handle& h) const
    {
      return h.has_value() && g.exists(h.descriptor())
             && h.id() == uid {g.id(h.descriptor())};
    }

    bool exists(const socket_handle& h) const
    {
      return h.has_value() && g.exists(h.descriptor())
             && h.id() == uid {g.id(h.descriptor())};
    }

    bool exists(const connection_handle& h) const
    {
      return h.has_value() && g.exists(h.descriptor())
             && h.id() == uid {g.id(h.descriptor())};
    }

  public:
    auto node(const uid& id) const
    {
      auto dsc = g.node(id.data);

      if (!dsc)
        return node_handle();

      return node_handle(dsc, id);
    }

    auto socket(const uid& id) const
    {
      auto dsc = g.socket(id.data);

      if (!dsc)
        return socket_handle();

      return socket_handle(dsc, id);
    }

    auto connection(const uid& id) const
    {
      auto dsc = g.edge(id.data);

      if (!dsc)
        return connection_handle();

      return connection_handle(dsc, id);
    }

    auto node(const socket_handle& socket) const
    {
      auto dsc = socket.descriptor();
      auto n   = g.nodes(dsc)[0];
      return node_handle(n, g.id(n));
    }

    auto interfaces(const socket_handle& socket) const
    {
      auto&& ns = g.nodes(socket.descriptor());
      return ns | views::drop(1) | to_node_handles(g);
    }

    auto nodes() const
    {
      auto&& ns = g.nodes();
      return ns | to_node_handles(g);
    }

    auto nodes(const std::string& name) const
    {
      auto&& ns = g.nodes();

      return ns //
             | views::filter([&](auto n) { return g[n].name() == name; })
             | to_node_handles(g);
    }

    auto sockets() const
    {
      auto&& ss = g.sockets();
      return ss | to_socket_handles(g);
    }

    auto sockets(const node_handle& node) const
    {
      auto&& ss = g.sockets(node.descriptor());
      return ss | to_socket_handles(g);
    }

    auto connections() const
    {
      auto&& es = g.edges();
      return es | to_connection_handles(g);
    }

    auto connections(const node_handle& node) const
    {
      auto&& ss = g.sockets(node.descriptor());

      auto es = views::concat(
                  ss | views::transform([&](auto s) { return g.src_edges(s); }),
                  ss | views::transform([&](auto s) { return g.dst_edges(s); }))
                | actions::join;

      return es | to_connection_handles(g);
    }

    auto connections(const socket_handle& socket) const
    {
      auto&& se = g.src_edges(socket.descriptor());
      auto&& de = g.dst_edges(socket.descriptor());

      return views::concat(se, de) | to_connection_handles(g);
    }

  public:
    auto get_info(const node_handle& h) const
    {
      auto&& ss = g.sockets(h.descriptor());

      auto iss = ss //
                 | views::filter([&](auto s) { return g[s].is_input(); })
                 | to_socket_handles(g);

      auto oss = ss //
                 | views::filter([&](auto s) { return g[s].is_output(); })
                 | to_socket_handles(g);

      auto&& n = g[h.descriptor()];

      return node_info(n.name(), iss, oss, n.type());
    }

    auto get_info(const socket_handle& h) const
    {
      auto&& ns = g.nodes(h.descriptor());

      auto nodes      = ns | to_node_handles(g);
      auto node       = nodes.front();
      auto interfaces = nodes | move | actions::drop(1);

      auto s = g[h.descriptor()];

      return socket_info(s.name(), s.type(), node, interfaces);
    }

    auto get_info(const connection_handle& h) const
    {
      auto src = g.src(h.descriptor());
      auto dst = g.dst(h.descriptor());

      auto src_nodes = g.nodes(src);
      auto dst_nodes = g.nodes(dst);

      assert(src_nodes.size() >= 1);
      assert(dst_nodes.size() >= 1);

      assert(!g[src_nodes[0]].is_interface());
      assert(!g[dst_nodes[0]].is_interface());

      auto src_handles    = src_nodes | to_node_handles(g);
      auto src_node       = src_handles.front();
      auto src_interfaces = src_handles | move | actions::drop(1);

      auto dst_handles    = dst_nodes | to_node_handles(g);
      auto dst_node       = dst_handles.front();
      auto dst_interfaces = dst_handles | move | actions::drop(1);

      return connection_info(
        src_node,
        socket_handle(src, g.id(src)),
        dst_node,
        socket_handle(dst, g.id(dst)),
        src_interfaces,
        dst_interfaces);
    }

  public:
    auto get_name(const node_handle& h) const
    {
      return g[h.descriptor()].name();
    }
    auto get_name(const socket_handle& h) const
    {
      return g[h.descriptor()].name();
    }
    void set_name(const node_handle& h, const std::string& name)
    {
      g[h.descriptor()].set_name(name);
    }
    void set_name(const socket_handle& h, const std::string& name)
    {
      g[h.descriptor()].set_name(name);
    }

  public:
    auto input_sockets(const node_handle& h) const
    {
      auto&& sockets = g.sockets(h.descriptor());

      return sockets //
             | views::filter([&](auto s) { return g[s].is_input(); })
             | to_socket_handles(g);
    }

    auto output_sockets(const node_handle& h) const
    {
      auto&& sockets = g.sockets(h.descriptor());

      return sockets //
             | views::filter([&](auto s) { return g[s].is_output(); })
             | to_socket_handles(g);
    }

  public:
    auto add(
      const std::string& name,
      const std::vector<std::string>& input_sockets,
      const std::vector<std::string>& output_sockets,
      const node_type& type)
    {
      auto id = uid::random_generate();

      assert(type == node_type::normal || type == node_type::interface);

      // add node
      auto node = g.add_node_with_id(id.data, name, type);

      if (!node)
        return node_handle();

      auto _add_attach_sockets = [&](auto&& names, auto io) {
        for (auto&& name : names) {
          auto socket = g.add_socket(name, io);
          if (!g.attach_socket(node, socket)) {
            return false;
          }
        }
        return true;
      };

      auto attach_i = _add_attach_sockets(input_sockets, socket_type::input);
      auto attach_o = _add_attach_sockets(output_sockets, socket_type::output);

      if (!attach_i || !attach_o) {
        // rollback changes
        for (auto&& s : g.sockets(node)) {
          g.remove_socket(s);
        }
        g.remove_node(node);
        return node_handle();
      }

      auto handle = node_handle(node, g.id(node));

      Info(
        g_logger,
        "Created Node: name=\"{}\", id={}",
        name,
        to_string(handle.id()));

      return handle;
    }

    auto add_copy(const impl& other, const node_handle& node)
    {
      auto is   = other.input_sockets(node);
      auto os   = other.output_sockets(node);
      auto info = other.get_info(node);

      if (!info.is_normal()) {
        Error(g_logger, "copy_add(): Invalid node type");
        return node_handle();
      }

      auto cpy_n =
        g.add_node_with_id(node.id().data, other.g[node.descriptor()]);

      if (!cpy_n)
        return node_handle();

      for (auto&& s : is) {
        auto cpy_s = g.add_socket_with_id(s.id().data, other.g[s.descriptor()]);
        if (!g.attach_socket(cpy_n, cpy_s)) {
          for (auto&& ss : g.sockets(cpy_n))
            g.remove_socket(ss);
          g.remove_node(cpy_n);
          return node_handle();
        }
      }

      for (auto&& s : os) {
        auto cpy_s = g.add_socket_with_id(s.id().data, other.g[s.descriptor()]);
        if (!g.attach_socket(cpy_n, cpy_s)) {
          for (auto&& ss : g.sockets(cpy_n))
            g.remove_socket(ss);
          g.remove_node(cpy_n);
          return node_handle();
        }
      }

      Info(
        g_logger,
        "Copied node from other graph: name={},id={}",
        info.name(),
        to_string(node.id()));

      return node_handle(cpy_n, g.id(cpy_n));
    }

    void remove(const node_handle& node)
    {
      if (!exists(node))
        return;

      Info(
        g_logger,
        "Removing Node: name=\"{}\", id={}",
        g[node.descriptor()].name(),
        to_string(node.id()));

      for (auto&& s : g.sockets(node.descriptor())) {
        // detach
        g.detach_socket(node.descriptor(), s);
        // delete socket if needed
        for (auto&& n : g.nodes(s)) {
          if (!g[n].is_interface())
            continue;
        }
        g.remove_socket(s);
      }

      g.remove_node(node.descriptor());
    }

  public:
    bool attach_interface(
      const node_handle& interface,
      const socket_handle& socket)
    {
      if (!g[interface.descriptor()].is_interface()) {
        Error(g_logger, "attach_interface(): Not interface node");
        return false;
      }

      auto info = get_info(socket);

      for (auto&& s : g.sockets(interface.descriptor())) {
        if (socket.id().data == g.id(s)) {
          Warning(g_logger, "attach_interface(): already attached");
          return true; // already attached
        }
      }

      if (!g.attach_socket(interface.descriptor(), socket.descriptor())) {
        Error(g_logger, "attach_interface(): Failed to attach socket!");
        return false;
      }

      Info(
        g_logger,
        "Attached interface: node={}({}), socket={}({})",
        get_name(info.node()),
        to_string(info.node().id()),
        info.name(),
        to_string(socket.id()));

      return true;
    }

    void detach_interface(
      const node_handle& interface,
      const socket_handle& socket)
    {
      auto info = get_info(socket);

      for (auto&& s : g.sockets(interface.descriptor())) {

        if (socket.id().data == g.id(s)) {

          // detach
          g.detach_socket(interface.descriptor(), s);

          Info(
            g_logger,
            "Detached interface: node={}({}), socket={}({})",
            get_name(info.node()),
            to_string(info.node().id()),
            info.name(),
            to_string(socket.id()));
          return;
        }
      }
    }

  public:
    bool find_loop(const node_handle& node) const;

    auto connect(
      const socket_handle& src_socket,
      const socket_handle& dst_socket)
    {
      // socket descriptors
      auto s = src_socket.descriptor();
      auto d = dst_socket.descriptor();

      // nodes
      auto sn       = g.nodes(s)[0];
      auto dn       = g.nodes(d)[0];
      auto src_node = node_handle(sn, g.id(sn));
      auto dst_node = node_handle(dn, g.id(dn));

      // check socket type
      if (!g[s].is_output() || !g[d].is_input()) {
        Error(g_logger, "Failed to connect sockets: Invalid socket type");
        return connection_handle();
      }

      // already exists
      for (auto&& e : g.dst_edges(d)) {
        if (g.src(e) == s) {
          Warning(
            g_logger,
            "Connection already exists. Return existing connection "
            "handle.");
          return connection_handle(e, g.id(e));
        }
      }

      // add new edge to graph
      auto new_edge = g.add_edge(s, d);
      assert(new_edge);

      // closed loop check
      if (find_loop(src_node)) {
        Error(
          g_logger,
          "Failed to connect: src='{}'({})::{}, dst='{}'({})::{}",
          get_name(src_node),
          to_string(src_node.id()),
          get_name(src_socket),
          get_name(dst_node),
          to_string(dst_node.id()),
          get_name(dst_socket));
        Error(g_logger, "Closed loop is not allowed");
        g.remove_edge(new_edge);
        return connection_handle();
      }

      Info(
        g_logger,
        "Connected socket: src=\"{}\"({})::{}, dst=\"{}\"({})::{}",
        get_name(src_node),
        to_string(src_node.id()),
        get_name(src_socket),
        get_name(dst_node),
        to_string(dst_node.id()),
        get_name(dst_socket));

      return connection_handle(new_edge, g.id(new_edge));
    }

    void disconnect(const connection_handle& h)
    {
      auto info = get_info(h);

      Info(
        g_logger,
        "Disconnecting: src='{}'({})::{} dst='{}'({})::{}",
        get_name(info.src_node()),
        to_string(info.src_node().id()),
        get_name(info.src_socket()),
        get_name(info.dst_node()),
        to_string(info.dst_node().id()),
        get_name(info.dst_socket()));

      // remove edge
      g.remove_edge(h.descriptor());
    }

  public:
    bool is_input_socket(const socket_handle& h) const
    {
      return g[h.descriptor()].is_input();
    }

    bool is_output_socket(const socket_handle& h) const
    {
      return g[h.descriptor()].is_output();
    }

    auto input_connections(const node_handle& node) const
    {
      auto&& ss = g.sockets(node.descriptor());

      auto es = ss //
                | views::filter([&](auto s) { return g[s].is_input(); })
                | views::transform([&](auto s) { return g.dst_edges(s); })
                | actions::join;

      return es | to_connection_handles(g);
    }

    auto output_connections(const node_handle& node) const
    {
      auto&& ss = g.sockets(node.descriptor());

      auto es = ss //
                | views::filter([&](auto s) { return g[s].is_output(); })
                | views::transform([&](auto s) { return g.src_edges(s); })
                | actions::join;

      return es | to_connection_handles(g);
    }

    bool has_connection(const socket_handle& h) const
    {
      if (!g.src_edges(h.descriptor()).empty()) {
        assert(g[h.descriptor()].is_output());
        return true;
      }

      if (!g.dst_edges(h.descriptor()).empty()) {
        assert(g[h.descriptor()].is_input());
        return true;
      }

      return false;
    }

    auto get_owner(const socket_handle& socket) const
    {
      assert(!g.nodes(socket.descriptor()).empty());
      auto n = g.nodes(socket.descriptor())[0];
      return node_handle(n, g.id(n));
    }

    bool is_normal(const node_handle& h) const
    {
      return g[h.descriptor()].is_normal();
    }

    bool is_interface(const node_handle& node) const
    {
      return g[node.descriptor()].is_interface();
    }

    bool has_data(const socket_handle& h) const
    {
      return g[h.descriptor()].has_data();
    }

    bool has_data(const node_handle& h) const
    {
      return g[h.descriptor()].has_data();
    }

    auto get_data(const socket_handle& h) const
    {
      auto data = g[h.descriptor()].get_data();
      return data ? *data : object_ptr();
    }

    auto get_data(const node_handle& h) const
    {
      auto data = g[h.descriptor()].get_data();
      return data ? *data : object_ptr();
    }

    void set_data(const socket_handle& h, object_ptr<Object> data)
    {
      if (!g[h.descriptor()].has_data())
        Info(
          g_logger, "Enable custom data on socket: id={}", to_string(h.id()));

      if (!data)
        Info(
          g_logger, "Clearing custom data on socket: id={}", to_string(h.id()));

      g[h.descriptor()].set_data(std::move(data));
    }

    void set_data(const node_handle& h, object_ptr<Object> data)
    {
      if (!g[h.descriptor()].has_data())
        Info(g_logger, "Enable custom data on node: id={}", to_string(h.id()));

      if (!data)
        Info(
          g_logger, "Clearing custom data on socket: id={}", to_string(h.id()));

      g[h.descriptor()].set_data(std::move(data));
    }

    auto root_of(const node_handle& node) const
    {
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
        for (auto&& s : g.sockets(n.descriptor())) {
          for (auto&& e : g.src_edges(s)) {
            auto dst_s = g.dst(e);
            auto dst_n = g.nodes(dst_s);
            assert(dst_n.size() == 1);
            stack.emplace_back(dst_n[0], uid {g.id(dst_n[0])});
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

    auto roots() const
    {
      std::vector<node_handle> ret;

      // TODO: Improve performance.
      for (auto&& n : g.nodes()) {
        for (auto&& root : root_of(node_handle(n, g.id(n)))) {
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

    void clear()
    {
      g.clear();
    }

    bool empty() const
    {
      return g.empty();
    }

    auto clone() const
    {
      return impl(g.clone());
    }

  };

  // DFS to check loop
  bool node_graph::impl::find_loop(const node_handle& node) const
  {
    std::vector<node_handle> stack;

    // clear flags before use
    for (auto&& n : nodes()) {
      g[n.descriptor()].set_flags(0);
    }

    // flag bits
    uint8_t visited_bit = 1 << 0;
    uint8_t on_path_bit = 1 << 1;

    stack.push_back(node);

    while (!stack.empty()) {
      auto top  = stack.back();
      auto topd = top.descriptor();

      g[topd].set_flags(visited_bit | on_path_bit);

      for (auto&& s : input_sockets(top)) {
        for (auto&& c : connections(s)) {
          auto src    = get_info(c).src_node();
          auto srcd   = src.descriptor();
          auto srcflg = g[srcd].get_flags();

          // remember path
          if (!(srcflg & visited_bit)) {
            stack.push_back(src);
            goto endloop;
          }

          // check loop
          if (srcflg & on_path_bit)
            return true;
        }
      }
      // remove on_path bit
      g[topd].set_flags(visited_bit);
      stack.pop_back();

    endloop:;
    }

    return false;
  }

  node_graph::node_graph()
    : m_pimpl {std::make_unique<impl>()}
  {
  }

  node_graph::~node_graph() noexcept
  {
  }

  node_graph::node_graph(node_graph&& other) noexcept = default;
  node_graph& node_graph::operator=(node_graph&& other) noexcept = default;

  bool node_graph::exists(const node_handle& h) const
  {
    return m_pimpl->exists(h);
  }

  bool node_graph::exists(const connection_handle& h) const
  {
    return m_pimpl->exists(h);
  }

  bool node_graph::exists(const socket_handle& h) const
  {
    return m_pimpl->exists(h);
  }

  auto node_graph::get_info(const node_handle& h) const
    -> std::optional<node_info>
  {
    if (!exists(h))
      return std::nullopt;

    return m_pimpl->get_info(h);
  }

  auto node_graph::get_info(const socket_handle& h) const
    -> std::optional<socket_info>
  {
    if (!exists(h))
      return std::nullopt;

    return m_pimpl->get_info(h);
  }

  auto node_graph::get_info(const connection_handle& h) const
    -> std::optional<connection_info>
  {
    if (!exists(h))
      return std::nullopt;

    return m_pimpl->get_info(h);
  }

  auto node_graph::get_name(const node_handle& node) const
    -> std::optional<std::string>
  {
    if (!exists(node))
      return std::nullopt;

    return m_pimpl->get_name(node);
  }

  auto node_graph::get_name(const socket_handle& socket) const
    -> std::optional<std::string>
  {
    if (!exists(socket))
      return std::nullopt;

    return m_pimpl->get_name(socket);
  }

  void node_graph::set_name(const node_handle& h, const std::string& name)
  {
    if (!exists(h))
      return;

    m_pimpl->set_name(h, name);
  }

  void node_graph::set_name(const socket_handle& h, const std::string& name)
  {
    if (!exists(h))
      return;

    m_pimpl->set_name(h, name);
  }

  auto node_graph::add(
    const std::string& name,
    const std::vector<std::string>& input_sockets,
    const std::vector<std::string>& output_sockets,
    const node_type& type) -> node_handle
  {
    auto ret = m_pimpl->add(name, input_sockets, output_sockets, type);

    if (!ret)
      throw std::runtime_error("Failed to add node");

    return ret;
  }

  auto node_graph::add_copy(const node_graph& other, const node_handle& node)
    -> node_handle
  {
    if (!other.exists(node)) {
      throw std::runtime_error("copy_add(): Invalid node handle");
    }

    auto ret = m_pimpl->add_copy(*other.m_pimpl, node);

    if (!ret)
      throw std::runtime_error("Failed to add copy of node");

    return ret;
  }

  bool node_graph::attach_interface(
    const node_handle& interface,
    const socket_handle& socket)
  {
    if (!exists(interface) || !exists(socket))
      return false;

    return m_pimpl->attach_interface(interface, socket);
  }

  void node_graph::detach_interface(
    const node_handle& interface,
    const socket_handle& socket)
  {
    if (!exists(interface) || !exists(socket))
      return;

    m_pimpl->detach_interface(interface, socket);
  }

  void node_graph::remove(const node_handle& node)
  {
    if (!exists(node))
      return;

    m_pimpl->remove(node);
  }

  auto node_graph::connect(
    const socket_handle& src_socket,
    const socket_handle& dst_socket) -> connection_handle
  {
    if (!exists(src_socket) || !exists(dst_socket)) {
      Error(g_logger, "Failed to connect sockets: Invalid socket descriptor");
      return nullptr;
    }

    return m_pimpl->connect(src_socket, dst_socket);
  }

  void node_graph::disconnect(const connection_handle& h)
  {
    if (!exists(h)) {
      Warning(
        g_logger,
        "node_graph::disconnect() on invalid node handle: id={}",
        to_string(h.id()));
      return;
    }

    m_pimpl->disconnect(h);
  }

  auto node_graph::node(const uid& id) const -> node_handle
  {
    return m_pimpl->node(id);
  }

  auto node_graph::socket(const uid& id) const -> socket_handle
  {
    return m_pimpl->socket(id);
  }

  auto node_graph::connection(const uid& id) const -> connection_handle
  {
    return m_pimpl->connection(id);
  }

  auto node_graph::node(const socket_handle& socket) const -> node_handle
  {
    if (!exists(socket))
      return {nullptr};

    return m_pimpl->node(socket);
  }

  auto node_graph::interfaces(const socket_handle& socket) const
    -> std::vector<node_handle>
  {
    if (!exists(socket))
      return {};

    return m_pimpl->interfaces(socket);
  }

  auto node_graph::nodes() const -> std::vector<node_handle>
  {
    return m_pimpl->nodes();
  }

  auto node_graph::nodes(const std::string& name) const
    -> std::vector<node_handle>
  {
    return m_pimpl->nodes(name);
  }

  auto node_graph::sockets() const -> std::vector<socket_handle>
  {
    return m_pimpl->sockets();
  }

  auto node_graph::sockets(const node_handle& node) const
    -> std::vector<socket_handle>
  {
    if (!exists(node))
      return {};

    return m_pimpl->sockets(node);
  }

  auto node_graph::connections() const -> std::vector<connection_handle>
  {
    return m_pimpl->connections();
  }

  auto node_graph::connections(const node_handle& node) const
    -> std::vector<connection_handle>
  {
    if (!exists(node))
      return {};

    return m_pimpl->connections(node);
  }

  auto node_graph::connections(const socket_handle& socket) const
    -> std::vector<connection_handle>
  {
    if (!exists(socket))
      return {};

    return m_pimpl->connections(socket);
  }

  bool node_graph::is_input_socket(const socket_handle& h) const
  {
    if (!exists(h)) {
      Warning(g_logger, "node_graph::is_input_socket on invalid handle.");
      return false;
    }

    return m_pimpl->is_input_socket(h);
  }

  bool node_graph::is_output_socket(const socket_handle& h) const
  {
    if (!exists(h)) {
      Warning(g_logger, "node_graph::is_output_socket on invalid handle.");
      return false;
    }

    return m_pimpl->is_output_socket(h);
  }

  auto node_graph::input_connections(const node_handle& node) const
    -> std::vector<connection_handle>
  {
    if (!exists(node))
      return {};

    return m_pimpl->input_connections(node);
  }

  auto node_graph::output_connections(const node_handle& node) const
    -> std::vector<connection_handle>
  {
    if (!exists(node))
      return {};

    return m_pimpl->output_connections(node);
  }

  bool node_graph::has_connection(const socket_handle& h) const
  {
    if (!exists(h)) {
      Warning(g_logger, "node_graph::has_connection() on invalid node handle.");
      return false;
    }

    return m_pimpl->has_connection(h);
  }

  auto node_graph::get_owner(const socket_handle& socket) const -> node_handle
  {
    if (!exists(socket))
      return {nullptr};

    return m_pimpl->get_owner(socket);
  }

  auto node_graph::input_sockets(const node_handle& h) const
    -> std::vector<socket_handle>
  {
    if (!exists(h)) {
      Warning(g_logger, "node_graph::input_sockets() on invalid node handle.");
      return {};
    }

    return m_pimpl->input_sockets(h);
  }

  auto node_graph::output_sockets(const node_handle& h) const
    -> std::vector<socket_handle>
  {
    if (!exists(h)) {
      Warning(g_logger, "node_graph::outupt_sockets() on invalid node handle.");
      return {};
    }

    return m_pimpl->output_sockets(h);
  }

  bool node_graph::is_normal(const node_handle& h) const
  {
    if (!exists(h)) {
      Warning(g_logger, "node_graph::is_primitive() on invalid node handle.");
      return false;
    }

    return m_pimpl->is_normal(h);
  }

  bool node_graph::is_interface(const node_handle& h) const
  {
    if (!exists(h))
      return false;

    return m_pimpl->is_interface(h);
  }

  bool node_graph::has_data(const socket_handle& h) const
  {
    if (!exists(h))
      return false;

    return m_pimpl->has_data(h);
  }

  bool node_graph::has_data(const node_handle& h) const
  {
    if (!exists(h))
      return false;

    return m_pimpl->has_data(h);
  }

  auto node_graph::get_data(const socket_handle& h) const -> object_ptr<Object>
  {
    if (!exists(h))
      return nullptr;

    return m_pimpl->get_data(h);
  }

  auto node_graph::get_data(const node_handle& h) const -> object_ptr<Object>
  {
    if (!exists(h))
      return nullptr;

    return m_pimpl->get_data(h);
  }

  void node_graph::set_data(const socket_handle& h, object_ptr<Object> data)
  {
    if (!exists(h))
      return;

    return m_pimpl->set_data(h, data);
  }

  void node_graph::set_data(const node_handle& h, object_ptr<Object> data)
  {
    if (!exists(h))
      return;

    return m_pimpl->set_data(h, data);
  }

  auto node_graph::roots() const -> std::vector<node_handle>
  {
    return m_pimpl->roots();
  }

  auto node_graph::root_of(const node_handle& node) const
    -> std::vector<node_handle>
  {
    if (!exists(node)) {
      Warning(g_logger, "node_graph::root_of() on invalid node handle.");
      return {};
    }

    return m_pimpl->root_of(node);
  }

  void node_graph::clear()
  {
    m_pimpl->clear();
  }

  bool node_graph::empty() const
  {
    return m_pimpl->empty();
  }

  node_graph::node_graph(std::unique_ptr<impl>&& pimpl) noexcept
    : m_pimpl {std::move(pimpl)}
  {
  }

  auto node_graph::clone() const -> node_graph
  {
    return node_graph(std::make_unique<impl>(m_pimpl->clone()));
  }

} // namespace yave