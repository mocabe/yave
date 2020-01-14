//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/app/editor_context.hpp>

#include <yave/support/log.hpp>

#include <algorithm>

YAVE_DECL_G_LOGGER(editor_context)

namespace yave::app {

  editor_context::editor_context(node_data_thread& data_thread)
    : m_data_thread {data_thread}
    , m_in_frame {false}
  {
    init_logger();

    // init snapshot
    m_snapshot = m_data_thread.snapshot();
  }

  void editor_context::begin_frame()
  {
    assert(!m_in_frame);

    // update snapshot
    m_snapshot = m_data_thread.snapshot();

    auto& g = m_snapshot->graph;

    // update group
    m_current_group = g.node(m_current_group.id());

    if (!g.is_group(m_current_group)) {
      m_current_group = g.root_group();
      m_scroll_pos    = {};
    }

    // reset hovered state
    m_n_hovered = {};
    m_s_hovered = {};
    m_c_hovered = {};

    // reset selected if not exists
    {
      auto it =
        std::remove_if(m_n_selected.begin(), m_n_selected.end(), [&](auto& h) {
          return !g.node(h.id());
        });
      m_n_selected.erase(it, m_n_selected.end());
    }
    {
      auto it =
        std::remove_if(m_s_selected.begin(), m_s_selected.end(), [&](auto& h) {
          return !g.socket(h.id());
        });
      m_s_selected.erase(it, m_s_selected.end());
    }
    {
      auto it =
        std::remove_if(m_c_selected.begin(), m_c_selected.end(), [&](auto& h) {
          return !g.connection(h.id());
        });
      m_c_selected.erase(it, m_c_selected.end());
    }

    // fix handles

    for (auto&& n : m_n_selected)
      n = g.node(n.id());

    for (auto&& s : m_s_selected)
      s = g.socket(s.id());

    for (auto&& c : m_c_selected)
      c = g.connection(c.id());

    m_in_frame = true;
  }

  void editor_context::end_frame()
  {
    // execute commands eunqueued
    for (auto&& cmd : m_command_queue) {
      cmd();
    }
    m_command_queue.clear();

    m_in_frame = false;
  }

  auto editor_context::node_graph() const -> const managed_node_graph&
  {
    return m_snapshot->graph;
  }

  void editor_context::create(
    const std::string& name,
    const node_handle& group,
    const tvec2<float>& pos)
  {
    assert(m_in_frame);

    auto func = [name, group, pos](managed_node_graph& g) {
      auto h = g.create(g.node(group.id()), name);
      g.set_pos(h, pos);
    };

    m_data_thread.send(node_data_thread_op_func {func});
  }

  void editor_context::connect(
    const socket_handle& src_socket,
    const socket_handle& dst_socket)
  {
    assert(m_in_frame);
    m_data_thread.send(node_data_thread_op_connect {src_socket, dst_socket});
  }

  void editor_context::disconnect(const connection_handle& c)
  {
    assert(m_in_frame);
    m_data_thread.send(node_data_thread_op_disconnect {c});
  }

  auto editor_context::get_editor_info(const node_handle& handle) const
    -> std::optional<editor_node_info>
  {
    assert(m_in_frame);
    auto& g = m_snapshot->graph;

    auto n = g.node(handle.id());

    if (!n)
      return std::nullopt;

    auto pos = g.get_pos(n);
    assert(pos);

    return editor_node_info {n, is_selected(n), is_hovered(n), *pos};
  }

  auto editor_context::get_editor_info(const socket_handle& handle) const
    -> std::optional<editor_socket_info>
  {
    auto& g = m_snapshot->graph;

    auto s = g.socket(handle.id());

    if (!s)
      return std::nullopt;

    return editor_socket_info {s, is_selected(s), is_hovered(s)};
  }

  auto editor_context::get_editor_info(const connection_handle& handle) const
    -> std::optional<editor_connection_info>
  {
    auto& g = m_snapshot->graph;

    auto c = g.connection(handle.id());

    if (!c)
      return std::nullopt;

    return editor_connection_info {c, is_selected(c), is_hovered(c)};
  }

  auto editor_context::get_group() const -> node_handle
  {
    return m_current_group;
  }

  void editor_context::set_group(const node_handle& node)
  {
    assert(m_in_frame);

    auto& g = m_snapshot->graph;

    m_command_queue.emplace_back([&, node] {
      auto n = g.node(node.id());
      if (g.is_group(n)) {
        m_current_group = n;
        m_scroll_pos    = {};
      }
    });
  }

  auto editor_context::get_pos(const node_handle& node) const
    -> std::optional<tvec2<float>>
  {
    auto& g = m_snapshot->graph;
    return g.get_pos(g.node(node.id()));
  }

  void editor_context::set_pos(
    const node_handle& node,
    const tvec2<float>& new_pos)
  {
    assert(m_in_frame);
    m_data_thread.send(node_data_thread_op_set_pos {node, new_pos});
  }

  auto editor_context::get_scroll() const -> tvec2<float>
  {
    return m_scroll_pos;
  }

  void editor_context::set_scroll(const tvec2<float>& new_pos)
  {
    assert(m_in_frame);
    m_command_queue.emplace_back([&, new_pos] { m_scroll_pos = new_pos; });
  }

  void editor_context::set_hovered(const node_handle& node)
  {
    assert(m_in_frame);
    m_command_queue.emplace_back([&, node] {
      auto& g = m_snapshot->graph;

      if (auto n = g.node(node.id()))
        m_n_hovered = n;
    });
  }

  void editor_context::set_hovered(const socket_handle& socket)
  {
    assert(m_in_frame);
    m_command_queue.emplace_back([&, socket] {
      auto& g = m_snapshot->graph;

      if (auto s = g.socket(socket.id()))
        m_s_hovered = s;
    });
  }

  void editor_context::set_hovered(const connection_handle& connection)
  {
    assert(m_in_frame);
    m_command_queue.emplace_back([&, connection] {
      auto& g = m_snapshot->graph;

      if (auto c = g.connection(connection.id()))
        m_c_hovered = c;
    });
  }

  void editor_context::clear_hovered_node()
  {
    assert(m_in_frame);
    m_command_queue.emplace_back([&] { m_n_hovered = {nullptr}; });
  }

  void editor_context::clear_hovered_socket()
  {
    assert(m_in_frame);
    m_command_queue.emplace_back([&] { m_s_hovered = {nullptr}; });
  }

  void editor_context::clear_hovered_connection()
  {
    assert(m_in_frame);
    m_command_queue.emplace_back([&] { m_c_hovered = {nullptr}; });
  }

  bool editor_context::is_node_hovered() const
  {
    return m_n_hovered.has_value();
  }

  bool editor_context::is_socket_hovered() const
  {
    return m_s_hovered.has_value();
  }

  bool editor_context::is_connection_hovered() const
  {
    return m_c_hovered.has_value();
  }

  bool editor_context::is_hovered(const node_handle& node) const
  {
    auto& g = m_snapshot->graph;

    if (m_n_hovered) {
      assert(g.exists(m_n_hovered));
      return m_n_hovered == g.node(node.id());
    }
    return false;
  }

  bool editor_context::is_hovered(const socket_handle& socket) const
  {
    auto& g = m_snapshot->graph;

    if (m_s_hovered) {
      assert(g.exists(m_s_hovered));
      return m_s_hovered == g.socket(socket.id());
    }
    return false;
  }

  bool editor_context::is_hovered(const connection_handle& connection) const
  {
    auto& g = m_snapshot->graph;

    if (m_c_hovered) {
      assert(g.exists(m_c_hovered));
      return m_c_hovered == g.connection(connection.id());
    }
    return false;
  }

  void editor_context::add_selected(const node_handle& node)
  {
    assert(m_in_frame);

    m_data_thread.send(node_data_thread_op_bring_front {node});

    m_command_queue.emplace_back([&, node] {
      auto& g = m_snapshot->graph;

      if (auto n = g.node(node.id())) {

        auto it = std::find(m_n_selected.begin(), m_n_selected.end(), n);

        if (it == m_n_selected.end())
          m_n_selected.push_back(n);
      }
    });
  }

  void editor_context::add_selected(const socket_handle& socket)
  {
    assert(m_in_frame);
    m_command_queue.emplace_back([&, socket] {
      auto& g = m_snapshot->graph;

      if (auto s = g.socket(socket.id())) {

        auto it = std::find(m_s_selected.begin(), m_s_selected.end(), s);

        if (it == m_s_selected.end())
          m_s_selected.push_back(s);
      }
    });
  }

  void editor_context::add_selected(const connection_handle& connection)
  {
    assert(m_in_frame);
    m_command_queue.emplace_back([&, connection] {
      auto& g = m_snapshot->graph;

      if (auto c = g.connection(connection.id())) {

        auto it = std::find(m_c_selected.begin(), m_c_selected.end(), c);

        if (it == m_c_selected.end())
          m_c_selected.push_back(c);
      }
    });
  }

  void editor_context::remove_selected(const node_handle& node)
  {
    assert(m_in_frame);
    m_command_queue.emplace_back([&, node] {
      auto& g = m_snapshot->graph;

      if (auto n = g.node(node.id())) {

        auto it = std::remove(m_n_selected.begin(), m_n_selected.end(), n);

        m_n_selected.erase(it, m_n_selected.end());
      }
    });
  }

  void editor_context::remove_selected(const socket_handle& socket)
  {
    assert(m_in_frame);
    m_command_queue.emplace_back([&, socket] {
      auto& g = m_snapshot->graph;

      if (auto s = g.socket(socket.id())) {

        auto it = std::remove(m_s_selected.begin(), m_s_selected.end(), s);

        m_s_selected.erase(it, m_s_selected.end());
      }
    });
  }

  void editor_context::remove_selected(const connection_handle& connection)
  {
    assert(m_in_frame);
    m_command_queue.emplace_back([&, connection] {
      auto& g = m_snapshot->graph;

      if (auto c = g.connection(connection.id())) {

        auto it = std::remove(m_c_selected.begin(), m_c_selected.end(), c);

        m_c_selected.erase(it, m_c_selected.end());
      }
    });
  }

  void editor_context::clear_selected_nodes()
  {
    assert(m_in_frame);
    m_command_queue.emplace_back([&] { m_n_selected.clear(); });
  }

  void editor_context::clear_selected_sockets()
  {
    assert(m_in_frame);
    m_command_queue.emplace_back([&] { m_s_selected.clear(); });
  }

  void editor_context::clear_selected_connections()
  {
    assert(m_in_frame);
    m_command_queue.emplace_back([&] { m_c_selected.clear(); });
  }

  bool editor_context::is_node_selected() const
  {
    return !m_n_selected.empty();
  }

  bool editor_context::is_socket_selected() const
  {
    return !m_s_selected.empty();
  }

  bool editor_context::is_connection_selected() const
  {
    return !m_c_selected.empty();
  }

  bool editor_context::is_selected(const node_handle& node) const
  {
    auto& g = m_snapshot->graph;

    if (auto n = g.node(node.id())) {
      auto it = std::find(m_n_selected.begin(), m_n_selected.end(), n);
      return it != m_n_selected.end();
    }
    return false;
  }

  bool editor_context::is_selected(const socket_handle& socket) const
  {
    auto& g = m_snapshot->graph;

    if (auto s = g.socket(socket.id())) {
      auto it = std::find(m_s_selected.begin(), m_s_selected.end(), s);
      return it != m_s_selected.end();
    }
    return false;
  }

  bool editor_context::is_selected(const connection_handle& connection) const
  {
    auto& g = m_snapshot->graph;

    if (auto c = g.connection(connection.id())) {
      auto it = std::find(m_c_selected.begin(), m_c_selected.end(), c);
      return it != m_c_selected.end();
    }
    return false;
  }

} // namespace yave::app