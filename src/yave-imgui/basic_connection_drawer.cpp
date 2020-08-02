//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave-imgui/basic_connection_drawer.hpp>
#include <yave-imgui/node_window.hpp>

#include <yave/editor/editor_data.hpp>
#include <yave/module/std/primitive/primitive.hpp>
#include <imgui_stdlib.h>

namespace yave::editor::imgui {

  using namespace yave::imgui;

  basic_connection_drawer::basic_connection_drawer(
    const connection_handle& c,
    const structured_node_graph& g,
    const node_window& nw)
    : connection_drawable {c, *g.get_info(c)}
  {
    (void)nw;
  }

  void basic_connection_drawer::draw(
    const node_window& nw,
    const data_context& dctx,
    const view_context& vctx,
    node_window_draw_info& draw_info,
    ImDrawListSplitter& splitter,
    int channel)
  {
    (void)dctx;
    (void)vctx;

    splitter.SetCurrentChannel(ImGui::GetWindowDrawList(), channel);

    auto s1 = info.src_socket();
    auto s2 = info.dst_socket();

    auto& sd1 = draw_info.find_drawable(s1);
    auto& sd2 = draw_info.find_drawable(s2);

    auto& nd1 = draw_info.find_drawable(sd1->info.node());
    auto& nd2 = draw_info.find_drawable(sd2->info.node());

    auto srcpos =
      nd1->screen_pos(nw, draw_info) + nd1->socket_area_pos(draw_info, s1)
      + sd1->slot_pos(draw_info, nd1->socket_area_size(draw_info, s1));

    auto dstpos =
      nd2->screen_pos(nw, draw_info) + nd2->socket_area_pos(draw_info, s2)
      + sd2->slot_pos(draw_info, nd2->socket_area_size(draw_info, s2));

    draw_connection_line(srcpos, dstpos, false, false);
  }

} // namespace yave::editor::imgui
