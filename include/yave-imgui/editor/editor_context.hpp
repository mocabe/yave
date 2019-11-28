//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/managed_node_graph.hpp>
#include <yave-imgui/editor/node_info.hpp>
#include <yave-imgui/editor/socket_info.hpp>
#include <yave-imgui/editor/connection_info.hpp>

namespace yave::editor::imgui {

  /// Editor context.
  /// Works like "View-Model" in MVVM archtecture. Manages commands for data and
  /// rendering info. Non-const operations are deferred until next frame. So
  /// most of them just return `void`.
  class editor_context
  {
  public:
    editor_context();
    editor_context(const editor_context&) = delete;
    editor_context(editor_context&&) noexcept;
    editor_context& operator=(const editor_context&) = delete;
    editor_context& operator=(editor_context&&) noexcept;

  public: /* const interface */
    /// Accessing node graph snapshot
    auto node_graph() const -> const managed_node_graph&;

  public: /* non-const interface */
    /// Create node node
    void create(
      const std::string& name,
      const node_handle& group,
      const ImVec2& pos);
    /// Destroy existing node
    void destroy(const node_handle& node);

    /// Connect nodes
    void connect(
      const socket_handle& src_socket,
      const socket_handle& dst_socket);

    /// Disconnect
    void disconnect(const connection_handle& c);

    /// Set primitive value
    auto get_primitive(const node_handle& node) -> std::optional<primitive_t>;

    /// Get primitive value
    void set_primitive(const node_handle& node, const primitive_t& primitive);

  public: /* state */
    /// Begin frame
    void begin_frame();
    /// End frame
    void end_frame();

  public: /* model data accessors */
    /// Get editor node info
    auto get_editor_info(const node_handle& handle) const
      -> std::optional<editor_node_info>;
    /// Get editor socket info
    auto get_editor_info(const socket_handle& handle) const
      -> std::optional<editor_socket_info>;
    /// Get editor connection
    auto get_editor_info(const connection_handle& handle) const
      -> std::optional<editor_connection_info>;

  public: /* model data accessors */
    /// Get position of node
    auto get_pos(const node_handle& node) const -> std::optional<ImVec2>;
    /// Set new position of node
    void set_pos(const node_handle& node, const ImVec2& new_pos);

    /// Set as hovered
    void set_hovered(const node_handle& node);
    /// Set as hovered
    void set_hovered(const socket_handle& socket);
    /// Set as hovered
    void set_hovered(const connection_handle& connection);

    /// Clear hovered
    void clear_hovered_node();
    /// Clear hovered
    void clear_hovered_socket();
    /// Clear hovered
    void clear_hovered_connection();

    /// Hovered?
    bool is_node_hovered() const;
    /// Hovered?
    bool is_socket_hovered() const;
    /// Hovered?
    bool is_connection_hovered() const;

    /// Hovered?
    bool is_hovered(const node_handle& node) const;
    /// Hovered?
    bool is_hovered(const socket_handle& socket) const;
    /// Hovered?
    bool is_hovered(const connection_handle& connection) const;

    /// Add to selected list
    void add_selected(const node_handle& node);
    /// Add to selected list
    void add_selected(const socket_handle& socket);
    /// Add to selected list
    void add_selected(const connection_handle& connection);

    /// Remove from selected list
    void remove_selected(const node_handle& node);
    /// Remove from selected list
    void remove_selected(const socket_handle& socket);
    /// Remove from selected list
    void remove_selected(const connection_handle& connection);

    /// Clear selected list
    void clear_selected_nodes();
    /// Clear selected list
    void clear_selected_sockets();
    /// Clear selected list
    void clear_selected_connections();

    /// Selected?
    bool is_node_selected() const;
    /// Selected?
    bool is_socket_selected() const;
    /// Selected?
    bool is_connection_selected() const;

    /// Selected?
    bool is_selected(const node_handle& node) const;
    /// Selected?
    bool is_selected(const socket_handle& socket) const;
    /// Selected?
    bool is_selected(const connection_handle& connection) const;

  private:
    /// snapshot of graph
    managed_node_graph m_graph;

  private:
    /// current group
    node_handle m_current_group;

  private:
    std::vector<std::function<void(void)>> m_command_queue;

  private: /* hovered handles */
    node_handle m_n_hovered;
    socket_handle m_s_hovered;
    connection_handle m_c_hovered;

  private: /* selected handles */
    std::vector<node_handle> m_n_selected;
    std::vector<socket_handle> m_s_selected;
    std::vector<connection_handle> m_c_selected;

  private: /* attributes */
    /// view model node attributes
    struct _n_attr
    {
      ImVec2 pos;
    };

    /// view model socket attributes
    struct _s_attr
    {
    };

    /// view model connection attributes
    struct _c_attr
    {
    };

    auto _get_attr(const node_handle&) const -> const _n_attr&;
    auto _get_attr(const node_handle&) -> _n_attr&;
    auto _get_attr(const socket_handle&) const -> const _s_attr&;
    auto _get_attr(const socket_handle&) -> _s_attr&;
    auto _get_attr(const connection_handle&) const -> const _c_attr&;
    auto _get_attr(const connection_handle&) -> _c_attr&;

    /// view model information
    std::map<node_handle, _n_attr> m_n_attrs;
    std::map<socket_handle, _s_attr> m_s_attrs;
    std::map<connection_handle, _c_attr> m_c_attrs;
  };
}