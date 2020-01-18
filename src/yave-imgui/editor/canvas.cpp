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
      bool hovered,
      bool selected)
    {
      (void)hovered;
      (void)selected;

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

    void draw_node(
      const node_handle& n,
      const node_layout& nlayout,
      const editor_context& editor_ctx)
    {
      auto& g = editor_ctx.node_graph();

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
      auto& g = editor_ctx.node_graph();

      auto spos      = slayout.pos;
      auto ssize     = slayout.size;
      auto shovered  = editor_ctx.is_hovered(s);
      auto sselected = editor_ctx.is_selected(s);
      auto sname     = *g.get_name(s);
      auto stype     = g.get_info(s)->type();

      // draw
      draw_socket_content(sname, stype, spos, ssize, shovered, sselected);
      draw_socket_slot(stype, spos, ssize, shovered, sselected);
    }

    void draw_node_channel(
      const imgui_context& imgui_ctx,
      const editor_context& editor_ctx,
      const canvas_layout& layout)
    {
      (void)imgui_ctx;

      auto wpos = ImGui::GetWindowPos();
      auto dl   = ImGui::GetWindowDrawList();

      dl->ChannelsSetCurrent(get_node_channel_index());
      ImGui::SetCursorScreenPos(wpos);

      auto& g = editor_ctx.node_graph();

      for (auto&& n : layout.nodes) {

        auto niter = layout.map.find(n);
        assert(niter != layout.map.end());

        auto& nlayout = niter->second;

        draw_node(n, nlayout, editor_ctx);

        for (auto&& s : g.output_sockets(n)) {

          auto siter = niter->second.sockets.find(s);
          assert(siter != niter->second.sockets.end());

          auto& slayout = siter->second;

          draw_socket(s, slayout, editor_ctx);
        }

        for (auto&& s : g.input_sockets(n)) {

          auto siter = niter->second.sockets.find(s);
          assert(siter != niter->second.sockets.end());

          auto& slayout = siter->second;

          draw_socket(s, slayout, editor_ctx);
        }
      }
    }

    struct SocketDnDPayload
    {
      socket_handle socket;
      socket_type type;
    };

    void handle_socket_input(
      const node_handle& n,
      const socket_handle& s,
      const canvas_layout& layout,
      editor_context& editor_ctx)
    {
      auto& g = editor_ctx.node_graph();

      auto niter = layout.map.find(n);
      auto siter = niter->second.sockets.find(s);

      auto& slayout = siter->second;

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
          // set hovered
          editor_ctx.set_hovered(s);
          // left click: select
          // TODO:
          if (ImGui::IsMouseClicked(0)) {
            editor_ctx.clear_selected();
            editor_ctx.add_selected(s);
          }
        }

        // handle drag source
        if (ImGui::BeginDragDropSource()) {
          SocketDnDPayload payload {s, socket_type};
          ImGui::SetDragDropPayload(
            "SocketDnDPayload", &payload, sizeof(SocketDnDPayload));
          ImGui::Text("%s::%s", g.get_name(n)->c_str(), g.get_name(s)->c_str());
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

    void handle_node_input(
      const node_handle& n,
      const canvas_layout& layout,
      editor_context& editor_ctx)
    {
      auto& g = editor_ctx.node_graph();

      auto niter = layout.map.find(n);
      assert(niter != layout.map.end());

      auto& nlayout = niter->second;

      auto npos  = nlayout.pos;
      auto nsize = nlayout.size;

      // handle node input
      ImGui::PushID(n.id().data);
      {
        ImGui::SetCursorScreenPos(npos);

        const char* popup_name = "node_info_popup";

        bool hovered;
        InvisibleButtonEx("node", nsize, &hovered);

        if (hovered) {
          // set hover
          editor_ctx.set_hovered(n);
          // right click: select
          if (ImGui::IsMouseClicked(0)) {
            editor_ctx.clear_selected();
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
            editor_ctx.set_pos(n, *editor_ctx.get_pos(n) + to_tvec2(delta));
        }

        if (ImGui::BeginPopup(popup_name)) {
          ImGui::Text("Node: %s", g.get_name(n)->c_str());
          ImGui::EndPopup();
        }
        }
        ImGui::PopID();
    }

    void handle_node_channel(
      imgui_context& imgui_ctx,
      editor_context& editor_ctx,
      const canvas_layout& layout)
    {
      (void)imgui_ctx;

      auto wpos = ImGui::GetWindowPos();
      ImGui::SetCursorScreenPos(wpos);

      auto& g     = editor_ctx.node_graph();
      auto& nodes = layout.nodes;

      // traverse in reverse order
      for (auto it = nodes.rbegin(); it != nodes.rend(); ++it) {

        auto& n = *it;

        for (auto&& s : g.output_sockets(n)) {
          handle_socket_input(n, s, layout, editor_ctx);
        }

        for (auto&& s : g.input_sockets(n)) {
          handle_socket_input(n, s, layout, editor_ctx);
        }

        handle_node_input(n, layout, editor_ctx);
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

      auto dl = ImGui::GetWindowDrawList();
      dl->ChannelsSetCurrent(get_connection_channel_index());

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

    // ------------------------------------------
    // Foreground
    void draw_foreground_channel(
      const imgui_context& imgui_ctx,
      const editor_context& editor_ctx,
      const canvas_layout& layout)
    {
      (void)imgui_ctx;

      auto dl = ImGui::GetWindowDrawList();
      dl->ChannelsSetCurrent(get_foreground_channel_index());

      auto& g = editor_ctx.node_graph();

      // connection
      if (editor_ctx.is_socket_selected() && ImGui::IsMouseDragging(0)) {

        auto ss = editor_ctx.get_selected_sockets();

        if (ss.size() == 1) {
          // socket
          auto s = ss.front();

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

            draw_connection(src, dst, false, false);
          }
        }
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
      (void)imgui_ctx;
      (void)editor_ctx;

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

    [[nodiscard]] auto get_nodes_to_render(const editor_context& editor_ctx)
    {
      auto& g = editor_ctx.node_graph();

      auto group = editor_ctx.get_group();
      auto nodes = g.get_group_members(group);
      nodes.push_back(g.get_group_output(group));
      nodes.push_back(g.get_group_input(group));
      return nodes;
    }

  } // namespace

  // impl
  void draw_node_canvas(imgui_context& imgui_ctx, editor_context& editor_ctx)
  {
    node_region(imgui_ctx, editor_ctx, [&] {
      /* handle input */
      editor_ctx.begin_frame();
      auto nodes  = get_nodes_to_render(editor_ctx);
      auto layout = calc_canvas_layout(nodes, editor_ctx);
      {
        handle_node_channel(imgui_ctx, editor_ctx, layout);
        handle_background_channel(imgui_ctx, editor_ctx, layout);
      }
      editor_ctx.end_frame();
      /* draw canvas */
      draw_connection_channel(imgui_ctx, editor_ctx, layout);
      draw_background_channel(imgui_ctx, editor_ctx, layout);
      draw_node_channel(imgui_ctx, editor_ctx, layout);
      draw_foreground_channel(imgui_ctx, editor_ctx, layout);
    });
  }
} // namespace yave::editor::imgui