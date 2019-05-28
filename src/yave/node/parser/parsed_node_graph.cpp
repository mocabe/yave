//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/parser/parsed_node_graph.hpp>

#include <cassert>

namespace yave {

  /* public */

  parsed_node_graph::parsed_node_graph()
  {
  }

  parsed_node_graph::parsed_node_graph(const parsed_node_graph& other)
    : m_graph {other.m_graph.clone()}
  {
    for (auto&& n : nodes()) {
      assert(exists(n));
      if (m_graph[n.descriptor()].is_root()) {
        m_roots.push_back(n);
      }
    }
  }

  parsed_node_graph::parsed_node_graph(parsed_node_graph&& other)
    : m_graph {std::move(other.m_graph)}
    , m_roots {std::move(other.m_roots)}
  {
  }

  parsed_node_graph::~parsed_node_graph()
  {
  }

  bool parsed_node_graph::exists(const parsed_node_handle& node) const
  {
    return m_graph.exists(node.descriptor());
  }

  bool
    parsed_node_graph::exists(const parsed_connection_handle& connection) const
  {
    return m_graph.exists(connection.descriptor());
  }

  std::vector<parsed_node_handle> parsed_node_graph::nodes() const
  {
    std::vector<parsed_node_handle> handles;
    for (auto&& d : m_graph.nodes()) {
      handles.emplace_back(d, m_graph.id(d));
    }
    return handles;
  }

  std::optional<parsed_node_info>
    parsed_node_graph::get_info(const parsed_node_handle& node) const
  {
    if (!exists(node))
      return std::nullopt;

    auto& property = m_graph[node.descriptor()];
    return parsed_node_info(
      property.instance(), property.type(), property.bind_info());
  }

  std::optional<parsed_connection_info> parsed_node_graph::get_info(
    const parsed_connection_handle& connection) const
  {
    if (!exists(connection))
      return std::nullopt;

    auto src = m_graph.src(connection.descriptor());
    auto dst = m_graph.dst(connection.descriptor());

    auto src_node = m_graph.nodes(src);
    assert(src_node.size() == 1);

    auto dst_node = m_graph.nodes(dst);
    assert(dst_node.size() == 1);

    auto srcd = parsed_node_handle(src_node[0], m_graph.id(src_node[0]));
    auto dstd = parsed_node_handle(dst_node[0], m_graph.id(dst_node[0]));

    return parsed_connection_info(srcd, dstd, m_graph[dst_node[0]].name());
  }

  std::vector<parsed_connection_handle> parsed_node_graph::input_connections(
    const parsed_node_handle& node,
    const std::string& socket) const
  {
    if (!exists(node))
      return {};

    for (auto&& s : m_graph.sockets(node.descriptor())) {
      auto e = m_graph.dst_edges(s);
      if (m_graph[s].name() == socket && !e.empty()) {
        assert(e.size() == 1);
        assert(m_graph[s].is_input());
        return {parsed_connection_handle(e[0], m_graph.id(e[0]))};
      }
    }
    return {};
  }

  std::vector<parsed_connection_handle>
    parsed_node_graph::input_connections(const parsed_node_handle& node) const
  {
    if (!exists(node))
      return {};

    std::vector<parsed_connection_handle> handles;
    for (auto&& s : m_graph.sockets(node.descriptor())) {
      auto e = m_graph.dst_edges(s);
      if (!e.empty()) {
        assert(e.size() == 1);
        assert(m_graph[s].is_input());
        handles.emplace_back(e[0], m_graph.id(e[0]));
      }
    }
    return handles;
  }

  std::vector<parsed_connection_handle>
    parsed_node_graph::output_connection(const parsed_node_handle& node) const
  {
    if (!exists(node))
      return {};

    std::vector<parsed_connection_handle> handles;

    for (auto&& s : m_graph.sockets(node.descriptor())) {
      auto es = m_graph.src_edges(s);

      if (!es.empty())
        assert(m_graph[s].is_output());

      for (auto&& e : es) {
        handles.emplace_back(e, m_graph.id(e));
      }
    }

    if (m_graph[node.descriptor()].is_root())
      assert(handles.empty());

    return handles;
  }

  std::vector<std::string>
    parsed_node_graph::output_socket(const parsed_node_handle& node) const
  {
    if (!exists(node))
      return {};

    std::vector<std::string> names;

    for (auto&& s : m_graph.sockets(node.descriptor())) {
      if (m_graph[s].is_output()) {
        names.push_back(m_graph[s].name());
      }
    }

    if (!names.empty())
      assert(names.size() == 1);

    return names;
  }

  std::vector<std::string>
    parsed_node_graph::input_sockets(const parsed_node_handle& node) const
  {
    if (!exists(node))
      return {};

    std::vector<std::string> names;

    for (auto&& s : m_graph.sockets(node.descriptor())) {
      if (m_graph[s].is_input()) {
        names.push_back(m_graph[s].name());
      }
    }

    return names;
  }

  std::vector<parsed_node_handle> parsed_node_graph::roots() const
  {
    for (auto&& n : m_roots) {
      assert(exists(n));
      assert(m_graph[n.descriptor()].is_root());
    }
    return m_roots;
  }

  std::unique_lock<std::mutex> parsed_node_graph::lock() const
  {
    return std::unique_lock {m_mtx};
  }

  /* private */

  parsed_node_handle parsed_node_graph::add(
    const object_ptr<const Object>& instance,
    const object_ptr<const Type>& type,
    const std::shared_ptr<const bind_info>& bind_info,
    bool is_root)
  {
    if (!instance)
      throw std::invalid_argument("Null instance");

    if (!type)
      throw std::invalid_argument("Null type");

    if (!bind_info)
      throw std::invalid_argument("Null bind info");

    auto node = m_graph.add_node(instance, type, bind_info, is_root);

    if (!node)
      throw std::runtime_error("Failed to create node");

    auto _attach_socket = [&](auto&& socket, auto io) {
      auto new_s = m_graph.add_socket(socket, io);
      if (!new_s)
        throw std::runtime_error("Failed to create socket");
      if (!m_graph.attach_socket(node, new_s))
        throw std::runtime_error("Failed to attach socket");
    };

    try {
      for (auto&& is : bind_info->input_sockets()) {
        _attach_socket(is, parsed_socket_property::input);
      }
      _attach_socket(
        bind_info->output_socket(), parsed_socket_property::output);
    } catch (...) {
      for (auto&& s : m_graph.sockets(node)) {
        m_graph.remove_socket(s);
      }
      m_graph.remove_node(node);
      throw;
    }

    return parsed_node_handle(node, m_graph.id(node));
  }

  void parsed_node_graph::remove(const parsed_node_handle& node)
  {
    if (!exists(node))
      return;
    m_graph.remove_node(node.descriptor());
  }

  void parsed_node_graph::remove_subtree(const parsed_node_handle& node)
  {
    if (!exists(node))
      return;

    struct
    {
      void rec(
        parsed_graph_t& graph,
        const typename parsed_graph_t::node_descriptor_type& n)
      {
        for (auto&& s : graph.sockets(n)) {
          if (graph[s].is_input()) {
            for (auto&& e : graph.dst_edges(s)) {
              for (auto&& srcn : graph.nodes(graph.src(e))) {
                rec(graph, srcn);
                for (auto&& srcs : graph.sockets(srcn)) {
                  graph.remove_socket(srcs);
                }
                graph.remove_node(srcn);
              }
            }
          }
        }
      }
    } impl;

    // remove subtree
    impl.rec(m_graph, node.descriptor());

    // remove root
    for (auto&& s : m_graph.sockets(node.descriptor())) {
      m_graph.remove_socket(s);
    }
    m_graph.remove_node(node.descriptor());
  }

  parsed_connection_handle
    parsed_node_graph::connect(const parsed_connection_info& info)
  {
    return connect(info.src_node(), info.dst_node(), info.dst_socket());
  }

  parsed_connection_handle parsed_node_graph::connect(
    const parsed_node_handle& src_node,
    const parsed_node_handle& dst_node,
    const std::string& dst_socket)
  {
    if (!exists(src_node) || !exists(dst_node))
      return nullptr;

    /* For internal use: NO closed loop check! */

    for (auto&& srcs : m_graph.sockets(src_node.descriptor())) {
      if (m_graph[srcs].is_output()) {
        for (auto&& dsts : m_graph.sockets(dst_node.descriptor())) {
          if (m_graph[dsts].name() == dst_socket) {
            auto e = m_graph.add_edge(srcs, dsts);
            return parsed_connection_handle(e, m_graph.id(e));
          }
        }
      }
    }
    return nullptr;
  }

  void parsed_node_graph::disconnect(const parsed_connection_handle& connection)
  {
    if (!exists(connection))
      return;
    m_graph.remove_edge(connection.descriptor());
  }

  void parsed_node_graph::set_root(const parsed_node_handle& node)
  {
    if (!exists(node))
      return;

    for (auto&& r : m_roots) {
      if (r == node)
        return;
    }

    if (!m_graph[node.descriptor()].is_root()) {
      m_graph[node.descriptor()].set_root();
      m_roots.push_back(node);
    }

    return;
  }

} // namespace yave