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

namespace yave {

  namespace {

    namespace rn = ranges;
    namespace rv = rn::views;
    namespace ra = rn::actions;

    // convert descriptors to handles
    constexpr auto to_handles = [](const graph_t& graph) {
      return rv::transform(
               [&](auto&& n) { return descriptor_handle(n, graph.id(n)); })
             | rn::to_vector;
    };

  } // namespace

  node_graph::node_graph(graph_t&& gg) noexcept
    : g {std::move(gg)}
  {
  }

  bool node_graph::exists(const node_handle& h) const
  {
    if (!h.has_value())
      return false;

    if (auto d = g.node(h.id().data))
      return d == h.descriptor();

    return false;
  }

  bool node_graph::exists(const socket_handle& h) const
  {
    if (!h.has_value())
      return false;

    if (auto d = g.socket(h.id().data))
      return d == h.descriptor();

    return false;
  }

  bool node_graph::exists(const connection_handle& h) const
  {
    if (!h.has_value())
      return false;

    if (auto d = g.edge(h.id().data))
      return d == h.descriptor();

    return false;
  }

  auto node_graph::get_info(const node_handle& h) const
    -> std::optional<node_info>
  {
    if (!exists(h))
      return std::nullopt;

    auto&& ss = g.sockets(h.descriptor());

    auto iss = ss //
               | rv::filter([&](auto s) { return g[s].is_input(); })
               | to_handles(g);

    auto oss = ss //
               | rv::filter([&](auto s) { return g[s].is_output(); })
               | to_handles(g);

    auto&& n = g[h.descriptor()];

    return node_info(n.name(), iss, oss, n.type());
  }

  auto node_graph::get_info(const socket_handle& h) const
    -> std::optional<socket_info>
  {
    if (!exists(h))
      return std::nullopt;

    auto&& ns = g.nodes(h.descriptor());

    auto nodes      = ns | to_handles(g);
    auto node       = nodes.front();
    auto interfaces = nodes | rn::move | ra::drop(1);

    auto s = g[h.descriptor()];

    return socket_info(s.name(), s.type(), node, interfaces);
  }

  auto node_graph::get_info(const connection_handle& h) const
    -> std::optional<connection_info>
  {
    if (!exists(h))
      return std::nullopt;

    auto src = g.src(h.descriptor());
    auto dst = g.dst(h.descriptor());

    auto src_nodes = g.nodes(src);
    auto dst_nodes = g.nodes(dst);

    assert(src_nodes.size() >= 1);
    assert(dst_nodes.size() >= 1);

    assert(!g[src_nodes[0]].is_interface());
    assert(!g[dst_nodes[0]].is_interface());

    auto src_handles    = src_nodes | to_handles(g);
    auto src_node       = src_handles.front();
    auto src_interfaces = src_handles | rn::move | ra::drop(1);

    auto dst_handles    = dst_nodes | to_handles(g);
    auto dst_node       = dst_handles.front();
    auto dst_interfaces = dst_handles | rn::move | ra::drop(1);

    return connection_info(
      src_node,
      socket_handle(src, g.id(src)),
      dst_node,
      socket_handle(dst, g.id(dst)),
      src_interfaces,
      dst_interfaces);
  }

  auto node_graph::get_name(const node_handle& h) const
    -> std::optional<std::string>
  {
    if (!exists(h))
      return std::nullopt;

    return g[h.descriptor()].name();
  }

  auto node_graph::get_name(const socket_handle& h) const
    -> std::optional<std::string>
  {
    if (!exists(h))
      return std::nullopt;

    return g[h.descriptor()].name();
  }

  void node_graph::set_name(const node_handle& h, const std::string& name)
  {
    if (!exists(h))
      return;
    g[h.descriptor()].set_name(name);
  }

  void node_graph::set_name(const socket_handle& h, const std::string& name)
  {
    if (!exists(h))
      return;
    g[h.descriptor()].set_name(name);
  }

  auto node_graph::add(
    const std::string& name,
    const std::vector<std::string>& input_sockets,
    const std::vector<std::string>& output_sockets,
    const node_type& type,
    const uid& id) -> node_handle
  {
    assert(type == node_type::normal || type == node_type::interface);

    // add node
    auto node = g.add_node_with_id(id.data, name, type);

    if (!node)
      return {};

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
      return {};
    }

    auto handle = node_handle(node, g.id(node));

    return handle;
  }

  void node_graph::remove(const node_handle& node)
  {
    if (!exists(node))
      return;

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

  bool node_graph::attach_interface(
    const node_handle& interface,
    const socket_handle& socket)
  {
    if (!exists(interface) || !exists(socket))
      return false;

    if (!g[interface.descriptor()].is_interface())
      return false;

    auto info = get_info(socket);

    for (auto&& s : g.sockets(interface.descriptor())) {
      if (socket.id().data == g.id(s)) {
        return true; // already attached
      }
    }

    if (!g.attach_socket(interface.descriptor(), socket.descriptor()))
      return false;

    return true;
  }

  void node_graph::detach_interface(
    const node_handle& interface,
    const socket_handle& socket)
  {
    if (!exists(interface) || !exists(socket))
      return;

    auto info = get_info(socket);

    for (auto&& s : g.sockets(interface.descriptor())) {
      if (socket.id().data == g.id(s)) {
        g.detach_socket(interface.descriptor(), s);
        return;
      }
    }
  }

  auto node_graph::connect(
    const socket_handle& src_socket,
    const socket_handle& dst_socket,
    const uid& id) -> connection_handle
  {
    if (!exists(src_socket) || !exists(dst_socket))
      return {};

    // socket descriptors
    auto s = src_socket.descriptor();
    auto d = dst_socket.descriptor();

    // nodes
    auto sn       = g.nodes(s)[0];
    auto dn       = g.nodes(d)[0];
    auto src_node = node_handle(sn, g.id(sn));
    auto dst_node = node_handle(dn, g.id(dn));

    // check socket type
    if (!g[s].is_output() || !g[d].is_input())
      return {};

    // already exists
    for (auto&& e : g.dst_edges(d)) {
      if (g.src(e) == s) {
        return connection_handle(e, g.id(e));
      }
    }

    // add new edge to graph
    auto new_edge = g.add_edge_with_id(s, d, id.data);

    if (!new_edge) {
    };

    // closed loop check
    if (_find_loop(src_node)) {
      g.remove_edge(new_edge);
      return {};
    }

    return connection_handle(new_edge, g.id(new_edge));
  }

  void node_graph::disconnect(const connection_handle& h)
  {
    if (!exists(h))
      return;

    auto info = get_info(h);
    g.remove_edge(h.descriptor());
  }

  auto node_graph::node(const uid& id) const -> node_handle
  {
    auto dsc = g.node(id.data);

    if (!dsc)
      return {};

    return node_handle(dsc, id);
  }

  auto node_graph::node(const socket_handle& socket) const -> node_handle
  {
    if (!exists(socket))
      return {};

    auto dsc = socket.descriptor();
    auto n   = g.nodes(dsc)[0];
    return node_handle(n, g.id(n));
  }

  auto node_graph::nodes() const -> std::vector<node_handle>
  {
    auto&& ns = g.nodes();
    return ns | to_handles(g);
  }

  auto node_graph::nodes(const std::string& name) const
    -> std::vector<node_handle>
  {
    auto&& ns = g.nodes();

    return ns //
           | rv::filter([&](auto n) { return g[n].name() == name; })
           | to_handles(g);
  }

  auto node_graph::interfaces(const socket_handle& h) const
    -> std::vector<node_handle>
  {
    if (!exists(h))
      return {};

    auto&& ns = g.nodes(h.descriptor());
    return ns | rv::drop(1) | to_handles(g);
  }

  auto node_graph::socket(const uid& id) const -> socket_handle
  {
    auto dsc = g.socket(id.data);

    if (!dsc)
      return socket_handle();

    return socket_handle(dsc, id);
  }

  auto node_graph::sockets() const -> std::vector<socket_handle>
  {
    auto&& ss = g.sockets();
    return ss | to_handles(g);
  }

  auto node_graph::sockets(const node_handle& h) const
    -> std::vector<socket_handle>
  {
    if (!exists(h))
      return {};

    auto&& ss = g.sockets(h.descriptor());
    return ss | to_handles(g);
  }

  auto node_graph::sockets(const node_handle& h, socket_type type) const
    -> std::vector<socket_handle>
  {
    if (!exists(h))
      return {};

    auto&& sockets = g.sockets(h.descriptor());

    return sockets //
           | rv::filter([&](auto s) { return g[s].type() == type; })
           | to_handles(g);
  }

  auto node_graph::connection(const uid& id) const -> connection_handle
  {
    auto dsc = g.edge(id.data);

    if (!dsc)
      return connection_handle();

    return connection_handle(dsc, id);
  }

  auto node_graph::connections() const -> std::vector<connection_handle>
  {
    auto&& es = g.edges();
    return es | to_handles(g);
  }

  auto node_graph::connections(const node_handle& h) const
    -> std::vector<connection_handle>
  {
    if (!exists(h))
      return {};

    auto&& ss = g.sockets(h.descriptor());

    auto es = rv::concat(
                ss | rv::transform([&](auto s) { return g.src_edges(s); }),
                ss | rv::transform([&](auto s) { return g.dst_edges(s); }))
              | ra::join;

    return es | to_handles(g);
  }

  auto node_graph::connections(const socket_handle& h) const
    -> std::vector<connection_handle>
  {
    if (!exists(h))
      return {};

    auto&& se = g.src_edges(h.descriptor());
    auto&& de = g.dst_edges(h.descriptor());

    return rv::concat(se, de) | to_handles(g);
  }

  auto node_graph::connections(const node_handle& h, socket_type type) const
    -> std::vector<connection_handle>
  {
    if (!exists(h))
      return {};

    auto&& ss = g.sockets(h.descriptor());

    auto sss = ss | rv::filter([&](auto s) { return g[s].type() == type; });

    auto es = rv::concat(
                sss | rv::transform([&](auto s) { return g.src_edges(s); }),
                sss | rv::transform([&](auto s) { return g.dst_edges(s); }))
              | ra::join;

    return es | to_handles(g);
  }

  bool node_graph::has_connection(const socket_handle& h) const
  {
    if (!exists(h))
      return false;

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

  auto node_graph::type(const socket_handle& h) const
    -> std::optional<socket_type>
  {
    if (!exists(h))
      return std::nullopt;

    return g[h.descriptor()].type();
  }

  auto node_graph::type(const node_handle& h) const -> std::optional<node_type>
  {
    if (!exists(h))
      return std::nullopt;

    return g[h.descriptor()].type();
  }

  bool node_graph::has_type(const socket_handle& h, socket_type type) const
  {
    if (!exists(h))
      return false;

    return g[h.descriptor()].type() == type;
  }

  bool node_graph::has_type(const node_handle& h, node_type type) const
  {
    if (!exists(h))
      return false;

    return g[h.descriptor()].type() == type;
  }

  bool node_graph::has_data(const socket_handle& h) const
  {
    if (!exists(h))
      return false;

    return g[h.descriptor()].has_data();
  }

  bool node_graph::has_data(const node_handle& h) const
  {
    if (!exists(h))
      return false;

    return g[h.descriptor()].has_data();
  }

  auto node_graph::get_data(const socket_handle& h) const -> object_ptr<Object>
  {
    if (!exists(h))
      return {};

    auto data = g[h.descriptor()].get_data();
    return data ? *data : object_ptr();
  }

  auto node_graph::get_data(const node_handle& h) const -> object_ptr<Object>
  {
    if (!exists(h))
      return {};

    auto data = g[h.descriptor()].get_data();
    return data ? *data : object_ptr();
  }

  void node_graph::set_data(const socket_handle& h, object_ptr<Object> data)
  {
    if (!exists(h))
      return;

    g[h.descriptor()].set_data(std::move(data));
  }

  void node_graph::set_data(const node_handle& h, object_ptr<Object> data)
  {
    if (!exists(h))
      return;

    g[h.descriptor()].set_data(std::move(data));
  }

  auto node_graph::root_of(const node_handle& node) const
    -> std::vector<node_handle>
  {
    if (!exists(node))
      return {};

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

  auto node_graph::roots() const -> std::vector<node_handle>
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

  void node_graph::clear()
  {
    g.clear();
  }

  bool node_graph::empty() const
  {
    return g.empty();
  }

  auto node_graph::clone() const -> node_graph
  {
    return node_graph(g.clone());
  }

  // DFS to check loop
  bool node_graph::_find_loop(const node_handle& node) const
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

      for (auto&& s : sockets(top, socket_type::input)) {
        for (auto&& c : connections(s)) {
          auto srcd   = g.nodes(g.src(c.descriptor()))[0];
          auto src    = node_handle(srcd, g.id(srcd));
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

} // namespace yave