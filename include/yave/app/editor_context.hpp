//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/managed_node_graph.hpp>
#include <yave/app/node_data_thread.hpp>
#include <yave/app/editor_node_info.hpp>
#include <yave/app/editor_socket_info.hpp>
#include <yave/app/editor_connection_info.hpp>

namespace yave::app {

  /// Editor context.
  class editor_context
  {
  public:
    editor_context(node_data_thread& data_thread);

    editor_context(const editor_context&) = delete;
    editor_context& operator=(const editor_context&) = delete;

  public: /* const interface */
    /// Accessing node graph snapshot
    auto node_graph() const -> const managed_node_graph&;

  public: /* state control */
    /// Begin frame
    void begin_frame();
    /// End frame
    void end_frame();

  public: /* non-const interface */
    /// Create node node
    void create(
      const std::string& name,
      const node_handle& group,
      const tvec2<float>& pos);

    /// Destroy existing node
    void destroy(const node_handle& node);

    /// Connect nodes
    void connect(
      const socket_handle& src_socket,
      const socket_handle& dst_socket);

    /// Disconnect
    void disconnect(const connection_handle& c);

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
    /// Get current node group
    auto get_group() const -> node_handle;
    /// Set next node group
    void set_group(const node_handle& node);

    /// Get position of node
    auto get_pos(const node_handle& node) const -> std::optional<tvec2<float>>;
    /// Set new position of node
    void set_pos(const node_handle& node, const tvec2<float>& new_pos);

    /// Get current scroll position
    auto get_scroll() const -> tvec2<float>;
    /// Set scroll position
    void set_scroll(const tvec2<float>& new_pos);

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
    /// Clear hovered all
    void clear_hovered();

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
    /// Clear selected all
    void clear_selected();

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

    /// Selected nodes
    auto get_selected_nodes() const -> std::vector<node_handle>;
    /// Selected sockets
    auto get_selected_sockets() const -> std::vector<socket_handle>;
    /// Selected connections
    auto get_selected_connections() const -> std::vector<connection_handle>;

    /// Hovered node
    auto get_hovered_node() const -> node_handle;
    /// Hovered socket
    auto get_hovered_socket() const -> socket_handle;
    /// Hovered connection
    auto get_hovered_connection() const -> connection_handle;

  private:
    /// data thread ref
    node_data_thread& m_data_thread;
    /// current snapshot of graph
    std::shared_ptr<const node_data_snapshot> m_snapshot;

  private:
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

  private:
    tvec2<float> m_scroll_pos;

  private:
    bool m_in_frame;
  };
} // namespace yave::app