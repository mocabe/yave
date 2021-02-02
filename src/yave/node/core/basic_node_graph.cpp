//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/core/basic_node_graph.hpp>
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

    using graph_t = graph::
      graph<basic_node_property, basic_socket_property, basic_edge_property>;

    using ndesc_t = graph_t::node_descriptor_type;
    using sdesc_t = graph_t::socket_descriptor_type;
    using cdesc_t = graph_t::edge_descriptor_type;

    using ondesc_t = opaque_node_descriptor_type;
    using osdesc_t = opaque_socket_descriptor_type;
    using ocdesc_t = opaque_edge_descriptor_type;

    // get descriptor from opaque handle
    template <class Handle>
    [[nodiscard]] auto desc(const Handle& h)
    {
      /**/ if constexpr (std::is_same_v<Handle, node_handle>)
        return (ndesc_t)h.descriptor();
      else if constexpr (std::is_same_v<Handle, socket_handle>)
        return (sdesc_t)h.descriptor();
      else if constexpr (std::is_same_v<Handle, connection_handle>)
        return (cdesc_t)h.descriptor();
      else
        static_assert(false_v<Handle>);
    }

    // convert descriptor to handle
    template <class Descriptor>
    [[nodiscard]] auto hndl(Descriptor dsc, uid id)
    {
      /**/ if constexpr (std::is_same_v<Descriptor, ndesc_t>)
        return descriptor_handle((ondesc_t)dsc, id);
      else if constexpr (std::is_same_v<Descriptor, sdesc_t>)
        return descriptor_handle((osdesc_t)dsc, id);
      else if constexpr (std::is_same_v<Descriptor, cdesc_t>)
        return descriptor_handle((ocdesc_t)dsc, id);
      else
        static_assert(false_v<Descriptor>);
    }

    // convert descriptor to opaque handle
    template <class Descriptor>
    [[nodiscard]] auto hndl(Descriptor dsc, const graph_t& g)
    {
      return hndl(dsc, uid {g.id(dsc)});
    }

    // convert descriptors to handles
    constexpr auto to_handles = [](const graph_t& graph) {
      return rv::transform([&](auto&& n) { return hndl(n, graph); })
             | rn::to_vector;
    };

  } // namespace

  basic_node_graph::basic_node_graph(graph_t&& gg) noexcept
    : g {std::move(gg)}
  {
  }

  bool basic_node_graph::exists(const node_handle& h) const
  {
    if (!h.has_value())
      return false;

    if (auto d = g.node(h.id().data))
      return d == desc(h);

    return false;
  }

  bool basic_node_graph::exists(const socket_handle& h) const
  {
    if (!h.has_value())
      return false;

    if (auto d = g.socket(h.id().data))
      return d == desc(h);

    return false;
  }

  bool basic_node_graph::exists(const connection_handle& h) const
  {
    if (!h.has_value())
      return false;

    if (auto d = g.edge(h.id().data))
      return d == desc(h);

    return false;
  }

  auto basic_node_graph::get_info(const node_handle& h) const
    -> std::optional<basic_node_info>
  {
    if (!exists(h))
      return std::nullopt;

    auto&& ss = g.sockets(desc(h));

    auto iss = ss //
               | rv::filter([&](auto s) { return g[s].is_input(); })
               | to_handles(g);

    auto oss = ss //
               | rv::filter([&](auto s) { return g[s].is_output(); })
               | to_handles(g);

    auto&& n = g[desc(h)];

    return basic_node_info(n.name(), iss, oss, n.type());
  }

  auto basic_node_graph::get_info(const socket_handle& h) const
    -> std::optional<basic_socket_info>
  {
    if (!exists(h))
      return std::nullopt;

    auto&& ns = g.nodes(desc(h));

    auto nodes      = ns | to_handles(g);
    auto node       = nodes.front();
    auto interfaces = nodes | rn::move | ra::drop(1);

    auto s = g[desc(h)];

    return basic_socket_info(s.name(), s.type(), node, interfaces);
  }

  auto basic_node_graph::get_info(const connection_handle& h) const
    -> std::optional<basic_connection_info>
  {
    if (!exists(h))
      return std::nullopt;

    auto src = g.src(desc(h));
    auto dst = g.dst(desc(h));

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

    return basic_connection_info(
      src_node,
      hndl(src, g),
      dst_node,
      hndl(dst, g),
      src_interfaces,
      dst_interfaces);
  }

  auto basic_node_graph::get_name(const node_handle& h) const
    -> std::optional<std::string>
  {
    if (!exists(h))
      return std::nullopt;

    return g[desc(h)].name();
  }

  auto basic_node_graph::get_name(const socket_handle& h) const
    -> std::optional<std::string>
  {
    if (!exists(h))
      return std::nullopt;

    return g[desc(h)].name();
  }

  void basic_node_graph::set_name(const node_handle& h, const std::string& name)
  {
    if (!exists(h))
      return;

    g[desc(h)].set_name(name);
  }

  void basic_node_graph::set_name(
    const socket_handle& h,
    const std::string& name)
  {
    if (!exists(h))
      return;

    g[desc(h)].set_name(name);
  }

  auto basic_node_graph::add(
    const std::string& name,
    const std::vector<std::string>& input_sockets,
    const std::vector<std::string>& output_sockets,
    const basic_node_type& type,
    const uid& id) -> node_handle
  {
    assert(
      type == basic_node_type::normal || type == basic_node_type::interface);

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

    auto attach_i =
      _add_attach_sockets(input_sockets, basic_socket_type::input);
    auto attach_o =
      _add_attach_sockets(output_sockets, basic_socket_type::output);

    if (!attach_i || !attach_o) {
      // rollback changes
      for (auto&& s : g.sockets(node)) {
        g.remove_socket(s);
      }
      g.remove_node(node);
      return {};
    }

    return hndl(node, g);
  }

  void basic_node_graph::remove(const node_handle& h)
  {
    if (!exists(h))
      return;

    for (auto&& s : g.sockets(desc(h))) {
      // detach
      g.detach_socket(desc(h), s);
      // delete socket if needed
      for (auto&& n : g.nodes(s)) {
        if (!g[n].is_interface())
          continue;
      }
      g.remove_socket(s);
    }

    g.remove_node(desc(h));
  }

  bool basic_node_graph::attach_interface(
    const node_handle& interface,
    const socket_handle& socket)
  {
    if (!exists(interface) || !exists(socket))
      return false;

    if (!g[desc(interface)].is_interface())
      return false;

    auto info = get_info(socket);

    for (auto&& s : g.sockets(desc(interface))) {
      if (socket.id().data == g.id(s)) {
        return true; // already attached
      }
    }

    if (!g.attach_socket(desc(interface), desc(socket)))
      return false;

    return true;
  }

  void basic_node_graph::detach_interface(
    const node_handle& interface,
    const socket_handle& socket)
  {
    if (!exists(interface) || !exists(socket))
      return;

    auto info = get_info(socket);

    for (auto&& s : g.sockets(desc(interface))) {
      if (socket.id().data == g.id(s)) {
        g.detach_socket(desc(interface), s);
        return;
      }
    }
  }

  auto basic_node_graph::connect(
    const socket_handle& src_socket,
    const socket_handle& dst_socket,
    const uid& id) -> connection_handle
  {
    if (!exists(src_socket) || !exists(dst_socket))
      return {};

    // socket descriptors
    auto s = desc(src_socket);
    auto d = desc(dst_socket);

    // nodes
    auto sn       = g.nodes(s)[0];
    auto dn       = g.nodes(d)[0];
    auto src_node = hndl(sn, g);
    auto dst_node = hndl(dn, g);

    // check socket type
    if (!g[s].is_output() || !g[d].is_input())
      return {};

    // already exists
    for (auto&& e : g.dst_edges(d)) {
      if (g.src(e) == s) {
        return hndl(e, g);
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

    return hndl(new_edge, g);
  }

  void basic_node_graph::disconnect(const connection_handle& h)
  {
    if (!exists(h))
      return;

    auto info = get_info(h);
    g.remove_edge(desc(h));
  }

  auto basic_node_graph::node(const uid& id) const -> node_handle
  {
    auto dsc = g.node(id.data);

    if (!dsc)
      return {};

    return hndl(dsc, id);
  }

  auto basic_node_graph::node(const socket_handle& socket) const -> node_handle
  {
    if (!exists(socket))
      return {};

    auto n = g.nodes(desc(socket))[0];
    return hndl(n, g);
  }

  auto basic_node_graph::nodes() const -> std::vector<node_handle>
  {
    auto&& ns = g.nodes();
    return ns | to_handles(g);
  }

  auto basic_node_graph::nodes(const std::string& name) const
    -> std::vector<node_handle>
  {
    auto&& ns = g.nodes();

    return ns //
           | rv::filter([&](auto n) { return g[n].name() == name; })
           | to_handles(g);
  }

  auto basic_node_graph::interfaces(const socket_handle& h) const
    -> std::vector<node_handle>
  {
    if (!exists(h))
      return {};

    auto&& ns = g.nodes(desc(h));
    return ns | rv::drop(1) | to_handles(g);
  }

  auto basic_node_graph::socket(const uid& id) const -> socket_handle
  {
    auto dsc = g.socket(id.data);

    if (!dsc)
      return socket_handle();

    return hndl(dsc, id);
  }

  auto basic_node_graph::sockets() const -> std::vector<socket_handle>
  {
    auto&& ss = g.sockets();
    return ss | to_handles(g);
  }

  auto basic_node_graph::sockets(const node_handle& h) const
    -> std::vector<socket_handle>
  {
    if (!exists(h))
      return {};

    auto&& ss = g.sockets(desc(h));
    return ss | to_handles(g);
  }

  auto basic_node_graph::sockets(const node_handle& h, basic_socket_type type)
    const -> std::vector<socket_handle>
  {
    if (!exists(h))
      return {};

    auto&& sockets = g.sockets(desc(h));

    return sockets //
           | rv::filter([&](auto s) { return g[s].type() == type; })
           | to_handles(g);
  }

  auto basic_node_graph::i_sockets(const node_handle& node) const
    -> std::vector<socket_handle>
  {
    return sockets(node, basic_socket_type::input);
  }

  auto basic_node_graph::o_sockets(const node_handle& node) const
    -> std::vector<socket_handle>
  {
    return sockets(node, basic_socket_type::output);
  }

  auto basic_node_graph::connection(const uid& id) const -> connection_handle
  {
    auto dsc = g.edge(id.data);

    if (!dsc)
      return connection_handle();

    return hndl(dsc, id);
  }

  auto basic_node_graph::connections() const -> std::vector<connection_handle>
  {
    auto&& es = g.edges();
    return es | to_handles(g);
  }

  auto basic_node_graph::connections(const node_handle& h) const
    -> std::vector<connection_handle>
  {
    if (!exists(h))
      return {};

    auto&& ss = g.sockets(desc(h));

    auto es = rv::concat(
                ss | rv::transform([&](auto s) { return g.src_edges(s); }),
                ss | rv::transform([&](auto s) { return g.dst_edges(s); }))
              | ra::join;

    return es | to_handles(g);
  }

  auto basic_node_graph::connections(const socket_handle& h) const
    -> std::vector<connection_handle>
  {
    if (!exists(h))
      return {};

    auto&& se = g.src_edges(desc(h));
    auto&& de = g.dst_edges(desc(h));

    return rv::concat(se, de) | to_handles(g);
  }

  auto basic_node_graph::connections(
    const node_handle& h,
    basic_socket_type type) const -> std::vector<connection_handle>
  {
    if (!exists(h))
      return {};

    auto&& ss = g.sockets(desc(h));

    auto sss = ss | rv::filter([&](auto s) { return g[s].type() == type; });

    auto es = rv::concat(
                sss | rv::transform([&](auto s) { return g.src_edges(s); }),
                sss | rv::transform([&](auto s) { return g.dst_edges(s); }))
              | ra::join;

    return es | to_handles(g);
  }

  auto basic_node_graph::i_connections(const node_handle& socket) const
    -> std::vector<connection_handle>
  {
    return connections(socket, basic_socket_type::input);
  }

  auto basic_node_graph::o_connections(const node_handle& socket) const
    -> std::vector<connection_handle>
  {
    return connections(socket, basic_socket_type::output);
  }

  bool basic_node_graph::has_connection(const socket_handle& h) const
  {
    if (!exists(h))
      return false;

    if (!g.src_edges(desc(h)).empty()) {
      assert(g[desc(h)].is_output());
      return true;
    }
    if (!g.dst_edges(desc(h)).empty()) {
      assert(g[desc(h)].is_input());
      return true;
    }
    return false;
  }

  auto basic_node_graph::type(const socket_handle& h) const
    -> std::optional<basic_socket_type>
  {
    if (!exists(h))
      return std::nullopt;

    return g[desc(h)].type();
  }

  auto basic_node_graph::type(const node_handle& h) const
    -> std::optional<basic_node_type>
  {
    if (!exists(h))
      return std::nullopt;

    return g[desc(h)].type();
  }

  bool basic_node_graph::has_type(
    const socket_handle& h,
    basic_socket_type type) const
  {
    if (!exists(h))
      return false;

    return g[desc(h)].type() == type;
  }

  bool basic_node_graph::has_type(const node_handle& h, basic_node_type type)
    const
  {
    if (!exists(h))
      return false;

    return g[desc(h)].type() == type;
  }

  bool basic_node_graph::has_data(const socket_handle& h) const
  {
    if (!exists(h))
      return false;

    return g[desc(h)].has_data();
  }

  bool basic_node_graph::has_data(const node_handle& h) const
  {
    if (!exists(h))
      return false;

    return g[desc(h)].has_data();
  }

  auto basic_node_graph::get_data(const socket_handle& h) const
    -> object_ptr<Object>
  {
    if (!exists(h))
      return {};

    auto data = g[desc(h)].get_data();
    return data ? *data : object_ptr();
  }

  auto basic_node_graph::get_data(const node_handle& h) const
    -> object_ptr<Object>
  {
    if (!exists(h))
      return {};

    auto data = g[desc(h)].get_data();
    return data ? *data : object_ptr();
  }

  void basic_node_graph::set_data(
    const socket_handle& h,
    object_ptr<Object> data)
  {
    if (!exists(h))
      return;

    g[desc(h)].set_data(std::move(data));
  }

  void basic_node_graph::set_data(const node_handle& h, object_ptr<Object> data)
  {
    if (!exists(h))
      return;

    g[desc(h)].set_data(std::move(data));
  }

  auto basic_node_graph::root_of(const node_handle& node) const
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
      for (auto&& s : g.sockets(desc(n))) {
        for (auto&& e : g.src_edges(s)) {
          auto dst_s = g.dst(e);
          auto dst_n = g.nodes(dst_s);
          assert(dst_n.size() == 1);
          stack.push_back(hndl(dst_n[0], g));
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

  auto basic_node_graph::roots() const -> std::vector<node_handle>
  {
    std::vector<node_handle> ret;

    // TODO: Improve performance.
    for (auto&& n : g.nodes()) {
      for (auto&& root : root_of(hndl(n, g))) {
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

  void basic_node_graph::clear()
  {
    g.clear();
  }

  bool basic_node_graph::empty() const
  {
    return g.empty();
  }

  auto basic_node_graph::clone() const -> basic_node_graph
  {
    return basic_node_graph(g.clone());
  }

  // DFS to check loop
  bool basic_node_graph::_find_loop(const node_handle& node) const
  {
    std::vector<node_handle> stack;

    // clear flags before use
    for (auto&& n : nodes()) {
      g[desc(n)].set_flags(0);
    }

    // flag bits
    uint8_t visited_bit = 1 << 0;
    uint8_t on_path_bit = 1 << 1;

    stack.push_back(node);

    while (!stack.empty()) {
      auto top  = stack.back();
      auto topd = desc(top);

      g[topd].set_flags(visited_bit | on_path_bit);

      for (auto&& s : sockets(top, basic_socket_type::input)) {
        for (auto&& c : connections(s)) {
          auto srcd   = g.nodes(g.src(desc(c)))[0];
          auto src    = hndl(srcd, g);
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