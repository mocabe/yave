//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave-imgui/editor/canvas.hpp>
#include <yave-imgui/editor/style.hpp>
#include <yave-imgui/editor/layout.hpp>

#include <yave/lib/imgui/extension.hpp>
#include <yave/node/decl/constructor.hpp>

#include <yave/support/log.hpp>

namespace yave::editor::imgui {

  namespace {

    using namespace yave::imgui;
    using namespace yave::app;

    ImVec2 to_ImVec2(const tvec2<float>& v)
    {
      return {v.x(), v.y()};
    }

    tvec2<float> to_tvec2(const ImVec2& v)
    {
      return {v.x, v.y};
    }

    // ------------------------------------------
    // Canvas

    /// draw background color, grid, etc.
    void draw_background_channel(
      imgui_context& imgui_ctx,
      editor_context& editor_ctx)
    {
      auto wpos  = ImGui::GetWindowPos();
      auto wsize = ImGui::GetWindowSize();
      auto dl    = ImGui::GetWindowDrawList();

      // setup
      ImGui::SetCursorScreenPos(wpos);
      dl->ChannelsSetCurrent(get_background_channel_index());

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
      editor_context& editor_ctx)
    {
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

      if (hovered) {
        // left: clear selected items
        if (ImGui::IsMouseClicked(0)) {
          editor_ctx.clear_selected_nodes();
          editor_ctx.clear_selected_sockets();
          editor_ctx.clear_selected_connections();
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
          auto npos = ImGui::GetMousePosOnOpeningCurrentPopup() - wpos - scroll;

          // list nodes avalable
          if (ImGui::BeginMenu("Add Node")) {
            for (auto&& decl : editor_ctx.node_graph().get_node_decls()) {
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

    // ------------------------------------------
    // Sockets

    void draw_socket_content(
      const std::string& name,
      socket_type type,
      const ImVec2& pos,
      const ImVec2& size,
      bool is_hovered,
      bool is_selected)
    {
      auto padding_x = gridpx(3) / 2;
      auto padding_y = gridpx(1);

      auto area_base = ImVec2 {pos.x + padding_x, pos.y + padding_y};
      auto area_size = ImVec2 {size.x - 2 * padding_x, size.y - 2 * padding_y};

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
        calc_text_pos(name, font_size_level::e15, area_size, align);

      auto col = get_socket_text_color();

      // socket name
      ImGui::SetCursorScreenPos(area_base + text_pos);
      ImGui::TextColored(col, "%s", name.c_str());
    }

    void draw_socket_slot(
      socket_type type,
      const ImVec2& pos,
      const ImVec2& size,
      bool is_hovered,
      bool is_selecetd)
    {
      auto slot_pos = calc_socket_slot_pos(type, pos, size);

      auto draw_list = ImGui::GetWindowDrawList();

      auto col = is_hovered ? get_socket_slot_color_hovered(type)
                            : is_selecetd ? get_socket_slot_color_selected(type)
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

    void draw_node_channel(imgui_context& imgui_ctx, editor_context& editor_ctx)
    {
      auto wpos   = ImGui::GetWindowPos();
      auto wsize  = ImGui::GetWindowSize();
      auto scroll = to_ImVec2(editor_ctx.get_scroll());
      auto dl     = ImGui::GetWindowDrawList();

      dl->ChannelsSetCurrent(get_node_channel_index());
      ImGui::SetCursorScreenPos(wpos);

      auto& g = editor_ctx.node_graph();

      /// (0,0) in screen coord
      auto origin = wpos + scroll;

      for (auto&& n : g.get_group_members(editor_ctx.get_group())) {

        auto node_pos  = origin + to_ImVec2(*editor_ctx.get_pos(n));
        auto node_name = *g.get_name(n);

        auto hovered  = editor_ctx.is_hovered(n);
        auto selected = editor_ctx.is_selected(n);

        auto bg_size = calc_node_background_size(n, g);
        auto hd_size = calc_node_header_size(node_name, bg_size);

        // draw background
        draw_node_background(node_pos, bg_size, hovered, selected);
        draw_node_header(node_name, node_pos, hd_size, hovered, selected);
        draw_node_edge(node_pos, bg_size, hovered, selected);

        // draw sockets

        ImVec2 cursor = {0, hd_size.y};

        for (auto&& s : g.output_sockets(n)) {

          auto socket_size = calc_socket_size(n, s, g);
          auto name        = *g.get_name(s);
          auto hovered     = editor_ctx.is_hovered(s);
          auto selected    = editor_ctx.is_selected(s);
          auto type        = socket_type::output;

          // draw
          draw_socket_content(
            name, type, node_pos + cursor, socket_size, hovered, selected);
          draw_socket_slot(
            type, node_pos + cursor, socket_size, hovered, selected);

          // move cursor
          cursor.y += socket_size.y;
        }

        for (auto&& s : g.input_sockets(n)) {

          auto socket_size = calc_socket_size(n, s, g);
          auto name        = *g.get_name(s);
          auto hovered     = editor_ctx.is_hovered(s);
          auto selected    = editor_ctx.is_selected(s);
          auto type        = socket_type::input;

          // draw
          draw_socket_content(
            name, type, node_pos + cursor, socket_size, hovered, selected);
          draw_socket_slot(
            type, node_pos + cursor, socket_size, hovered, selected);

          // move cursor
          cursor.y += socket_size.y;
        }
      }
    }

    struct SocketDnDPayload
    {
      socket_handle socket;
      socket_type type;
    };

    void handle_socket_input(
      const ImVec2& node_screen_pos,
      const socket_type& socket_type,
      const ImVec2& slot_pos,
      const float& slot_size,
      const socket_handle& s,
      const managed_node_graph& g,
      editor_context& editor_ctx)
    {
      ImGui::PushID(s.id().data);
      {
        // handle slot
        bool hovered;
        ImGui::SetCursorScreenPos(slot_pos - ImVec2 {slot_size, slot_size});
        InvisibleButtonEx("slot", {2 * slot_size, 2 * slot_size}, &hovered);

        if (hovered) {
          // set hovered
          editor_ctx.set_hovered(s);
          // left click: select
          // TODO:
          if (ImGui::IsMouseClicked(0)) {
            editor_ctx.clear_selected_nodes();
            editor_ctx.clear_selected_sockets();
            editor_ctx.clear_selected_connections();
            editor_ctx.add_selected(s);
            Info("output slot clicked!");
          }
        }

        // handle drag source
        if (ImGui::BeginDragDropSource()) {
          SocketDnDPayload payload {s, socket_type};
          ImGui::SetDragDropPayload(
            "SocketDnDPayload", &payload, sizeof(SocketDnDPayload));
          ImGui::Text("hello");
          ImGui::EndDragDropSource();
        }

        // handle drag target
        if (ImGui::BeginDragDropTarget()) {
          if (auto pld = ImGui::AcceptDragDropPayload("SocketDnDPayload")) {
            auto payload = (const SocketDnDPayload*)pld->Data;
            if (payload->type != socket_type) {
              // connect
              switch (socket_type) {
                case socket_type::input:
                  editor_ctx.connect(payload->socket, s);
                  break;
                case socket_type::output:
                  editor_ctx.connect(s, payload->socket);
                  break;
                default:
                  unreachable();
              }
            } else
              Info("invalid dnd");
            ImGui::EndDragDropTarget();
          }
        }
      }
      ImGui::PopID();
    }

    void handle_node_channel(
      imgui_context& imgui_ctx,
      editor_context& editor_ctx)
    {
      auto wpos   = ImGui::GetWindowPos();
      auto wsize  = ImGui::GetWindowSize();
      auto scroll = to_ImVec2(editor_ctx.get_scroll());
      auto origin = wpos + scroll;

      ImGui::SetCursorScreenPos(wpos);

      auto& g = editor_ctx.node_graph();

      auto nodes = g.get_group_members(editor_ctx.get_group());

      // traverse in reverse order
      for (auto it = nodes.rbegin(); it != nodes.rend(); ++it) {

        auto& n = *it;

        auto node_name        = *g.get_name(n);
        auto node_pos         = to_ImVec2(*editor_ctx.get_pos(n));
        auto node_screen_pos  = origin + node_pos;
        auto node_size        = calc_node_background_size(n, g);
        auto node_header_size = calc_node_header_size(node_name, node_size);

        // handle socket input

        ImVec2 cursor = {0, node_header_size.y};

        // output sockets
        for (auto&& s : g.output_sockets(n)) {

          auto socket_type = socket_type::output;
          auto socket_size = calc_socket_size(n, s, g);
          auto slot_pos    = calc_socket_slot_pos(
            socket_type, node_screen_pos + cursor, socket_size);
            auto slot_size = get_socket_slot_radius() * 2;

          handle_socket_input(
            node_screen_pos,
            socket_type,
                slot_pos,
            slot_size,
            s,
            g,
            editor_ctx);

          // move cursor
          cursor.y += socket_size.y;
        }

        // input sockets
        for (auto&& s : g.input_sockets(n)) {

          auto socket_type = socket_type::input;
          auto socket_size = calc_socket_size(n, s, g);
          auto slot_pos    = calc_socket_slot_pos(
            socket_type, node_screen_pos + cursor, socket_size);
            auto slot_size = get_socket_slot_radius() * 2;

          handle_socket_input(
            node_screen_pos,
            socket_type,
            slot_pos,
            slot_size,
            s,
            g,
            editor_ctx);

          // move cursor
          cursor.y += socket_size.y;
        }

        // handle node input
        ImGui::PushID(n.id().data);
        {
          ImGui::SetCursorScreenPos(node_screen_pos);

          const char* popup_name = "node_info_popup";

          bool hovered;
          InvisibleButtonEx("node", node_size, &hovered);

          if (hovered) {
            // set hover
            editor_ctx.set_hovered(n);
            // right click: select
            if (ImGui::IsMouseClicked(0)) {
              Info("node right cligk");
              editor_ctx.clear_selected_nodes();
              editor_ctx.clear_selected_sockets();
              editor_ctx.clear_selected_connections();
              editor_ctx.add_selected(n);
            }
            // left click: open info popup
            if (ImGui::IsMouseClicked(1)) {
              Info(popup_name);
              ImGui::OpenPopup("node_info_popup");
            }
          }

          // handle dragging
          if (
            ImGui::IsWindowFocused() && ImGui::IsMouseDragging(0)
            && editor_ctx.is_selected(n)) {
            auto delta = ImGui::GetIO().MouseDelta;
            if (delta != ImVec2(0, 0))
              editor_ctx.set_pos(n, to_tvec2(node_pos + delta));
          }

          if (ImGui::BeginPopup(popup_name)) {
            ImGui::Text("Node: %s", g.get_name(n)->c_str());
            ImGui::EndPopup();
          }
        }
        ImGui::PopID();
      }
    }

    // ------------------------------------------
    // toplevel

    template <class F>
    void node_region(
      imgui_context& imgui_ctx,
      editor_context& editor_ctx,
      F&& f)
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

          dl->ChannelsSplit(get_channel_count());
          dl->ChannelsSetCurrent(get_foreground_channel_index());
          {
            std::forward<F>(f)();
          }
          dl->ChannelsMerge();
        }
        ImGui::EndChild();
      }
      ImGui::PopStyleVar(2);
    }

  } // namespace

  // impl
  void draw_node_canvas(imgui_context& imgui_ctx, editor_context& editor_ctx)
  {
    // node canvas
    node_region(imgui_ctx, editor_ctx, [&] {
      // draw (bottom up)
      draw_background_channel(imgui_ctx, editor_ctx);
      draw_node_channel(imgui_ctx, editor_ctx);

      // handle input (top down)
      editor_ctx.begin_frame();
      {
        handle_node_channel(imgui_ctx, editor_ctx);
        handle_background_channel(imgui_ctx, editor_ctx);
      }
      editor_ctx.end_frame();
    });
  }
} // namespace yave::editor::imgui