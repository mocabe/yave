//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave-imgui/basic_socket_drawer.hpp>
#include <yave-imgui/node_window.hpp>
#include <yave-imgui/data_commands.hpp>

#include <yave/editor/editor_data.hpp>
#include <yave/editor/data_command.hpp>
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

  auto basic_socket_drawer::_choose_slot_color(
    const node_window& /*nw*/,
    const data_context& dctx,
    const view_context& /*vctx*/) const -> ImColor
  {
    auto s = handle;

    // defualt color
    auto col = get_socket_slot_color_empty();

    // choose color
    {
      auto lck    = dctx.lock();
      auto& data  = lck.get_data<editor_data>();
      auto& ng    = data.node_graph;
      auto& parse = data.compiler.parse_result();

      auto res = parse.get_results(ng, s);

      for (auto&& r : res) {

        if (yave::type(r) == parse_result_type::info) {
          // has input
          if (
            std::get_if<parse_results::has_default_argument>(&r) || //
            std::get_if<parse_results::has_input_connection>(&r) || //
            std::get_if<parse_results::has_output_connection>(&r)) {
            col = get_socket_slot_color_connected();
          }
        }

        if (yave::type(r) == parse_result_type::warning) {
          // TODO
        }

        if (yave::type(r) == parse_result_type::error) {
          col = get_socket_slot_color_missing();
          break;
        }
      }
    }
    return col;
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
    auto col            = _choose_slot_color(nw, dctx, vctx);
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
              auto c = info.connections()[0];
              dctx.cmd(std::make_unique<dcmd_disconnect>(c));
              socket_to_select = draw_info.find_drawable(c)->info.src_socket();
            }
          }

          // start dragging
          vctx.cmd(make_window_view_command(
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

          if (nw.state() == node_window::state::socket) {
            // dragged connect
            auto s1    = nw.get_selected_socket().value();
            auto& s1d  = draw_info.find_drawable(s1);
            auto s1n   = s1d->info.node();
            auto s1idx = s1d->info.index();

            // dropped socket
            auto s2n   = info.node();
            auto s2idx = info.index();

            // src -> dst
            if (info.is_output()) {
              std::swap(s1n, s2n);
              std::swap(s1idx, s2idx);
            }
            // connect sockets
            dctx.cmd(std::make_unique<dcmd_connect>(s1n, s1idx, s2n, s2idx));
          }
        }
        ImGui::EndDragDropTarget();
      }

      if (nw.state() == node_window::state::socket) {
        if (ImGui::IsMouseReleased(0)) {
          // only once
          if (draw_info.sockets.front().get() == this)
            vctx.cmd(make_window_view_command(
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