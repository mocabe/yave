//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave-imgui/node_window.hpp>

#include <yave/editor/editor_data.hpp>
#include <yave/support/log.hpp>

#include <imgui_internal.h>

YAVE_DECL_G_LOGGER(node_window);

namespace yave::editor::imgui {

  using namespace yave::imgui;

  void draw_connection_line(
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
  }

  node_window::node_window(yave::imgui::imgui_context& im)
    : window {"node_window"}
    , imgui_ctx {im}
  {
    init_logger();

    current_state   = state::neutral;
    scroll_pos      = {0, 0};
    drag_source_pos = {0, 0};
  }

  node_window::~node_window() noexcept = default;

  auto node_window::_create_draw_info(const structured_node_graph& g)
    -> node_window_draw_info
  {
    assert(g.exists(current_group));

    auto nodes = g.get_group_nodes(current_group);

    node_window_draw_info info;

    std::vector<connection_handle> cs;

    for (auto&& n : nodes) {

      info.nodes.push_back(create_node_drawable(n, g, *this));

      for (auto&& s : g.input_sockets(n))
        info.sockets.push_back(create_socket_drawable(s, g, *this));

      for (auto&& s : g.output_sockets(n))
        info.sockets.push_back(create_socket_drawable(s, g, *this));

      // list connections
      auto ics = g.input_connections(n);
      auto ocs = g.output_connections(n);
      cs.insert(cs.end(), ics.begin(), ics.end());
      cs.insert(cs.end(), ocs.begin(), ocs.end());
    }

    // unique connection list
    {
      std::sort(cs.begin(), cs.end());
      auto it = std::unique(cs.begin(), cs.end());
      cs.erase(it, cs.end());
    }

    for (auto&& c : cs)
      info.connections.push_back(create_connection_drawable(c, g, *this));

    info.channel_size             = info.nodes.size() + info.sockets.size() + 3;
    info.background_channel_index = 0;
    info.connection_channel_index = 1;
    info.node_channel_index_base  = 2;
    info.foreground_channel_index = info.channel_size - 1;

    return info;
  }

  void node_window::update(
    editor::data_context& data_ctx,
    editor::view_context& view_ctx)
  {
    auto lck = data_ctx.lock();
    auto& g  = data_ctx.data().node_graph;

    if (!g.exists(current_group) || !g.is_group(current_group)) {
      Info(g_logger, "Reset current_group to /root");
      current_group = g.search_path("/root").at(0);
    }
    current_group_path = *g.get_path(current_group);

    draw_info = std::make_unique<node_window_draw_info>(_create_draw_info(g));
    decls     = data_ctx.data().node_decls.enumerate();
  }

  void node_window::_draw_background(
    data_context& dctx,
    view_context& vctx,
    node_window_draw_info& di,
    ImDrawListSplitter& chs) const
  {
    chs.SetCurrentChannel(
      ImGui::GetWindowDrawList(), di.background_channel_index);

    // draw stuff (WIP)

    auto wpos  = ImGui::GetWindowPos();
    auto wsize = ImGui::GetWindowSize();
    auto dl    = ImGui::GetWindowDrawList();

    // cover entire canvas to hover test
    ImGui::SetCursorScreenPos(wpos);
    bool hov = false;
    InvisibleButtonEx("node background input", wsize, &hov);

    // recover cursor
    ImGui::SetCursorScreenPos(wpos);

    // draw background
    dl->AddRectFilled(wpos, wpos + wsize, get_background_color());

    // draw grid
    auto scroll = to_ImVec2(scroll_pos);
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
    ImGui::Text("path: %s", current_group_path.c_str());
    ImGui::Text("scroll: %f, %f", scroll.x, scroll.y);
    ImGui::Text("state: %d", (int)current_state);
    ImGui::Text("hov: %s", to_string(n_hovered.id()).c_str());
    ImGui::Text("bg hov: %d", (int)hov);

    if (s_selected)
      ImGui::Text("ssel: %s", to_string(s_selected.id()).c_str());

    for (auto&& n : n_selected)
      ImGui::Text("nsel: %s", to_string(n.id()).c_str());


    // handle scrolling
    if (
      ImGui::IsWindowHovered() && !ImGui::IsAnyItemActive()
      && ImGui::IsMouseDragging(2)) {

      auto delta = ImGui::GetIO().MouseDelta;

      vctx.push(make_view_command([scroll, delta, id = id()](auto& ctx) {
        if (auto w = ctx.window_manager().get_window(id)) {
          auto _this        = w->template as<node_window>();
          _this->scroll_pos = to_tvec2(scroll + delta);
        }
      }));
    }

    auto popup_name = "node background popup menu";

    // netral
    if (current_state == node_window::state::neutral) {

      // clear hover/select state when out of window
      auto wrect = ImRect {wpos, wpos + wsize};
      if (!wrect.Contains(ImGui::GetMousePos())) {

        if (ImGui::IsMouseClicked(0))
          vctx.push(make_window_view_command(
            *this, [](auto& w) { w.clear_selected(); }));

        vctx.push(
          make_window_view_command(*this, [](auto& w) { w.clear_hovered(); }));
      }

      if (hov) {
        // clear hover
        vctx.push(
          make_window_view_command(*this, [](auto& w) { w.clear_hovered(); }));

        // left: clear selected items
        if (ImGui::IsMouseClicked(0))
          vctx.push(make_window_view_command(*this, [](auto& w) {
            w.clear_selected();
            w.begin_background_drag(to_tvec2(ImGui::GetMousePos()));
          }));

        // right: open popup menu
        if (ImGui::IsMouseClicked(1)) {
          ImGui::OpenPopup(popup_name);
        }
      }

      // popup menu
      ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {8, 8});
      ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, {3, 3});
      if (ImGui::BeginPopup(popup_name)) {

        // pos to create node
        auto npos = ImGui::GetMousePosOnOpeningCurrentPopup() - wpos
                    - to_ImVec2(scroll_pos);

        // list nodes avalable
        if (ImGui::BeginMenu("Add Node")) {
          for (auto&& decl : decls) {
            if (ImGui::Selectable(decl->name().c_str())) {

              struct dcmd_ncreate : data_command
              {
                ImVec2 npos;
                node_handle group;
                std::string npath;

                // created node
                node_handle node = {};

                void exec(data_context& ctx) override
                {
                  auto& g = ctx.data().node_graph;
                  auto n  = g.search_path(npath);

                  if (n.size() == 1) {
                    node = g.create_copy(group, n[0]);
                    g.set_pos(node, {npos.x, npos.y});
                    assert(node);
                  }
                }

                void undo(data_context& ctx) override
                {
                  auto& g = ctx.data().node_graph;

                  if (g.exists(node)) {
                    g.destroy(node);
                    node = {};
                  }
                }
              };

              auto cmd   = std::make_unique<dcmd_ncreate>();
              cmd->npos  = npos;
              cmd->group = current_group;
              cmd->npath = decl->qualified_name();
              dctx.exec(std::move(cmd));

              break;
            }
          }
          ImGui::EndMenu();
        }
        ImGui::EndPopup();
      }
      ImGui::PopStyleVar(2);
    }

    // background drag
    if (current_state == node_window::state::background) {
      if (ImGui::IsMouseReleased(0)) {

        auto p1   = to_ImVec2(drag_source_pos);
        auto p2   = ImGui::GetMousePos();
        auto rect = ImRect {p1, p2};

        assert(draw_info);
        for (auto&& nd : draw_info->nodes) {
          auto npos  = nd->screen_pos(*this, *draw_info);
          auto nsize = nd->size(*draw_info);
          auto nrect = ImRect {npos, npos + nsize};

          // fix inverted rect
          if (rect.Min.x > rect.Max.x)
            std::swap(rect.Min.x, rect.Max.x);
          if (rect.Min.y > rect.Max.y)
            std::swap(rect.Min.y, rect.Max.y);

          if (rect.Overlaps(nrect))
            vctx.push(make_window_view_command(
              *this, [n = nd->handle](auto& w) { w.add_selected(n); }));
        }

        vctx.push(make_window_view_command(
          *this, [](auto& w) { w.end_background_drag(); }));
      }
    }
  }

  void node_window::_draw_connections(
    data_context& dctx,
    view_context& vctx,
    node_window_draw_info& di,
    ImDrawListSplitter& chs) const
  {
    for (auto&& c : di.connections)
      c->draw(*this, dctx, vctx, di, chs, di.connection_channel_index);
  }

  void node_window::_draw_nodes(
    data_context& dctx,
    view_context& vctx,
    node_window_draw_info& di,
    ImDrawListSplitter& chs) const
  {
    auto idx = di.foreground_channel_index;

    for (auto&& n : di.nodes | ranges::views::reverse) {
      assert(idx >= di.node_channel_index_base);
      // channel
      idx -= 1; // node bg
      idx -= n->info.input_sockets().size();
      idx -= n->info.output_sockets().size();
      // draw node and sockets
      n->draw(*this, dctx, vctx, di, chs, idx);
    }

    if (current_state == state::neutral) {

      // Ctrl + G = grouping
      if (
        !n_selected.empty() &&                     //
        ImGui::IsKeyDown(GLFW_KEY_LEFT_CONTROL) && //
        ImGui::IsKeyPressed(GLFW_KEY_G)) {

        // FIXME: supoprt undo
        dctx.exec(
          make_data_command([g = current_group, ns = n_selected](auto& ctx) {
            auto& ng = ctx.data().node_graph;

            auto newg = ng.create_group(g, {ns});

            fvec2 newpos = {};
            for (auto&& n : ns)
              if (auto np = ng.get_pos(n))
                newpos += *np;
            newpos /= ns.size();
            ng.set_pos(newg, newpos);

            ctx.data().compiler.notify_recompile();
          }));

        vctx.push(
          make_window_view_command(*this, [](auto& w) { w.clear_selected(); }));
      }

      // Delete nodes
      if (!n_selected.empty() && ImGui::IsKeyPressed(GLFW_KEY_DELETE)) {

        // FIXME support undo
        dctx.exec(make_data_command([ns = n_selected](auto& ctx) {
          auto& ng = ctx.data().node_graph;
          for (auto&& n : ns)
            ng.destroy(n);

          ctx.data().compiler.notify_recompile();
        }));

        vctx.push(
          make_window_view_command(*this, [](auto& w) { w.clear_selected(); }));
      }

      // Ctrol + U = move to upper group
      if (
        ImGui::IsKeyDown(GLFW_KEY_LEFT_CONTROL) && //
        ImGui::IsKeyPressed(GLFW_KEY_U)) {

        vctx.push(
          make_window_view_command(*this, [&dctx, g = current_group](auto& w) {
            auto lck   = dctx.lock();
            auto& ng   = dctx.data().node_graph;
            auto& root = dctx.data().root_group;

            assert(ng.exists(root));
            if (g == root)
              return;

            w.set_group(ng.get_parent_group(g));
          }));
      }
    }
  }

  void node_window::_draw_foreground(
    data_context& dctx,
    view_context& vctx,
    node_window_draw_info& di,
    ImDrawListSplitter& chs) const
  {
    chs.SetCurrentChannel(
      ImGui::GetWindowDrawList(), di.foreground_channel_index);

    auto dl = ImGui::GetWindowDrawList();

    if (current_state == state::background)
      dl->AddRect(
        to_ImVec2(drag_source_pos),
        ImGui::GetMousePos(),
        ImColor(0, 0, 0, 255));

    if (current_state == state::socket) {

      assert(s_selected);
      auto s = s_selected;

      if (auto& sd = draw_info->find_drawable(s)) {

        auto n = sd->info.node();
        if (auto& nd = draw_info->find_drawable(n)) {

          ImVec2 src, dst;

          if (sd->info.type() == socket_type::input) {
            src = ImGui::GetMousePos();
            dst =
              nd->screen_pos(*this, *draw_info)
              + nd->socket_area_pos(*draw_info, s)
              + sd->slot_pos(*draw_info, nd->socket_area_size(*draw_info, s));
          }

          if (sd->info.type() == socket_type::output) {
            src =
              nd->screen_pos(*this, *draw_info)
              + nd->socket_area_pos(*draw_info, s)
              + sd->slot_pos(*draw_info, nd->socket_area_size(*draw_info, s));
            dst = ImGui::GetMousePos();
          }

          if (to_ImVec2(drag_source_pos) != ImGui::GetMousePos())
            draw_connection_line(src, dst, false, false);
        }
      }
    }
  }

  void node_window::draw(
    editor::data_context& data_ctx,
    editor::view_context& view_ctx) const
  {
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, {1, 1});
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0, 0});
    ImGui::Begin(name().c_str());
    {
      auto wsize   = ImGui::GetContentRegionAvail();
      auto wborder = false;
      auto wflag   = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove
                   | ImGuiWindowFlags_AlwaysUseWindowPadding;

      ImGui::BeginChild(name().c_str(), wsize, wborder, wflag);
      {
        auto dl = ImGui::GetWindowDrawList();

        assert(draw_info);
        auto& di = *draw_info;

        ImDrawListSplitter channels;

        channels.Split(dl, di.channel_size);
        {
          _draw_foreground(data_ctx, view_ctx, di, channels);
          _draw_nodes(data_ctx, view_ctx, di, channels);
          _draw_connections(data_ctx, view_ctx, di, channels);
          _draw_background(data_ctx, view_ctx, di, channels);
        }
        channels.Merge(dl);
      }
      ImGui::EndChild();
    }
    ImGui::End();
    ImGui::PopStyleVar(2);
  }

  auto node_window::state() const -> enum state { return current_state; }

  void node_window::begin_background_drag(const fvec2& drag_src_pos)
  {
    if (current_state != state::neutral) {
      Warning("current_state != state::neutral");
      return;
    }
    Info(g_logger, "state: neutral -> background");
    current_state   = state::background;
    drag_source_pos = drag_src_pos;
  }

  void node_window::end_background_drag()
  {
    if (current_state != state::background) {
      Warning("current_state != state::background");
      return;
    }
    Info(g_logger, "state: background -> neutral");
    current_state   = state::neutral;
    drag_source_pos = {};
  }

  void node_window::begin_node_drag(const fvec2& drag_src_pos)
  {
    if (current_state != state::neutral) {
      Warning("current_state != state::neutral");
      return;
    }
    Info(g_logger, "state: neutral -> node");
    current_state   = state::node;
    drag_source_pos = drag_src_pos;
  }

  void node_window::end_node_drag()
  {
    if (current_state != state::node) {
      Warning("current_state != state::node");
      return;
    }
    Info(g_logger, "state: node -> neutral");
    current_state   = state::neutral;
    drag_source_pos = {};
  }

  void node_window::begin_socket_drag(const fvec2& drag_src_pos)
  {
    if (current_state != state::neutral) {
      Warning("current_state != state::neutral");
      return;
    }
    Info(g_logger, "state: neutral -> socket");
    current_state   = state::socket;
    drag_source_pos = drag_src_pos;
  }

  void node_window::end_socket_drag()
  {
    if (current_state != state::socket) {
      Warning("current_state != state::socket");
      return;
    }
    Info(g_logger, "state: socket -> neutral");
    current_state   = state::neutral;
    drag_source_pos = {};
  }

  auto node_window::scroll() const -> ImVec2
  {
    return to_ImVec2(scroll_pos);
  }

  auto node_window::drag_source() const -> ImVec2
  {
    return to_ImVec2(drag_source_pos);
  }

  auto node_window::group() const -> node_handle
  {
    return current_group;
  }

  void node_window::set_group(const node_handle& n)
  {
    current_group = n;
  }

  bool node_window::is_hovered(const node_handle& n) const
  {
    return n == n_hovered;
  }

  auto node_window::get_hovered_node() const -> std::optional<node_handle>
  {
    if (!n_hovered)
      return std::nullopt;

    return {n_hovered};
  }

  void node_window::set_hovered(const node_handle& n)
  {
    n_hovered = n;
  }

  void node_window::clear_hovered()
  {
    n_hovered = {};
  }

  bool node_window::is_selected(const node_handle& n) const
  {
    auto it = std::find(n_selected.begin(), n_selected.end(), n);
    return it != n_selected.end();
  }

  auto node_window::get_selected_nodes() const -> std::vector<node_handle>
  {
    return n_selected;
  }

  void node_window::add_selected(const node_handle& n)
  {
    if (!is_selected(n)) {
      n_selected.push_back(n);
    }
  }

  void node_window::clear_selected_nodes()
  {
    n_selected = {};
  }

  bool node_window::is_selected(const socket_handle& s) const
  {
    return s == s_selected;
  }

  auto node_window::get_selected_socket() const -> std::optional<socket_handle>
  {
    if (s_selected)
      return {s_selected};
    return std::nullopt;
  }

  void node_window::set_selected(const socket_handle& s)
  {
    s_selected = s;
  }

  void node_window::clear_selected_socket()
  {
    s_selected = {};
  }

  void node_window::clear_selected()
  {
    clear_selected_nodes();
    clear_selected_socket();
  }

} // namespace yave::editor::imgui