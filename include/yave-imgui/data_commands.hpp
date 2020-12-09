//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/editor/data_command.hpp>
#include <yave/lib/imgui/extension.hpp>
#include <yave/node/core/structured_node_graph.hpp>
#include <yave/obj/node/argument.hpp>

namespace yave::editor::imgui {

  /// push update
  struct dcmd_push_update : data_command
  {
    std::vector<node_argument_diff> m_diffs;

    dcmd_push_update(std::vector<node_argument_diff> diffs)
      : m_diffs {std::move(diffs)}
    {
    }

    void exec(data_context& ctx) override;
    void undo(data_context& ctx) override;
    auto type() const -> data_command_type override;
  };

  /// notify execute
  struct dcmd_notify_execute : data_command
  {
    void exec(data_context& ctx) override;
    void undo(data_context& ctx) override;
    auto type() const -> data_command_type override;
  };

  /// create_copy()
  struct dcmd_ncreate : data_command
  {
    // param
    ImVec2 pos;
    node_handle group;
    node_handle source;
    // data
    uid id;
    node_handle node;

    dcmd_ncreate(
      const ImVec2& pos,
      const node_handle& group,
      const node_handle& source);

    void exec(data_context& ctx) override;
    void undo(data_context& ctx) override;
    auto type() const -> data_command_type override;
  };

  /// create_gruop()
  struct dcmd_gcreate : data_command
  {
    // param
    ImVec2 pos;
    node_handle parent;
    // data
    uid id;
    node_handle group;

    dcmd_gcreate(const ImVec2& pos, const node_handle& parent);

    void exec(data_context& ctx) override;
    void undo(data_context& ctx) override;
    auto type() const -> data_command_type override;
  };

  /// destroy()
  struct dcmd_ndestroy : data_command
  {
    // param
    std::vector<node_handle> nodes;

    dcmd_ndestroy(std::vector<node_handle> ns);

    void exec(data_context& ctx) override;
    void undo(data_context& ctx) override;
    auto type() const -> data_command_type override;
  };

  /// group
  struct dcmd_ngroup : data_command
  {
    // param
    node_handle parent;
    std::vector<node_handle> nodes;

    dcmd_ngroup(node_handle p, std::vector<node_handle> ns);

    void exec(data_context& ctx) override;
    void undo(data_context& ctx) override;
    auto type() const -> data_command_type override;
  };

  /// connect()
  /// overwrites existing connection to dst socket.
  struct dcmd_connect : data_command
  {
    // param
    node_handle src_node;
    size_t src_idx;
    node_handle dst_node;
    size_t dst_idx;
    // data for old connection
    uid old_id;
    node_handle old_src_node;
    size_t old_src_idx;
    // data for new connection
    uid id;
    connection_handle connection;

    dcmd_connect(
      const node_handle& src_node,
      size_t src_idx,
      const node_handle& dst_node,
      size_t dst_idx);

    void exec(data_context& ctx) override;
    void undo(data_context& ctx) override;
    auto type() const -> data_command_type override;
  };

  // disconnect()
  struct dcmd_disconnect : data_command
  {
    // param
    uid id;
    connection_handle connection;
    // data
    node_handle src_node;
    size_t src_idx;
    node_handle dst_node;
    size_t dst_idx;

    dcmd_disconnect(const connection_handle& connection);

    void exec(data_context& ctx) override;
    void undo(data_context& ctx) override;
    auto type() const -> data_command_type override;
  };

  // set_name()
  struct dcmd_nset_name : data_command
  {
    // param
    node_handle node;
    std::string new_name;
    // old state
    std::string old_name;

    dcmd_nset_name(node_handle node, std::string new_name);

    void exec(data_context& ctx) override;
    void undo(data_context& ctx) override;
    auto type() const -> data_command_type override;
  };

  // set_name()
  struct dcmd_sset_name : data_command
  {
    // param
    socket_handle socket;
    std::string new_name;
    // old state
    std::string old_name;

    dcmd_sset_name(socket_handle socket, std::string new_name);

    void exec(data_context& ctx) override;
    void undo(data_context& ctx) override;
    auto type() const -> data_command_type override;
  };

  // set_pos()
  struct dcmd_nset_pos : data_command
  {
    // param
    node_handle node;
    glm::vec2 new_pos;
    // old state
    glm::vec2 old_pos;

    dcmd_nset_pos(node_handle node, glm::vec2 new_name);

    void exec(data_context& ctx) override;
    void undo(data_context& ctx) override;
    auto type() const -> data_command_type override;
  };

  // bring_front
  struct dcmd_nbring_front : data_command
  {
    // param
    node_handle node;

    dcmd_nbring_front(node_handle node);

    void exec(data_context& ctx) override;
    void undo(data_context& ctx) override;
    auto type() const -> data_command_type override;
  };

  // remove()
  struct dcmd_sremove : data_command
  {
    socket_handle socket;

    dcmd_sremove(socket_handle s);

    void exec(data_context& ctx) override;
    void undo(data_context& ctx) override;
    auto type() const -> data_command_type override;
  };

  // add_*_socket()
  struct dcmd_sadd : data_command
  {
    node_handle node;
    socket_type stype;
    size_t index;

    dcmd_sadd(node_handle n, socket_type stype, size_t index);

    void exec(data_context& ctx) override;
    void undo(data_context& ctx) override;
    auto type() const -> data_command_type override;
  };

  // save graph
  struct dcmd_save : data_command
  {
    void exec(data_context& ctx) override;
    void undo(data_context& ctx) override;
    auto type() const -> data_command_type override;
  };

  // load graph
  struct dcmd_load : data_command
  {
    void exec(data_context& ctx) override;
    void undo(data_context& ctx) override;
    auto type() const -> data_command_type override;
  };
}