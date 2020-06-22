//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave-imgui/node_window_drawables.hpp>
#include <yave-imgui/node_window.hpp>

#include <yave/editor/editor_data.hpp>
#include <yave/module/std/primitive/primitive.hpp>
#include <imgui_stdlib.h>
#include <iostream>

namespace yave::editor::imgui {

  using namespace yave::imgui;

  struct basic_node_drawer : node_drawable
  {
    basic_node_drawer(
      const node_handle& n,
      const structured_node_graph& g,
      const node_window& nw)
      : node_drawable {n, *g.get_info(n)}
    {
    }

    struct socket_area
    {
      socket_handle socket;
      ImVec2 pos, size;
    };

    // calc node size and socket areas
    auto calc_area_layout(node_window_draw_info& draw_info) const
    {
      // header min size
      auto header_min_size = calc_node_header_min_size(info.name());
      // total node size
      auto node_size = header_min_size;
      // socket area list
      auto socket_areas = std::vector<socket_area>();
      {
        for (auto&& s : info.output_sockets()) {
          // minimum socket size
          auto minsz = draw_info.find_drawable(s)->min_size(draw_info);
          // add socket area
          socket_areas.push_back({s, {0, node_size.y}, minsz});
          // update size
          node_size = {std::max(node_size.x, minsz.x), node_size.y + minsz.y};
        }

        for (auto&& s : info.input_sockets()) {
          auto minsz = draw_info.find_drawable(s)->min_size(draw_info);
          socket_areas.push_back({s, {0, node_size.y}, minsz});
          node_size = {std::max(node_size.x, minsz.x), node_size.y + minsz.y};
        }
      }

      // match socket width to node width
      for (auto&& area : socket_areas) {
        area.size.x = node_size.x;
      }

      // calc header size
      auto header_size = node_size;
      header_size.y    = header_min_size.y;

      return std::make_tuple(header_size, node_size, socket_areas);
    }

    // draw node bg
    void draw_background(
      bool hovered,
      bool selected,
      const ImVec2& pos,
      const ImVec2& size) const
    {
      auto fill = hovered ? get_node_background_fill_color_hovered()
                          : selected ? get_node_background_fill_color_selected()
                                     : get_node_background_fill_color();

      auto rounding  = get_node_rounding();
      auto draw_list = ImGui::GetWindowDrawList();

      // fill
      draw_list->AddRectFilled(pos, pos + size, fill, rounding);
    }

    // draw node header
    void draw_header(
      bool hovered,
      bool selected,
      const ImVec2& pos,
      const ImVec2& size) const
    {
      auto fill = hovered ? get_node_header_fill_color_hovered()
                          : selected ? get_node_header_fill_color_selected()
                                     : get_node_header_fill_color();

      auto title     = info.name();
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

    void draw_edge(
      bool hovered,
      bool selected,
      const ImVec2& pos,
      const ImVec2& size) const
    {
      auto col = hovered ? get_node_edge_color_hovered()
                         : selected ? get_node_edge_color_selected()
                                    : get_node_edge_color();

      auto dl = ImGui::GetWindowDrawList();
      dl->AddRect(pos, pos + size, col, get_node_rounding());
    }

    void handle_input(
      const node_window& nw,
      data_context& dctx,
      view_context& vctx,
      bool hovered,
      bool selected,
      const ImVec2& pos,
      const ImVec2& size) const
    {
      const auto& n          = handle;
      const char* popup_name = "node popup";

      ImGui::PushID(n.id().data);
      {
        ImGui::SetCursorScreenPos(pos);

        bool hov = false;
        InvisibleButtonEx("node input", size, &hov);

        // neutral
        if (nw.state() == node_window::state::neutral) {

          if (hov) {

            // set hover
            vctx.push(
              make_window_view_command(nw, [n](auto& w) { w.set_hovered(n); }));

            // double click: inspect
            if (info.is_group() && ImGui::IsMouseDoubleClicked(0))
              vctx.push(
                make_window_view_command(nw, [n](auto& w) { w.set_group(n); }));

            // left click: select
            if (ImGui::IsMouseClicked(0) && !ImGui::IsMouseDoubleClicked(0)) {

              auto mpos = to_tvec2(ImGui::GetMousePos());

              if (nw.get_selected_nodes().size() <= 1)
                dctx.exec(make_data_command([n](auto& ctx) {
                  auto& g = ctx.data().node_graph;
                  g.bring_front(n);
                }));

              vctx.push(make_window_view_command(nw, [n, mpos](auto& w) {
                if (!w.is_selected(n))
                  w.clear_selected();
                w.add_selected(n);
                w.begin_node_drag(mpos);
              }));
            }

            // right click: open info popup
            if (ImGui::IsMouseClicked(1)) {
              ImGui::OpenPopup("node_info_popup");
            }
          }

          if (ImGui::BeginPopup(popup_name)) {
            ImGui::Text("Node: %s", info.name().c_str());
            ImGui::Text("id: %s", to_string(n.id()).c_str());
            ImGui::EndPopup();
          }
        }

        // node drag
        if (nw.state() == node_window::state::node) {
          if (ImGui::IsMouseReleased(0)) {
            if (nw.is_selected(n)) {

              auto new_pos =
                to_tvec2(pos - nw.scroll() - ImGui::GetWindowPos());

              // set new position
              dctx.exec(make_data_command(
                [n, new_pos](auto& ctx) {
                  ctx.data().node_graph.set_pos(n, new_pos);
                },
                [n, pos = info.pos()](auto& ctx) {
                  ctx.data().node_graph.set_pos(n, pos);
                }));

              // back to neutral
              if (n == nw.get_selected_nodes()[0])
                vctx.push(make_window_view_command(
                  nw, [](auto& w) { w.end_node_drag(); }));
            }
          }
        }
      }
      ImGui::PopID();
    }

    auto screen_pos(const node_window& nw, node_window_draw_info& draw_info)
      const -> ImVec2 override
    {
      auto spos = ImGui::GetWindowPos() + nw.scroll() + to_ImVec2(info.pos());

      // set dummy position to nodes being dragged, to prevent submitting
      // position updates every frame
      if (nw.state() == node_window::state::node && nw.is_selected(handle))
        spos = spos + ImGui::GetMousePos() - nw.drag_source();

      return spos;
    }

    auto size(node_window_draw_info& draw_info) const -> ImVec2 override
    {
      auto [hs, ns, sa] = calc_area_layout(draw_info);
      (void)hs;
      (void)sa;
      return ns;
    }

    auto socket_area_pos(
      node_window_draw_info& draw_info,
      const socket_handle& s) const -> ImVec2 override
    {
      auto [hs, ns, sa] = calc_area_layout(draw_info);
      (void)hs;
      (void)ns;

      for (auto&& [ss, apos, asize] : sa) {
        (void)asize;
        if (s == ss)
          return apos;
      }
      unreachable();
    }

    auto socket_area_size(
      node_window_draw_info& draw_info,
      const socket_handle& s) const -> ImVec2 override
    {
      auto [hs, ns, sa] = calc_area_layout(draw_info);
      (void)hs;
      (void)ns;

      for (auto&& [ss, apos, asize] : sa) {
        (void)apos;
        if (s == ss)
          return asize;
      }
      unreachable();
    }

    void draw(
      const node_window& nw,
      data_context& dctx,
      view_context& vctx,
      node_window_draw_info& draw_info,
      ImDrawListSplitter& splitter,
      size_t channel) const override
    {
      // layout
      auto [header_size, node_size, socket_areas] = calc_area_layout(draw_info);

      // draw sockets
      // assume channels are reserved for sockets
      auto socket_ch = channel + 1;

      auto screen_pos = this->screen_pos(nw, draw_info);

      // draw sockets
      for (auto&& [s, apos, asize] : socket_areas) {
        draw_info.find_drawable(s)->draw(
          nw,
          dctx,
          vctx,
          draw_info,
          splitter,
          socket_ch,
          screen_pos + apos,
          asize);

        ++socket_ch;
      }

      // draw node
      splitter.SetCurrentChannel(ImGui::GetWindowDrawList(), channel);
      {
        bool hov = nw.is_hovered(handle);
        bool sel = nw.is_selected(handle);

        draw_background(hov, sel, screen_pos, node_size);
        draw_header(hov, sel, screen_pos, header_size);
        draw_edge(hov, sel, screen_pos, node_size);

        // handle node input
        handle_input(nw, dctx, vctx, hov, sel, screen_pos, node_size);
      }
    }
  };

  struct basic_socket_drawer : socket_drawable
  {
    object_ptr<Object> data;

    basic_socket_drawer(
      const socket_handle& s,
      const structured_node_graph& g,
      const node_window& nw)
      : socket_drawable {s, *g.get_info(s)}
    {
      data = g.get_data(s);
    }

    auto min_size(node_window_draw_info&) const -> ImVec2 override
    {
      auto name      = info.name();
      auto text_size = calc_text_size(name, font_size_level::e15);
      auto padding   = get_socket_padding();
      // fix 16 px height
      return {text_size.x + 2 * padding.x, gridpx(2) + 2 * padding.y};
    }

    auto slot_pos(node_window_draw_info& draw_info, ImVec2 size) const
      -> ImVec2 override
    {
      auto type = info.type();

      switch (type) {
        case socket_type::input:
          return {size.x, size.y / 2};
        case socket_type::output:
          return {0, size.y / 2};
      }
      unreachable();
    }

    void draw_slot(
      const node_window& nw,
      data_context& dctx,
      view_context& vctx,
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

    void draw_content(
      const node_window& nw,
      data_context& dctx,
      view_context& vctx,
      node_window_draw_info& draw_info,
      ImVec2 pos,
      ImVec2 size) const
    {
      auto s    = handle;
      auto n    = info.node();
      auto type = info.type();
      auto name = info.name();

      bool has_custom_data = false;

      if (data && info.connections().empty()) {

        auto slider_bg_col   = get_socket_slider_color();
        auto slider_text_col = get_socket_slider_text_color();

        // custom data handling
        if (auto holder = value_cast_if<DataTypeHolder>(data)) {

          has_custom_data = true;

          ImGui::PushID(s.id().data);
          ImGui::SetCursorScreenPos(pos);
          ImGui::PushItemWidth(size.x);
          ImGui::PushStyleColor(ImGuiCol_Text, ImU32(slider_text_col));
          ImGui::PushStyleColor(ImGuiCol_FrameBg, ImU32(slider_bg_col));
          ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImU32(slider_bg_col));
          ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImU32(slider_bg_col));
          ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, get_node_rounding());
          {
            /*  */ if (auto f = value_cast_if<Float>(holder->data())) {
              float val = *f;
              ImGui::DragFloat("", &val);
              *f = val;
            } else if (auto i = value_cast_if<Int>(holder->data())) {
              int val = *i;
              ImGui::DragInt("", &val);
              *i = val;
            } else if (auto b = value_cast_if<Bool>(holder->data())) {
              ImGui::Checkbox("", &*b);
            } else if (auto s = value_cast_if<String>(holder->data())) {
              std::string val = *s;
              ImGui::InputText("", &val);
              *s = yave::string(val);
            } else
              // invalid data type. fallback to text
              has_custom_data = false;
          }
          ImGui::PopStyleColor(4);
          ImGui::PopStyleVar(1);
          ImGui::PopItemWidth();
          ImGui::PopID();
        }
      }

      // draw generic socket name as text
      if (!has_custom_data) {

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
    }

    void draw(
      const node_window& nw,
      data_context& dctx,
      view_context& vctx,
      node_window_draw_info& draw_info,
      ImDrawListSplitter& splitter,
      size_t channel,
      ImVec2 pos,
      ImVec2 size) const override
    {
      splitter.SetCurrentChannel(ImGui::GetWindowDrawList(), channel);

      // draw slots
      auto slotpos = pos + slot_pos(draw_info, size);
      draw_slot(nw, dctx, vctx, draw_info, slotpos);
      // draw content
      auto padding = ImVec2 {gridpx(1), gridpx(1)};
      draw_content(
        nw, dctx, vctx, draw_info, pos + padding, size - padding * 2);
    }
  };

  struct basic_connection_drawer : connection_drawable
  {
    basic_connection_drawer(
      const connection_handle& c,
      const structured_node_graph& g,
      const node_window& nw)
      : connection_drawable {c, *g.get_info(c)}
    {
    }

    void draw(
      const node_window& nw,
      data_context& dctx,
      view_context& vctx,
      node_window_draw_info& draw_info,
      ImDrawListSplitter& splitter,
      int channel) override
    {
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
  };

  auto create_node_drawable(
    const node_handle& n,
    const structured_node_graph& g,
    const node_window& nw) -> std::unique_ptr<node_drawable>
  {
    return std::make_unique<basic_node_drawer>(n, g, nw);
  }

  auto create_socket_drawable(
    const socket_handle& s,
    const structured_node_graph& g,
    const node_window& nw) -> std::unique_ptr<socket_drawable>
  {
    return std::make_unique<basic_socket_drawer>(s, g, nw);
  }

  auto create_connection_drawable(
    const connection_handle& c,
    const structured_node_graph& g,
    const node_window& nw) -> std::unique_ptr<connection_drawable>
  {
    return std::make_unique<basic_connection_drawer>(c, g, nw);
  }

} // namespace yave::editor::imgui