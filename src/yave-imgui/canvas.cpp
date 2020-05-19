//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave-imgui/canvas.hpp>
#include <yave-imgui/style.hpp>
#include <yave-imgui/layout.hpp>

#include <yave/lib/imgui/extension.hpp>
#include <yave/module/std/primitive/primitive.hpp>

#include <boost/geometry/algorithms/intersects.hpp>
#include <boost/geometry/geometries/register/point.hpp>
#include <utility>

#include <imgui_stdlib.h>

BOOST_GEOMETRY_REGISTER_POINT_2D(ImVec2, float, boost::geometry::cs::cartesian, x, y)

namespace yave::editor::imgui {

  namespace {

    using namespace yave::imgui;
    using namespace yave::app;

    // ------------------------------------------
    // Util

    bool rectHit(ImVec2 p1, ImVec2 p2, const ImVec2& p)
    {
      boost::geometry::model::box<ImVec2> b(p1, p2);
      return boost::geometry::intersects(b, p);
    }

    bool rectIntersec(ImVec2 p1, ImVec2 p2, ImVec2 q1, ImVec2 q2)
    {
      boost::geometry::model::box<ImVec2> b1(p1, p2);
      boost::geometry::model::box<ImVec2> b2(q1, q2);
      return boost::geometry::intersects(b1, b2);
    }

    // ------------------------------------------
    // Canvas

    /// draw background color, grid, etc.
    void draw_background_channel(
      const imgui_context& imgui_ctx,
      const editor_context& editor_ctx,
      const canvas_layout& layout)
    {
      (void)imgui_ctx;
      (void)layout;

      auto wpos  = ImGui::GetWindowPos();
      auto wsize = ImGui::GetWindowSize();
      auto dl    = ImGui::GetWindowDrawList();

      // setup
      ImGui::SetCursorScreenPos(wpos);

      // draw background
      dl->AddRectFilled(wpos, wpos + wsize, get_background_color());

      // draw grid
      auto scroll = to_ImVec2(editor_ctx.get_scroll());
      auto grid   = get_background_grid_size();

      for (auto x = std::fmod(scroll.x, grid); x < wsize.x; x += grid)
        dl->AddLine(
          {wpos.x + x, wpos.y},
          {wpos.x + x, wpos.y + wsize.y},
          get_background_grid_color());

      for (auto y = std::fmod(scroll.y, grid); y < wsize.y; y += grid)
        dl->AddLine(
          {wpos.x, wpos.y + y},
          {wpos.x + wsize.x, wpos.y + y},
          get_background_grid_color());

      // origin marker
      dl->AddRectFilled(
        wpos + scroll - ImVec2(1, 1),
        wpos + scroll + ImVec2(1, 1),
        ImColor(0, 0, 0, 255));

      // print debug info
      ImGui::Text("scroll=%f, %f", scroll.x, scroll.y);
    }

    void handle_background_channel(
      imgui_context& imgui_ctx,
      editor_context& editor_ctx,
      const canvas_layout& layout)
    {
      (void)imgui_ctx;
      (void)layout;

      auto wpos   = ImGui::GetWindowPos();
      auto wsize  = ImGui::GetWindowSize();
      auto scroll = to_ImVec2(editor_ctx.get_scroll());

      ImGui::SetCursorScreenPos(wpos);

      // handle scrolling
      if (
        ImGui::IsWindowHovered() && !ImGui::IsAnyItemActive()
        && ImGui::IsMouseDragging(2)) {
        auto delta = ImGui::GetIO().MouseDelta;
        editor_ctx.set_scroll(editor_ctx.get_scroll() + to_tvec2(delta));
      }

      // popup menu
      auto popup_name = "node_background_menu";

      bool hovered;
      InvisibleButtonEx("node background input", wsize, &hovered);

      // switch by current state

      if (editor_ctx.get_state() == editor_state::neutral) {

        if (hovered) {
          // left: clear selected items
          if (ImGui::IsMouseClicked(0)) {
            editor_ctx.clear_selected();
            editor_ctx.begin_background_drag(to_tvec2(ImGui::GetMousePos()));
          }
          // right: open popup menu
          if (ImGui::IsMouseClicked(1)) {
            ImGui::OpenPopup(popup_name);
          }
        }

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {8, 8});
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, {3, 3});
        {
          // popup menu
          if (ImGui::BeginPopup(popup_name)) {

            // pos to create node
            auto npos =
              ImGui::GetMousePosOnOpeningCurrentPopup() - wpos - scroll;

            // list nodes avalable
            if (ImGui::BeginMenu("Add Node")) {
              for (auto&& decl : editor_ctx.node_declarations().enumerate()) {
                if (ImGui::Selectable(decl->name().c_str())) {
                  editor_ctx.create(
                    decl->name(), editor_ctx.get_group(), {npos.x, npos.y});
                  break;
                }
              }
              ImGui::EndMenu();
            }
            ImGui::EndPopup();
          }
        }
        ImGui::PopStyleVar(2);
      }

      if (editor_ctx.get_state() == editor_state::background) {
        if (ImGui::IsMouseReleased(0)) {
          for (auto&& [n, nlayout] : layout.map) {
            if (rectIntersec(
                  nlayout.pos,
                  nlayout.pos + nlayout.size,
                  to_ImVec2(editor_ctx.get_drag_source_pos()),
                  ImGui::GetMousePos()))
              editor_ctx.add_selected(n);
          }
          editor_ctx.end_background_drag();
        }
      }
    }

    void process_background_channel(
      imgui_context& imgui_ctx,
      editor_context& editor_ctx,
      const canvas_layout& layout)
    {
      ImGui::GetWindowDrawList()->ChannelsSetCurrent(
        layout.background_channel_index);

      handle_background_channel(imgui_ctx, editor_ctx, layout);
      draw_background_channel(imgui_ctx, editor_ctx, layout);
    }

    // ------------------------------------------
    // Sockets

    void draw_socket_content(
      const socket_handle& s,
      const socket_layout& slayout,
      const editor_context& editor_ctx)
    {
      auto& g = editor_ctx.node_graph();

      auto sinfo = g.get_info(s);
      auto type  = sinfo->type();
      auto name  = sinfo->name();

      if (auto data = g.get_data(s);
          data && value_cast_if<DataTypeHolder>(data)) {

        // see handle_socket_content()

      } else {

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

        auto text_pos =
          calc_text_pos(name, font_size_level::e15, slayout.area_size, align);

        auto col = get_socket_text_color();

        // socket name
        ImGui::SetCursorScreenPos(slayout.area_pos + text_pos);
        ImGui::TextColored(col, "%s", name.c_str());
      }
    }

    void draw_socket_slot(
      const socket_handle& s,
      const socket_layout& slayout,
      const editor_context& editor_ctx)
    {
      auto& g = editor_ctx.node_graph();

      auto type        = g.get_info(s)->type();
      auto is_hovered  = editor_ctx.is_hovered(s);
      auto is_selected = editor_ctx.is_selected(s);

      auto slot_pos = calc_socket_slot_pos(type, slayout.pos, slayout.size);

      auto draw_list = ImGui::GetWindowDrawList();

      auto col = is_hovered ? get_socket_slot_color_hovered(type)
                            : is_selected ? get_socket_slot_color_selected(type)
                                          : get_socket_slot_color(type);

      // slot
      draw_list->AddCircleFilled(slot_pos, get_socket_slot_radius(), col);
    }

    // ------------------------------------------
    // Node

    void draw_node_background(
      const ImVec2& pos,
      const ImVec2& size,
      bool is_hovered,
      bool is_selected)
    {
      auto fill = is_hovered
                    ? get_node_background_fill_color_hovered()
                    : is_selected ? get_node_background_fill_color_selected()
                                  : get_node_background_fill_color();

      auto rounding  = get_node_rounding();
      auto draw_list = ImGui::GetWindowDrawList();

      // fill
      draw_list->AddRectFilled(pos, pos + size, fill, rounding);
    }

    void draw_node_header(
      const std::string& title,
      const ImVec2& pos,
      const ImVec2& size,
      bool is_hovered,
      bool is_selected)
    {
      auto fill = is_hovered
                    ? get_node_header_fill_color_hovered()
                    : is_selected ? get_node_header_fill_color_selected()
                                  : get_node_header_fill_color();

      auto rounding  = get_node_rounding();
      auto draw_list = ImGui::GetWindowDrawList();

      // fill
      draw_list->AddRectFilled(
        pos, {pos.x + size.x, pos.y + size.y / 2}, fill, rounding);
      draw_list->AddRectFilled(
        {pos.x, pos.y + size.y / 2 - 10}, pos + size, fill, 0);

      // text
      auto text_pos = calc_text_pos(
        title, font_size_level::e15, size, text_alignment::center);
      ImGui::SetCursorScreenPos(pos + text_pos);
      ImGui::TextColored(get_node_header_text_color(), "%s", title.c_str());
    }

    void draw_node_edge(
      const ImVec2& pos,
      const ImVec2& size,
      bool is_hovered,
      bool is_selected)
    {
      auto col = is_hovered ? get_node_edge_color_hovered()
                            : is_selected ? get_node_edge_color_selected()
                                          : get_node_edge_color();

      auto dl = ImGui::GetWindowDrawList();
      dl->AddRect(pos, pos + size, col, get_node_rounding());
    }

    void draw_node(
      const node_handle& n,
      const node_layout& nlayout,
      const editor_context& editor_ctx)
    {
      auto& g = editor_ctx.node_graph();

      ImGui::GetWindowDrawList()->ChannelsSetCurrent(nlayout.channel_index);

      auto npos         = nlayout.pos;
      auto nsize        = nlayout.size;
      auto nheader_size = nlayout.header_size;
      auto nhovered     = editor_ctx.is_hovered(n);
      auto nselected    = editor_ctx.is_selected(n);
      auto nname        = *g.get_name(n);

      // draw background
      draw_node_background(npos, nsize, nhovered, nselected);
      draw_node_header(nname, npos, nheader_size, nhovered, nselected);
      draw_node_edge(npos, nsize, nhovered, nselected);
    }

    void draw_socket(
      const socket_handle& s,
      const socket_layout& slayout,
      const editor_context& editor_ctx)
    {
      draw_socket_content(s, slayout, editor_ctx);
      draw_socket_slot(s, slayout, editor_ctx);
    }

    void handle_socket_slot_input(
      const socket_handle& s,
      const socket_layout& slayout,
      editor_context& editor_ctx)
    {
      auto& g          = editor_ctx.node_graph();
      auto socket_type = g.get_info(s)->type();
      auto slot_pos    = slayout.slot_pos;
      auto slot_size   = get_socket_slot_radius() * 2;

      ImGui::PushID(s.id().data);
      {
        // handle slot
        bool hovered;
        ImGui::SetCursorScreenPos(slot_pos - ImVec2 {slot_size, slot_size});
        InvisibleButtonEx("slot", {2 * slot_size, 2 * slot_size}, &hovered);

        if (hovered) {
          editor_ctx.set_hovered(s);
        }

        // switch by current state

        // In-frame socket hit detection flag
        // FIXME: this should be handled by editor_context
        static bool socket_hit;

        if (editor_ctx.get_state() == editor_state::neutral) {
          // neutral -> socket
          if (hovered && ImGui::IsMouseClicked(0)) {

            auto select_src = s;

            // when input socket already has connections, disconnect.
            if (socket_type == socket_type::input) {
              if (auto cs = g.connections(s); !cs.empty()) {
                auto cinfo = g.get_info(cs[0]);
                assert(cs.size() == 1);
                editor_ctx.disconnect(cs[0]);
                select_src = cinfo->src_socket();
              }
            }

            // start dragging
            editor_ctx.clear_selected();
            editor_ctx.add_selected(select_src);
            editor_ctx.begin_socket_drag(to_tvec2(ImGui::GetMousePos()));
            socket_hit = false;
          }
        }

        if (editor_ctx.get_state() == editor_state::socket) {

          if (ImGui::IsMouseReleased(0)) {

            assert(editor_ctx.get_selected_sockets().size() == 1);
            auto drag_src_socket = editor_ctx.get_selected_sockets()[0];

            // InvisibleButtonEx cannot detect hit when mouse is pressed, so
            // we just manually check hover.
            auto mouse_pos = ImGui::GetMousePos();
            auto p1        = slot_pos - ImVec2 {slot_size, slot_size};
            auto p2        = slot_pos + ImVec2 {slot_size, slot_size};

            if (!socket_hit) {

              if (rectHit(p1, p2, mouse_pos)) {
                auto src = drag_src_socket;
                auto dst = s;

                if (socket_type != socket_type::input)
                  std::swap(src, dst);

                // allow overwriting connection
                for (auto&& c : g.connections(dst))
                  editor_ctx.disconnect(c);

                editor_ctx.connect(src, dst);

                socket_hit = true;
              }
            }

            if (s == drag_src_socket)
              editor_ctx.end_socket_drag();
          }
        }
      }
      ImGui::PopID();
    }

    void handle_socket_content(
      const socket_handle& s,
      const socket_layout& slayout,
      editor_context& editor_ctx)
    {
      auto& g = editor_ctx.node_graph();

      auto n = g.node(s);

      // enable data
      auto slider_bg_col   = get_socket_slider_color();
      auto slider_text_col = get_socket_slider_text_color();

      ImGui::PushID(s.id().data);
      {
        if (auto data = g.get_data(s); data && g.connections(s).empty()) {
          ImGui::SetCursorScreenPos(slayout.area_pos);
          ImGui::PushItemWidth(slayout.area_size.x);
          ImGui::PushStyleColor(ImGuiCol_Text, ImU32(slider_text_col));
          ImGui::PushStyleColor(ImGuiCol_FrameBg, ImU32(slider_bg_col));
          ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImU32(slider_bg_col));
          ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImU32(slider_bg_col));
          ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, get_node_rounding());
          {
            if (auto holder = value_cast_if<DataTypeHolder>(data)) {
              // Float slider
              if (auto f = value_cast_if<Float>(holder->data())) {
                float val = *f;
                ImGui::DragFloat("", &val);
                *f = val;
              }
              if (auto i = value_cast_if<Int>(holder->data())) {
                int val = *i;
                ImGui::DragInt("", &val);
                *i = val;
              }
              if (auto b = value_cast_if<Bool>(holder->data())) {
                ImGui::Checkbox("", &*b);
              }
              if (auto s = value_cast_if<String>(holder->data())) {
                std::string val = *s;
                ImGui::InputText("", &val);
                *s = yave::string(val);
              }
            }
          }
          ImGui::PopStyleColor(4);
          ImGui::PopStyleVar(1);
          ImGui::PopItemWidth();
        }
      }
      ImGui::PopID();
    }

    void handle_socket_input(
      const socket_handle& s,
      const socket_layout& slayout,
      editor_context& editor_ctx)
    {
      handle_socket_slot_input(s, slayout, editor_ctx);
      handle_socket_content(s, slayout, editor_ctx);
    }

    void handle_node_input(
      const node_handle& n,
      const node_layout& nlayout,
      editor_context& editor_ctx)
    {
      auto& g = editor_ctx.node_graph();

      auto npos  = nlayout.pos;
      auto nsize = nlayout.size;

      // handle node input
      ImGui::PushID(n.id().data);
      {
        ImGui::SetCursorScreenPos(npos);

        const char* popup_name = "node_info_popup";

        bool hovered;
        InvisibleButtonEx("node", nsize, &hovered);

        // switch by current state

        if (editor_ctx.get_state() == editor_state::neutral) {

          if (hovered) {
            // set hover
            editor_ctx.set_hovered(n);

            // double click: inspect
            if (ImGui::IsMouseDoubleClicked(0)) {
              if (g.is_group(n)) {
                editor_ctx.set_group(n);
              }
            }

            // right click: select
            if (ImGui::IsMouseClicked(0) && !ImGui::IsMouseDoubleClicked(0)) {
              if (!editor_ctx.is_selected(n))
                editor_ctx.clear_selected();
              editor_ctx.add_selected(n);
              editor_ctx.begin_node_drag(to_tvec2(ImGui::GetMousePos()));
            }

            // left click: open info popup
            if (ImGui::IsMouseClicked(1)) {
              ImGui::OpenPopup("node_info_popup");
            }
          }

          if (ImGui::BeginPopup(popup_name)) {
            ImGui::Text("Node: %s", g.get_name(n)->c_str());
            ImGui::Text("id: %s", to_string(n.id()).c_str());
            ImGui::EndPopup();
          }
        }

        if (editor_ctx.get_state() == editor_state::node) {
          if (ImGui::IsMouseReleased(0)) {
            if (editor_ctx.is_selected(n)) {
              // apply new position to data. notice layout engine provides new
              // position for dragged nodes.
              auto new_pos = to_tvec2(npos) - editor_ctx.get_scroll()
                             - to_tvec2(ImGui::GetWindowPos());
              editor_ctx.set_pos(n, new_pos);
              // back to neutral
              if (n == editor_ctx.get_selected_nodes()[0])
                editor_ctx.end_node_drag();
            }
          }
        }
      }
      ImGui::PopID();
    }

    void process_node_channel(
      imgui_context& imgui_ctx,
      editor_context& editor_ctx,
      const canvas_layout& layout)
    {
      (void)imgui_ctx;

      auto wpos = ImGui::GetWindowPos();
      ImGui::SetCursorScreenPos(wpos);

      auto& g     = editor_ctx.node_graph();
      auto& nodes = layout.nodes;

      if (editor_ctx.get_state() == editor_state::neutral) {
        if (!editor_ctx.get_selected_nodes().empty()) {
          // handle grouping
          if (
            ImGui::IsKeyDown(GLFW_KEY_LEFT_CONTROL)
            && ImGui::IsKeyPressed(GLFW_KEY_G)) {
            editor_ctx.group(editor_ctx.get_selected_nodes());
            return;
          }
          // handle delete
          if (ImGui::IsKeyPressed(GLFW_KEY_DELETE)) {
            for (auto&& n : editor_ctx.get_selected_nodes()) {
              editor_ctx.destroy(n);
            }
            return;
          }
        }
      }

      // traverse in reverse order
      for (auto it = nodes.rbegin(); it != nodes.rend(); ++it) {

        auto& n = *it;

        auto niter = layout.map.find(n);
        assert(niter != layout.map.end());

        auto& nlayout = niter->second;

        ImGui::GetWindowDrawList()->ChannelsSetCurrent(nlayout.channel_index);

        /// we need to handle inputs from top down, but need to render bottom
        /// up.
        draw_node(n, nlayout, editor_ctx);

        for (auto&& s : g.output_sockets(n)) {

          auto siter = nlayout.sockets.find(s);
          assert(siter != nlayout.sockets.end());

          auto& slayout = siter->second;

          draw_socket(s, slayout, editor_ctx);
          handle_socket_input(s, slayout, editor_ctx);
        }

        for (auto&& s : g.input_sockets(n)) {

          auto siter = nlayout.sockets.find(s);
          assert(siter != nlayout.sockets.end());

          auto& slayout = siter->second;

          draw_socket(s, slayout, editor_ctx);
          handle_socket_input(s, slayout, editor_ctx);
        }

        handle_node_input(n, nlayout, editor_ctx);
      }
    }

    // ------------------------------------------
    // Connection

    void draw_connection(
      const ImVec2& p1,
      const ImVec2& p2,
      bool hovered,
      bool selected)
    {
      auto dl = ImGui::GetWindowDrawList();

      auto col = hovered ? get_connection_color_hovered()
                         : selected ? get_connection_color_selected()
                                    : get_connection_color();

      auto thickness = get_connection_thickness();

      ImVec2 disp {gridpx(5), 0};
      dl->AddBezierCurve(p1, p1 - disp, p2 + disp, p2, col, thickness);
      // dl->AddLine(p1, p2, col, thickness);
    }

    void draw_connection_channel(
      const imgui_context& imgui_ctx,
      const editor_context& editor_ctx,
      const canvas_layout& layout)
    {
      (void)imgui_ctx;

      auto& g = editor_ctx.node_graph();

      for (auto&& n : layout.nodes) {

        auto nit = layout.map.find(n);
        assert(nit != layout.map.end());

        for (auto&& s : g.output_sockets(n)) {

          auto sit = nit->second.sockets.find(s);
          assert(sit != nit->second.sockets.end());

          for (auto&& c : g.connections(s)) {

            auto cinfo = g.get_info(c);

            auto ndstit = layout.map.find(cinfo->dst_node());
            assert(ndstit != layout.map.end());

            auto sdstit = ndstit->second.sockets.find(cinfo->dst_socket());
            assert(sdstit != ndstit->second.sockets.end());

            auto src_pos = sit->second.slot_pos;
            auto dst_pos = sdstit->second.slot_pos;

            auto chovered  = editor_ctx.is_hovered(c);
            auto cselected = editor_ctx.is_selected(c);

            // draw
            draw_connection(src_pos, dst_pos, chovered, cselected);
          }
        }
      }
    }

    void process_connection_channel(
      const imgui_context& imgui_ctx,
      const editor_context& editor_ctx,
      const canvas_layout& layout)
    {
      ImGui::GetWindowDrawList()->ChannelsSetCurrent(
        layout.connection_channel_index);

      draw_connection_channel(imgui_ctx, editor_ctx, layout);
    }

    // ------------------------------------------
    // Foreground

    void draw_foreground_channel(
      const imgui_context& imgui_ctx,
      const editor_context& editor_ctx,
      const canvas_layout& layout)
    {
      (void)imgui_ctx;

      auto dl = ImGui::GetWindowDrawList();

      auto& g = editor_ctx.node_graph();

      if (editor_ctx.get_state() == editor_state::background) {
        dl->AddRect(
          to_ImVec2(editor_ctx.get_drag_source_pos()),
          ImGui::GetMousePos(),
          ImColor(0, 0, 0, 255));
      }

      if (editor_ctx.get_state() == editor_state::socket) {

        // socket
        assert(editor_ctx.get_selected_sockets().size() == 1);
        auto s = editor_ctx.get_selected_sockets()[0];

        if (auto n = g.node(s)) {
          auto niter = layout.map.find(n);
          auto siter = niter->second.sockets.find(s);

          ImVec2 src, dst;

          if (g.get_info(s)->type() == socket_type::input) {
            src = ImGui::GetMousePos();
            dst = siter->second.slot_pos;
          }

          if (g.get_info(s)->type() == socket_type::output) {
            src = siter->second.slot_pos;
            dst = ImGui::GetMousePos();
          }

          if (
            to_ImVec2(editor_ctx.get_drag_source_pos()) != ImGui::GetMousePos())
            draw_connection(src, dst, false, false);
        }
      }
    }

    void process_foreground_channel(
      const imgui_context& imgui_ctx,
      const editor_context& editor_ctx,
      const canvas_layout& layout)
    {
      ImGui::GetWindowDrawList()->ChannelsSetCurrent(
        layout.foreground_channel_index);

      draw_foreground_channel(imgui_ctx, editor_ctx, layout);
    }

    // ------------------------------------------
    // toplevel

    [[nodiscard]] auto get_nodes_to_render(const editor_context& editor_ctx)
    {
      auto& g = editor_ctx.node_graph();

      auto group = editor_ctx.get_group();
      auto nodes = g.get_group_members(group);

      auto go = g.get_group_output(group);
      auto gi = g.get_group_input(group);

      if (!g.input_sockets(go).empty())
        nodes.push_back(go);

      if (!g.output_sockets(gi).empty())
        nodes.push_back(gi);

      return nodes;
    }

  } // namespace

  // impl
  void draw_node_canvas(imgui_context& imgui_ctx, editor_context& editor_ctx)
  {
    auto wname = "node region";
    auto wflag = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove
                 | ImGuiWindowFlags_AlwaysUseWindowPadding;
    auto wsize   = ImGui::GetContentRegionAvail();
    auto wborder = false;

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, {1, 1});
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0, 0});
    {
      if (ImGui::BeginChild(wname, wsize, wborder, wflag)) {

        auto dl = ImGui::GetWindowDrawList();

        editor_ctx.begin_frame();
        {
          // generate layout
          auto nodes  = get_nodes_to_render(editor_ctx);
          auto layout = calc_canvas_layout(nodes, editor_ctx);

          // split channels
          dl->ChannelsSplit(layout.channel_size);
          dl->ChannelsSetCurrent(layout.foreground_channel_index);
          {
            process_foreground_channel(imgui_ctx, editor_ctx, layout);
            process_node_channel(imgui_ctx, editor_ctx, layout);
            process_connection_channel(imgui_ctx, editor_ctx, layout);
            process_background_channel(imgui_ctx, editor_ctx, layout);
          }
          dl->ChannelsMerge();
        }
        editor_ctx.end_frame();
      }
      ImGui::EndChild();
    }
    ImGui::PopStyleVar(2);
  }
} // namespace yave::editor::imgui