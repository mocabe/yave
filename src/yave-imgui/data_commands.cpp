//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave-imgui/data_commands.hpp>
#include <yave/editor/data_context.hpp>
#include <yave/editor/serialize.hpp>
#include <yave/node/core/properties.hpp>

namespace yave::editor::imgui {

  // ------------------------------------------
  // dcmd_push_update

  void dcmd_push_update::exec(data_context& ctx)
  {
    auto lck  = ctx.get_data<editor_data>();
    auto& upd = lck.ref().update_channel();

    for (auto&& diff : m_diffs)
      upd.push_update({.arg = diff.node, .data = diff.value});
  }

  void dcmd_push_update::undo(data_context& /*ctx*/)
  {
    assert(false);
  }

  auto dcmd_push_update::type() const -> data_command_type
  {
    return data_command_type::single_time;
  }

  // ------------------------------------------
  // dcmd_notify_execute

  dcmd_notify_execute::dcmd_notify_execute(std::optional<media::time> time)
    : m_time {time}
  {
  }

  void dcmd_notify_execute::exec(data_context& ctx)
  {
    if (m_time) {
      auto lck = ctx.get_data<editor_data>();
      lck.ref().executor_data().set_arg_time(*m_time);
    }

    {
      auto lck = ctx.get_data<execute_thread>();
      lck.ref().notify_execute();
    }
  }

  void dcmd_notify_execute::undo(data_context& /*ctx*/)
  {
    assert(false);
  }

  auto dcmd_notify_execute::type() const -> data_command_type
  {
    return data_command_type::single_time;
  }

  // ------------------------------------------
  // dcmd_notify_compile

  void dcmd_notify_compile::exec(data_context& ctx)
  {
    auto lck = ctx.get_data<compile_thread>();
    lck.ref().notify_compile();
  }

  void dcmd_notify_compile::undo(data_context& ctx)
  {
    assert(false);
  }

  auto dcmd_notify_compile::type() const -> data_command_type
  {
    return data_command_type::single_time;
  }

  // ------------------------------------------
  // dcmd_ncreate

  dcmd_ncreate::dcmd_ncreate(
    const ImVec2& pos,
    const node_handle& group,
    const node_handle& source)
    : pos {pos}
    , group {group}
    , source {source}
  {
  }

  void dcmd_ncreate::exec(data_context& ctx)
  {
    auto lck = ctx.get_data<editor_data>();
    auto& ng = lck.ref().node_graph();

    if (!ng.exists(source) || !ng.exists(group))
      return;

    if (id == uid())
      id = uid::random_generate();

    node = ng.create_copy(group, source, id);
    set_pos({pos.x, pos.y}, node, ng);
  }

  void dcmd_ncreate::undo(data_context& ctx)
  {
    auto lck = ctx.get_data<editor_data>();
    lck.ref().node_graph().destroy(node);
    node = {};
  }

  auto dcmd_ncreate::type() const -> data_command_type
  {
    // redo/undo ready.
    // return data_command_type::undo_redo;
    return data_command_type::single_time;
  }

  // ------------------------------------------
  // dcmd_gcreate

  dcmd_gcreate::dcmd_gcreate(const ImVec2& pos, const node_handle& parent)
    : pos {pos}
    , parent {parent}
  {
  }

  void dcmd_gcreate::exec(data_context& ctx)
  {
    auto lck = ctx.get_data<editor_data>();
    auto& ng = lck.ref().node_graph();

    if (!ng.exists(parent))
      return;

    if (id == uid())
      id = uid::random_generate();

    group = ng.create_group(parent, {}, id);
    set_pos({pos.x, pos.y}, group, ng);
  }

  void dcmd_gcreate::undo(data_context& ctx)
  {
    auto lck = ctx.get_data<editor_data>();
    lck.ref().node_graph().destroy(group);
    group = {};
  }

  auto dcmd_gcreate::type() const -> data_command_type
  {
    // redo/undo ready.
    // return data_command_type::undo_redo;
    return data_command_type::single_time;
  }

  // ------------------------------------------
  // dcmd_ndestroy

  dcmd_ndestroy::dcmd_ndestroy(std::vector<node_handle> ns)
    : nodes {std::move(ns)}
  {
  }

  void dcmd_ndestroy::exec(data_context& ctx)
  {
    auto lck   = ctx.get_data<editor_data>();
    auto& data = lck.ref();
    auto& ng   = data.node_graph();

    for (auto&& n : nodes)
      ng.destroy(n);

    ctx.cmd(std::make_unique<dcmd_notify_compile>());
  }

  void dcmd_ndestroy::undo(data_context& /*ctx*/)
  {
    // TODO
    assert(false);
  }

  auto dcmd_ndestroy::type() const -> data_command_type
  {
    return data_command_type::single_time;
  }

  // ------------------------------------------
  // dcmd_ngroup

  dcmd_ngroup::dcmd_ngroup(node_handle p, std::vector<node_handle> ns)
    : parent {p}
    , nodes {std::move(ns)}
  {
  }

  void dcmd_ngroup::exec(data_context& ctx)
  {
    auto lck   = ctx.get_data<editor_data>();
    auto& data = lck.ref();
    auto& ng   = data.node_graph();

    auto newg = ng.create_group(parent, {nodes});

    auto avg_pos = glm::vec2();

    for (auto&& n : nodes)
      avg_pos += get_pos(n, ng);

    avg_pos /= nodes.size();
    set_pos(avg_pos, newg, ng);

    ctx.cmd(std::make_unique<dcmd_notify_compile>());
  }

  void dcmd_ngroup::undo(data_context& /*ctx*/)
  {
    // TODO
    assert(false);
  }

  auto dcmd_ngroup::type() const -> data_command_type
  {
    return data_command_type::single_time;
  }

  // ------------------------------------------
  // dcmd_connect

  dcmd_connect::dcmd_connect(
    const node_handle& src_node,
    size_t src_idx,
    const node_handle& dst_node,
    size_t dst_idx)
    : src_node {src_node}
    , src_idx {src_idx}
    , dst_node {dst_node}
    , dst_idx {dst_idx}
  {
  }

  void dcmd_connect::exec(data_context& ctx)
  {
    auto lck   = ctx.get_data<editor_data>();
    auto& data = lck.ref();
    auto& ng   = data.node_graph();

    if (!ng.exists(src_node) || !ng.exists(dst_node))
      return;

    if (ng.output_sockets(src_node).size() <= src_idx)
      return;

    if (ng.input_sockets(dst_node).size() <= dst_idx)
      return;

    if (id == uid())
      id = uid::random_generate();

    auto src = ng.output_sockets(src_node)[src_idx];
    auto dst = ng.input_sockets(dst_node)[dst_idx];

    if (auto cs = ng.connections(dst); !cs.empty()) {
      assert(cs.size() == 1);
      auto info    = ng.get_info(cs[0]);
      old_id       = cs[0].id();
      old_src_node = info->src_node();
      old_src_idx  = *ng.get_index(info->src_socket());
      ng.disconnect(cs[0]);
    }

    connection = ng.connect(src, dst, id);

    ctx.cmd(std::make_unique<dcmd_notify_compile>());
  }

  void dcmd_connect::undo(data_context& ctx)
  {
    auto lck   = ctx.get_data<editor_data>();
    auto& data = lck.ref();
    auto& ng   = data.node_graph();

    ng.disconnect(connection);
    connection = {};

    if (old_id != uid()) {
      if (ng.output_sockets(old_src_node).size() <= old_src_idx)
        return;
      if (ng.input_sockets(dst_node).size() <= dst_idx)
        return;

      auto src = ng.output_sockets(old_src_node)[old_src_idx];
      auto dst = ng.input_sockets(dst_node)[dst_idx];
      (void)ng.connect(src, dst, old_id);
    }

    ctx.cmd(std::make_unique<dcmd_notify_compile>());
  }

  auto dcmd_connect::type() const -> data_command_type
  {
    // redo/undo ready.
    // return data_command_type::undo_redo;
    return data_command_type::single_time;
  }

  // ------------------------------------------
  // dcmd_disconnect

  dcmd_disconnect::dcmd_disconnect(const connection_handle& connection)
    : id {connection.id()}
    , connection {connection}
  {
  }

  void dcmd_disconnect::exec(data_context& ctx)
  {
    auto lck   = ctx.get_data<editor_data>();
    auto& data = lck.ref();
    auto& ng   = data.node_graph();

    if (!ng.exists(connection))
      return;

    auto info = ng.get_info(connection);

    src_node = info->src_node();
    dst_node = info->dst_node();
    src_idx  = *ng.get_index(info->src_socket());
    dst_idx  = *ng.get_index(info->dst_socket());

    ng.disconnect(connection);
    connection = {};

    ctx.cmd(std::make_unique<dcmd_notify_compile>());
  }

  void dcmd_disconnect::undo(data_context& ctx)
  {
    auto lck   = ctx.get_data<editor_data>();
    auto& data = lck.ref();
    auto& ng   = data.node_graph();

    if (!ng.exists(src_node) || !ng.exists(dst_node))
      return;

    if (ng.output_sockets(src_node).size() <= src_idx)
      return;

    if (ng.input_sockets(dst_node).size() <= dst_idx)
      return;

    auto src   = ng.output_sockets(src_node)[src_idx];
    auto dst   = ng.input_sockets(dst_node)[dst_idx];
    connection = ng.connect(src, dst, id);

    ctx.cmd(std::make_unique<dcmd_notify_compile>());
  }

  auto dcmd_disconnect::type() const -> data_command_type
  {
    // redo/undo ready.
    // return data_command_type::undo_redo;
    return data_command_type::single_time;
  }

  // ------------------------------------------
  // dcmd_nset_name

  dcmd_nset_name::dcmd_nset_name(node_handle node, std::string new_name)
    : node {node}
    , new_name {new_name}
  {
  }

  void dcmd_nset_name::exec(data_context& ctx)
  {
    auto lck = ctx.get_data<editor_data>();
    auto& ng = lck.ref().node_graph();

    if (!ng.exists(node))
      return;

    old_name = *ng.get_name(node);
    ng.set_name(node, new_name);
  }

  void dcmd_nset_name::undo(data_context& ctx)
  {
    auto lck = ctx.get_data<editor_data>();
    auto& ng = lck.ref().node_graph();

    if (ng.exists(node))
      ng.set_name(node, old_name);
  }

  auto dcmd_nset_name::type() const -> data_command_type
  {
    // redo/undo ready.
    // return data_command_type::undo_redo;
    return data_command_type::single_time;
  }

  // ------------------------------------------
  // dcmd_sset_name

  dcmd_sset_name::dcmd_sset_name(socket_handle socket, std::string new_name)
    : socket {socket}
    , new_name {new_name}
  {
  }

  void dcmd_sset_name::exec(data_context& ctx)
  {
    auto lck = ctx.get_data<editor_data>();
    auto& ng = lck.ref().node_graph();

    if (!ng.exists(socket))
      return;

    old_name = *ng.get_name(socket);
    ng.set_name(socket, new_name);
  }

  void dcmd_sset_name::undo(data_context& ctx)
  {
    auto lck = ctx.get_data<editor_data>();
    auto& ng = lck.ref().node_graph();

    if (ng.exists(socket))
      ng.set_name(socket, old_name);
  }

  auto dcmd_sset_name::type() const -> data_command_type
  {
    // redo/undo ready.
    // return data_command_type::undo_redo;
    return data_command_type::single_time;
  }

  // ------------------------------------------
  // dcmd_nset_pos

  dcmd_nset_pos::dcmd_nset_pos(node_handle node, glm::vec2 new_pos)
    : node {node}
    , new_pos {new_pos}
  {
  }

  void dcmd_nset_pos::exec(data_context& ctx)
  {
    auto lck = ctx.get_data<editor_data>();
    auto& ng = lck.ref().node_graph();

    if (!ng.exists(node))
      return;

    old_pos = get_pos(node, ng);
    set_pos(new_pos, node, ng);
  }

  void dcmd_nset_pos::undo(data_context& ctx)
  {
    auto lck = ctx.get_data<editor_data>();
    auto& ng = lck.ref().node_graph();

    if (ng.exists(node))
      set_pos(old_pos, node, ng);
  }

  auto dcmd_nset_pos::type() const -> data_command_type
  {
    // redo/undo ready.
    // return data_command_type::undo_redo;
    return data_command_type::single_time;
  }

  // ------------------------------------------
  // dcmd_nbring_front

  dcmd_nbring_front::dcmd_nbring_front(node_handle node)
    : node {node}
  {
  }

  void dcmd_nbring_front::exec(data_context& ctx)
  {
    auto lck = ctx.get_data<editor_data>();
    auto& ng = lck.ref().node_graph();

    if (!ng.exists(node))
      return;

    ng.bring_front(node);
  }

  void dcmd_nbring_front::undo(data_context& /*ctx*/)
  {
    assert(false);
  }

  auto dcmd_nbring_front::type() const -> data_command_type
  {
    // TODO
    return data_command_type::single_time;
  }

  // ------------------------------------------
  // dcmd_sremove

  dcmd_sremove::dcmd_sremove(socket_handle socket)
    : socket {socket}
  {
  }

  void dcmd_sremove::exec(data_context& ctx)
  {
    auto lck   = ctx.get_data<editor_data>();
    auto& data = lck.ref();
    data.node_graph().remove_socket(socket);
    ctx.cmd(std::make_unique<dcmd_notify_compile>());
  }

  void dcmd_sremove::undo(data_context& /*ctx*/)
  {
    assert(false);
  }

  auto dcmd_sremove::type() const -> data_command_type
  {
    // TODO
    return data_command_type::single_time;
  }

  // ------------------------------------------
  // dcmd_sadd

  dcmd_sadd::dcmd_sadd(node_handle n, socket_type stype, size_t index)
    : node {n}
    , stype {stype}
    , index {index}
  {
  }

  void dcmd_sadd::exec(data_context& ctx)
  {
    auto lck   = ctx.get_data<editor_data>();
    auto& data = lck.ref();
    auto& ng   = data.node_graph();

    auto new_s = [&] {
      if (stype == socket_type::input)
        return ng.add_input_socket(node, std::to_string(index));
      if (stype == socket_type::output)
        return ng.add_output_socket(node, std::to_string(index));

      return socket_handle();
    }();

    if (new_s)
      ctx.cmd(std::make_unique<dcmd_notify_compile>());
  }

  void dcmd_sadd::undo(data_context& /*ctx*/)
  {
    assert(false);
  }

  auto dcmd_sadd::type() const -> data_command_type
  {
    // TODO: undo/redo
    return data_command_type::single_time;
  }

  // ------------------------------------------
  // dcmd_save

  dcmd_save::dcmd_save(std::string path)
    : m_path {path}
  {
  }

  void dcmd_save::exec(data_context& ctx)
  {
    auto lck = ctx.get_data<editor_data>();

    (void)save(lck.ref(), m_path);
  }

  void dcmd_save::undo(data_context& ctx)
  {
    assert(false);
  }

  auto dcmd_save::type() const -> data_command_type
  {
    return data_command_type::single_time;
  }

  // ------------------------------------------
  // dcmd_load

  dcmd_load::dcmd_load(std::string path)
    : m_path {path}
  {
  }

  void dcmd_load::exec(data_context& ctx)
  {
    auto lck = ctx.get_data<editor_data>();

    if (load(lck.ref(), m_path)) {
      ctx.cmd(std::make_unique<dcmd_notify_compile>());
    }
  }

  void dcmd_load::undo(data_context& ctx)
  {
    assert(false);
  }

  auto dcmd_load::type() const -> data_command_type
  {
    return data_command_type::single_time;
  }
} // namespace yave::editor::imgui