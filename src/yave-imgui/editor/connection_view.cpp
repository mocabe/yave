//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave-imgui/editor/connection_view.hpp>
#include <yave-imgui/editor/editor_context.hpp>
#include <yave-imgui/editor/style.hpp>
#include <yave/support/log.hpp>

YAVE_DECL_G_LOGGER(connection_view)

namespace yave::editor::imgui {

  namespace {

    void draw_connection_edge(
      const ImVec2& src,
      const ImVec2& dst,
      bool is_hovered,
      bool is_selected)
    {
      auto draw_list = ImGui::GetWindowDrawList();

      auto col = is_hovered ? get_node_edge_color_hovered()
                            : is_selected ? get_node_edge_color_selected()
                                          : get_node_edge_color();

      // line
      draw_list->AddLine(src, src, col);
    }

  } // namespace

  connection_view::connection_view(
    const connection_handle& h,
    const editor_context& ctx)
    : handle {h}
    , info {*ctx.node_graph().get_info(h)}
  {
    init_logger();
  }

  basic_connection_view::basic_connection_view(
    const connection_handle& h,
    const editor_context& ctx,
    const std::shared_ptr<node_view>& srcn,
    const std::shared_ptr<node_view>& dstn,
    const std::shared_ptr<socket_view>& srcs,
    const std::shared_ptr<socket_view>& dsts)
    : connection_view(h, ctx)
    , src_node {srcn}
    , dst_node {dstn}
    , src_socket {srcs}
    , dst_socket {dsts}
  {
    auto editor_info = ctx.get_editor_info(h);

    is_hovered  = editor_info->is_hovered;
    is_selected = editor_info->is_selected;
  }

  void basic_connection_view::draw(const ImVec2& src_pos, const ImVec2& dst_pos)
    const
  {
    auto channel = get_connection_channel_index();
    ImGui::GetWindowDrawList()->ChannelsSetCurrent(channel);

    draw_connection_edge(src_pos, dst_pos, is_hovered, is_selected);
  }

  void basic_connection_view::handle_input(const ImVec2&, const ImVec2&) const
  {
    ImGui::PushID(handle.id().data);

    // TODO

    ImGui::PopID();
  }
}