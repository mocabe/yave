//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave-imgui/editor/editor_context.hpp>
#include <yave-imgui/editor/style.hpp>
#include <yave/support/log.hpp>
#include <range/v3/algorithm.hpp>

YAVE_DECL_G_LOGGER(editor_context);

namespace yave::editor::imgui {

  editor_context::editor_context()
    : m_graph {}
  {
    init_logger();

    m_current_group = m_graph.root_group();

    Info(g_logger, "Initialized editor context");
    Info(g_logger, "current group: {}", to_string(m_current_group.id()));
  }

  void editor_context::begin_frame()
  {
    // query updates from model
  }

  void editor_context::end_frame()
  {
    // apply view-model changes
    for (auto&& cmd : m_command_queue) {
      cmd();
    }
    m_command_queue.clear();
  }

  auto editor_context::node_graph() const -> const managed_node_graph&
  {
    return m_graph;
  }

  auto editor_context::get_pos(const node_handle& node) const
    -> std::optional<ImVec2>
  {
    if (!m_graph.exists(node))
      return std::nullopt;

    return _get_attr(node).pos;
  }

  void editor_context::set_pos(const node_handle& node, const ImVec2& new_pos)
  {
    Info(
      g_logger,
      "Set new position: node={}, pos=({},{})",
      to_string(node.id()),
      new_pos.x,
      new_pos.y);

    m_command_queue.push_back([&]() {
      if (m_graph.exists(node))
        _get_attr(node).pos = new_pos;
    });
  }

  auto editor_context::get_editor_info(const node_handle& handle) const
    -> std::optional<editor_node_info>
  {
    if (!m_graph.exists(handle))
      return std::nullopt;

    auto attr = _get_attr(handle);

    return editor_node_info {
      handle, is_selected(handle), is_hovered(handle), attr.pos};
  }

  auto editor_context::get_editor_info(const socket_handle& handle) const
    -> std::optional<editor_socket_info>
  {
    if (!m_graph.exists(handle))
      return std::nullopt;

    return editor_socket_info {handle, is_selected(handle), is_hovered(handle)};
  }

  auto editor_context::get_editor_info(const connection_handle& handle) const
    -> std::optional<editor_connection_info>
  {
    if (!m_graph.exists(handle))
      return std::nullopt;

    return editor_connection_info {
      handle, is_selected(handle), is_hovered(handle)};
  }

  void editor_context::create(
    const std::string& name,
    const node_handle& group,
    const ImVec2& pos)
  {
    Info(g_logger, "Create new node at ({},{})", pos.x, pos.y);

    // FIXME: async
    m_command_queue.push_back([&]() {
      auto n = m_graph.create(group, name);
      if (n)
        m_n_attrs.emplace(n, _n_attr {pos});
    });
  }

  void editor_context::destroy(const node_handle& node)
  {
    // FIXME: async
    m_command_queue.push_back([&]() {
      m_graph.destroy(node);
      m_n_attrs.erase(node);
    });
  }

  void editor_context::connect(
    const socket_handle& src_socket,
    const socket_handle& dst_socket)
  {
    // FIXME: async
    m_command_queue.push_back([&]() {
      auto c = m_graph.connect(src_socket, dst_socket);
      if (c)
        m_c_attrs.emplace(c, _c_attr {});
    });
  }

  void editor_context::disconnect(const connection_handle& c)
  {
    // FIXME: async
    m_command_queue.push_back([&]() {
      m_graph.disconnect(c);
      m_c_attrs.erase(c);
    });
  }

  auto editor_context::get_primitive(const node_handle& node)
    -> std::optional<primitive_t>
  {
    // FIXME: async
    return m_graph.get_primitive(node);
  }

  void editor_context::set_primitive(
    const node_handle& node,
    const primitive_t& primitive)
  {
    // FIXME: async
    m_command_queue.push_back(
      [&]() { (void)m_graph.set_primitive(node, primitive); });
  }

  void editor_context::set_hovered(const node_handle& handle)
  {
    m_command_queue.push_back([&]() {
      if (!m_graph.exists(handle))
        return;

      if (is_hovered(handle))
        return;

      Info(g_logger, "Set hovered node: id={}", to_string(handle.id()));
      m_n_hovered = handle;
    });
  }

  void editor_context::set_hovered(const socket_handle& handle)
  {
    m_command_queue.push_back([&]() {
      if (!m_graph.exists(handle))
        return;

      if (is_hovered(handle))
        return;

      Info(g_logger, "Set hovered socket: id={}", to_string(handle.id()));
      m_s_hovered = handle;
    });
  }

  void editor_context::set_hovered(const connection_handle& handle)
  {
    m_command_queue.push_back([&]() {
      if (!m_graph.exists(handle))
        return;

      if (is_hovered(handle))
        return;

      Info(g_logger, "Set hovered connection: id={}", to_string(handle.id()));
      m_c_hovered = handle;
    });
  }

  void editor_context::clear_hovered_node()
  {
    m_command_queue.push_back([&]() { m_n_hovered = {nullptr}; });
  }

  void editor_context::clear_hovered_socket()
  {
    m_command_queue.push_back([&]() { m_s_hovered = {nullptr}; });
  }

  void editor_context::clear_hovered_connection()
  {
    m_command_queue.push_back([&]() { m_c_hovered = {nullptr}; });
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

  bool editor_context::is_hovered(const node_handle& handle) const
  {
    return m_n_hovered == handle;
  }

  bool editor_context::is_hovered(const socket_handle& handle) const
  {
    return m_s_hovered == handle;
  }

  bool editor_context::is_hovered(const connection_handle& handle) const
  {
    return m_c_hovered == handle;
  }

  void editor_context::add_selected(const node_handle& handle)
  {
    m_command_queue.push_back([&]() {
      if (!m_graph.exists(handle))
        return;

      if (ranges::find(m_n_selected, handle) != m_n_selected.end())
        return;

      Info(g_logger, "Select node: id={}", to_string(handle.id()));
      m_n_selected.push_back(handle);
    });
  }

  void editor_context::add_selected(const socket_handle& handle)
  {
    m_command_queue.push_back([&]() {
      if (!m_graph.exists(handle))
        return;

      if (ranges::find(m_s_selected, handle) != m_s_selected.end())
        return;

      Info(g_logger, "Select socket: id={}", to_string(handle.id()));
      m_s_selected.push_back(handle);
    });
  }

  void editor_context::add_selected(const connection_handle& handle)
  {
    m_command_queue.push_back([&]() {
      if (!m_graph.exists(handle))
        return;

      if (ranges::find(m_c_selected, handle) != m_c_selected.end())
        return;

      Info(g_logger, "Select connection: id={}", to_string(handle.id()));
      m_c_selected.push_back(handle);
    });
  }

  void editor_context::clear_selected_nodes()
  {
    m_command_queue.push_back([&]() { m_n_selected.clear(); });
  }

  void editor_context::clear_selected_sockets()
  {
    m_command_queue.push_back([&]() { m_s_selected.clear(); });
  }

  void editor_context::clear_selected_connections()
  {
    m_command_queue.push_back([&]() { m_c_selected.clear(); });
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

  bool editor_context::is_selected(const node_handle& handle) const
  {
    return ranges::find(m_n_selected, handle) != m_n_selected.end();
  }

  bool editor_context::is_selected(const socket_handle& handle) const
  {
    return ranges::find(m_s_selected, handle) != m_s_selected.end();
  }

  bool editor_context::is_selected(const connection_handle& handle) const
  {
    return ranges::find(m_c_selected, handle) != m_c_selected.end();
  }

  auto editor_context::_get_attr(const node_handle& handle) const
    -> const _n_attr&
  {
    auto iter = m_n_attrs.find(handle);

    assert(m_graph.exists(handle));
    assert(iter != m_n_attrs.end());

    return iter->second;
  }

  auto editor_context::_get_attr(const node_handle& handle) -> _n_attr&
  {
    auto iter = m_n_attrs.find(handle);

    assert(m_graph.exists(handle));
    assert(iter != m_n_attrs.end());

    return iter->second;
  }

  auto editor_context::_get_attr(const socket_handle& handle) const
    -> const _s_attr&
  {
    auto iter = m_s_attrs.find(handle);

    assert(m_graph.exists(handle));
    assert(iter != m_s_attrs.end());

    return iter->second;
  }

  auto editor_context::_get_attr(const socket_handle& handle) -> _s_attr&
  {
    auto iter = m_s_attrs.find(handle);

    assert(m_graph.exists(handle));
    assert(iter != m_s_attrs.end());

    return iter->second;
  }

  auto editor_context::_get_attr(const connection_handle& handle) const
    -> const _c_attr&
  {
    auto iter = m_c_attrs.find(handle);

    assert(m_graph.exists(handle));
    assert(iter != m_c_attrs.end());

    return iter->second;
  }

  auto editor_context::_get_attr(const connection_handle& handle) -> _c_attr&
  {
    auto iter = m_c_attrs.find(handle);

    assert(m_graph.exists(handle));
    assert(iter != m_c_attrs.end());

    return iter->second;
  }
} // namespace yave::editor::imgui