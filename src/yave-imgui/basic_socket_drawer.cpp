//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave-imgui/basic_socket_drawer.hpp>
#include <yave-imgui/node_window.hpp>

#include <yave/editor/editor_data.hpp>
#include <yave/module/std/primitive/primitive.hpp>
#include <imgui_stdlib.h>
#include <iostream>

namespace yave::editor::imgui {

  using namespace yave::imgui;

  basic_socket_drawer::basic_socket_drawer(
    const socket_handle& s,
    const structured_node_graph& g,
    const node_window& nw)
    : socket_drawable {s, *g.get_info(s)}
  {
    (void)nw;
    m_data = g.get_data(s);
  }

  auto basic_socket_drawer::min_size(node_window_draw_info&) const -> ImVec2
  {
    auto name      = info.name();
    auto text_size = calc_text_size(name, font_size_level::e15);
    auto padding   = get_socket_padding();
    auto height    = 16.f; // fixed
    return {text_size.x + 2 * padding.x, height + 2 * padding.y};
  }

  auto basic_socket_drawer::slot_pos(
    node_window_draw_info& draw_info,
    ImVec2 size) const -> ImVec2
  {
    (void)draw_info;

    auto type = info.type();

    switch (type) {
      case socket_type::input:
        return {size.x, size.y / 2};
      case socket_type::output:
        return {0, size.y / 2};
    }
    unreachable();
  }

  void basic_socket_drawer::_draw_slot(
    const node_window& nw,
    const data_context& dctx,
    const view_context& vctx,
    node_window_draw_info& draw_info,
    const ImVec2& slot_pos) const
  {
    auto s              = handle;
    auto dl             = ImGui::GetWindowDrawList();
    auto type           = info.type();
    auto col            = get_socket_slot_color(type);
    auto slot_radius    = get_socket_slot_radius();
    auto slot_rect_size = slot_radius * 2;

    ImGui::PushID(s.id().data);
    {
      ImGui::SetCursorScreenPos(
        slot_pos - ImVec2 {slot_rect_size, slot_rect_size});

      // draw
      dl->AddCircleFilled(slot_pos, slot_radius, col);

      // handle socket input
      bool hov = false;
      InvisibleButtonEx(
        "socket slot", {2 * slot_rect_size, 2 * slot_rect_size}, &hov);

      if (ImGui::BeginDragDropSource()) {

        if (nw.state() == node_window::state::neutral) {

          auto socket_to_select = s;

          // when input socket already has connections, disconnect.
          if (info.type() == socket_type::input) {

            if (!info.connections().empty()) {

              auto c    = info.connections()[0];
              auto& cd  = draw_info.find_drawable(c);
              auto dsts = cd->info.dst_socket();
              auto srcs = cd->info.src_socket();

              dctx.exec(make_data_command(
                [c](auto& ctx) {
                  auto& g = ctx.data().node_graph;
                  g.disconnect(c);
                  ctx.data().compiler.notify_recompile();
                },
                [srcs, dsts](auto& ctx) {
                  auto& g = ctx.data().node_graph;
                  (void)g.connect(srcs, dsts);
                  ctx.data().compiler.notify_recompile();
                }));

              socket_to_select = srcs;
            }
          }

          // start dragging
          vctx.push(make_window_view_command(
            nw, [s = socket_to_select, mpos = ImGui::GetMousePos()](auto& w) {
              w.clear_selected();
              w.clear_hovered();
              w.begin_socket_drag(to_tvec2(mpos));
              w.set_selected(s);
            }));
        }
        ImGui::SetDragDropPayload("socket_dd", nullptr, 0, ImGuiCond_Once);
        ImGui::EndDragDropSource();
      }

      if (ImGui::BeginDragDropTarget()) {
        if (ImGui::AcceptDragDropPayload("socket_dd")) {

          struct dcmd_connect : data_command
          {
            socket_handle m_s1;
            socket_handle m_s2;
            connection_handle m_c;

            dcmd_connect(socket_handle s1, socket_handle s2)
              : m_s1 {s1}
              , m_s2 {s2}
            {
            }
            void exec(data_context& dctx) override
            {
              auto& g = dctx.data().node_graph;
              m_c     = g.connect(m_s1, m_s2);
              dctx.data().compiler.notify_recompile();
            }
            void undo(data_context& dctx) override
            {
              auto& g = dctx.data().node_graph;
              g.disconnect(m_c);
            }
          };

          if (nw.state() == node_window::state::socket) {
            // sockets to connect
            auto s1 = nw.get_selected_socket().value();
            auto s2 = s;
            if (info.is_output())
              std::swap(s1, s2);

            // disconnect existing connection to target input
            auto& sd2 = draw_info.find_drawable(s2);
            if (!sd2->info.connections().empty()) {

              assert(sd2->info.connections().size() == 1);
              auto c   = sd2->info.connections()[0];
              auto& cd = draw_info.find_drawable(c);

              auto srcs = cd->info.src_socket();
              auto dsts = cd->info.dst_socket();

              // ignore when connection already exist
              if (srcs != s1 || dsts != s2) {
                // disconnect existing connection
                dctx.exec(make_data_command(
                  [c](auto& ctx) {
                    auto& g = ctx.data().node_graph;
                    g.disconnect(c);
                  },
                  [srcs, dsts](auto& ctx) {
                    auto& g = ctx.data().node_graph;
                    (void)g.connect(srcs, dsts);
                  }));
                // connect sockets
                dctx.exec(std::make_unique<dcmd_connect>(s1, s2));
              }
            } else
              // connect sockets
              dctx.exec(std::make_unique<dcmd_connect>(s1, s2));
          }
        }
        ImGui::EndDragDropTarget();
      }

      if (nw.state() == node_window::state::socket) {
        if (ImGui::IsMouseReleased(0)) {
          // only once
          if (draw_info.sockets.front().get() == this)
            vctx.push(make_window_view_command(
              nw, [](auto& w) { w.end_socket_drag(); }));
        }
      }
    }
    ImGui::PopID();
  }

  void basic_socket_drawer::_draw_content(
    const node_window& nw,
    const data_context& dctx,
    const view_context& vctx,
    node_window_draw_info& draw_info,
    ImVec2 pos,
    ImVec2 size) const
  {
    (void)nw, (void)dctx, (void)vctx, (void)draw_info;

    auto type = info.type();
    auto name = info.name();

    text_alignment align;

    switch (type) {
      case socket_type::input:
        align = text_alignment::right;
        break;
      case socket_type::output:
        align = text_alignment::left;
        break;
      default:
        unreachable();
    }

    auto text_pos = calc_text_pos(name, font_size_level::e15, size, align);
    auto col      = get_socket_text_color();

    ImGui::SetCursorScreenPos(pos + text_pos);
    ImGui::TextColored(col, "%s", name.c_str());
  }

  void basic_socket_drawer::draw(
    const node_window& nw,
    const data_context& dctx,
    const view_context& vctx,
    node_window_draw_info& draw_info,
    ImDrawListSplitter& splitter,
    size_t channel,
    ImVec2 pos,
    ImVec2 size) const
  {
    splitter.SetCurrentChannel(ImGui::GetWindowDrawList(), channel);

    // draw slots
    auto slotpos = pos + slot_pos(draw_info, size);
    _draw_slot(nw, dctx, vctx, draw_info, slotpos);
    // draw content
    auto padding = ImVec2 {8, 8};
    _draw_content(nw, dctx, vctx, draw_info, pos + padding, size - padding * 2);
  }
} // namespace yave::editor::imgui