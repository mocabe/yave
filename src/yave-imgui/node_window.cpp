//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave-imgui/node_window.hpp>
#include <yave-imgui/data_commands.hpp>
#include <yave/editor/editor_data.hpp>
#include <yave/editor/data_command.hpp>
#include <yave/support/log.hpp>

#include <range/v3/view.hpp>
#include <imgui_internal.h>

YAVE_DECL_LOCAL_LOGGER(node_window)

namespace yave::editor::imgui {

  using namespace yave::imgui;
  namespace rn = ranges;
  namespace rv = ranges::views;

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
    current_state   = state::neutral;
    scroll_pos      = {0, 0};
    drag_source_pos = {0, 0};
  }

  node_window::~node_window() noexcept = default;

  auto node_window::_create_draw_info(const structured_node_graph& g)
    -> node_window_draw_info
  {
    assert(g.exists(current_group));

    auto nodes = g.get_group_members(current_group);

    // add IO handler which has socket
    {
      auto gi = g.get_group_input(current_group);
      auto go = g.get_group_output(current_group);

      if (!g.output_sockets(gi).empty())
        nodes.push_back(gi);

      if (!g.input_sockets(go).empty())
        nodes.push_back(go);
    }

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
    editor::view_context& /*view_ctx*/)
  {
    auto lck   = data_ctx.get_data<editor_data>();
    auto& data = lck.ref();
    auto& g    = data.node_graph();

    if (!g.exists(current_group) || !g.is_group(current_group)) {
      log_info( "Current group is no longer valid, reset to root group");
      current_group = data.root_group();
    }
    current_group_path = *g.get_path(current_group);

    draw_info = std::make_unique<node_window_draw_info>(_create_draw_info(g));
  }

  void node_window::_draw_background(
    const data_context& dctx,
    const view_context& vctx,
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

    // compiler errors
    auto compile_errors = [&] {
      auto&& lck  = dctx.get_data<editor_data>();
      auto&& data = lck.ref();
      auto&& msgs = data.compiler_data().last_message().get_errors();

      return msgs //
             | rv::transform([&](auto&& msg) { return msg.get_text(); })
             | rv::filter([](auto&& opt) { return opt.has_value(); })
             | rv::transform([](auto&& opt) { return opt.value(); })
             | rn::to_vector;
    }();

    for (auto&& e : compile_errors) {
      ImGui::TextColored({255, 0, 0, 255}, "%s", e.c_str());
    }

    // handle scrolling
    if (
      ImGui::IsWindowHovered() && !ImGui::IsAnyItemActive()
      && ImGui::IsMouseDragging(2)) {

      auto delta = ImGui::GetIO().MouseDelta;

      vctx.cmd(make_view_command([scroll, delta, id = id()](auto& ctx) {
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
          vctx.cmd(make_window_view_command(
            *this, [](auto& w) { w.clear_selected(); }));

        vctx.cmd(
          make_window_view_command(*this, [](auto& w) { w.clear_hovered(); }));
      }

      if (hov) {
        // clear hover
        vctx.cmd(
          make_window_view_command(*this, [](auto& w) { w.clear_hovered(); }));

        // left: clear selected items
        if (ImGui::IsMouseClicked(0))
          vctx.cmd(make_window_view_command(*this, [](auto& w) {
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

        // TODO: store tree info in update stage
        auto lck        = dctx.get_data<editor_data>();
        auto& data      = lck.ref();
        auto& ng        = data.node_graph();
        auto& decl_tree = data.node_declarations().get_pub_tree();

        if (ImGui::BeginMenu("New Node")) {

          auto build_menu_impl = [&](auto&& self, auto n) -> void {
            auto name = decl_tree.name(n);

            // leaf
            if (auto decl = decl_tree.decl(n)) {
              if (ImGui::Selectable(name.c_str())) {

                auto ns = ng.search_path(decl->full_name());

                if (!ns.empty()) {
                  dctx.cmd(
                    std::make_unique<dcmd_ncreate>(npos, current_group, ns[0]));
                }
              }
              return;
            }

            if (ImGui::BeginMenu(name.c_str())) {
              for (auto&& c : decl_tree.children(n)) {
                self(c);
              }
              ImGui::EndMenu();
            }
          };

          auto build_menu = fix_lambda(build_menu_impl);

          // skip root dir '/'
          for (auto c : decl_tree.children(decl_tree.root()))
            build_menu(c);

          ImGui::EndMenu();
        }

        if (ImGui::Selectable("New Group")) {
          dctx.cmd(std::make_unique<dcmd_gcreate>(npos, current_group));
        }

        if (data.root_group() != current_group) {

          if (ng.output_sockets(ng.get_group_input(current_group)).empty()) {
            if (ImGui::Selectable("New Input")) {
              dctx.cmd(std::make_unique<dcmd_sadd>(
                ng.get_group_input(current_group), socket_type::output, 0));
            }
          }

          if (ng.input_sockets(ng.get_group_output(current_group)).empty()) {
            if (ImGui::Selectable("New Output")) {
              dctx.cmd(std::make_unique<dcmd_sadd>(
                ng.get_group_output(current_group), socket_type::input, 0));
            }
          }
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
            vctx.cmd(make_window_view_command(
              *this, [n = nd->handle](auto& w) { w.add_selected(n); }));
        }

        vctx.cmd(make_window_view_command(
          *this, [](auto& w) { w.end_background_drag(); }));
      }
    }
  }

  void node_window::_draw_connections(
    const data_context& dctx,
    const view_context& vctx,
    node_window_draw_info& di,
    ImDrawListSplitter& chs) const
  {
    for (auto&& c : di.connections)
      c->draw(*this, dctx, vctx, di, chs, di.connection_channel_index);
  }

  void node_window::_draw_nodes(
    const data_context& dctx,
    const view_context& vctx,
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

        dctx.cmd(std::make_unique<dcmd_ngroup>(current_group, n_selected));

        vctx.cmd(
          make_window_view_command(*this, [](auto& w) { w.clear_selected(); }));
      }

      // Delete nodes
      if (!n_selected.empty() && ImGui::IsKeyPressed(GLFW_KEY_DELETE)) {

        dctx.cmd(std::make_unique<dcmd_ndestroy>(n_selected));

        vctx.cmd(
          make_window_view_command(*this, [](auto& w) { w.clear_selected(); }));
      }

      // Ctrol + U = move to upper group
      if (
        ImGui::IsKeyDown(GLFW_KEY_LEFT_CONTROL) && //
        ImGui::IsKeyPressed(GLFW_KEY_U)) {

        vctx.cmd(
          make_window_view_command(*this, [&dctx, g = current_group](auto& w) {
            auto lck   = dctx.template get_data<editor_data>();
            auto& data = lck.ref();
            auto& ng   = data.node_graph();

            auto root = data.root_group();

            assert(ng.exists(root));
            if (g == root)
              return;

            w.set_group(ng.get_parent_group(g));
          }));
      }
    }
  }

  void node_window::_draw_foreground(
    const data_context& /*dctx*/,
    const view_context& /*vctx*/,
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

          if (sd->info.is_input()) {
            src = ImGui::GetMousePos();
            dst =
              nd->screen_pos(*this, *draw_info)
              + nd->socket_area_pos(*draw_info, s)
              + sd->slot_pos(*draw_info, nd->socket_area_size(*draw_info, s));
          }

          if (sd->info.is_output()) {
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
    const editor::data_context& data_ctx,
    const editor::view_context& view_ctx) const
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

  void node_window::begin_background_drag(const glm::fvec2& drag_src_pos)
  {
    if (current_state != state::neutral) {
      log_warning("current_state != state::neutral");
      return;
    }
    current_state   = state::background;
    drag_source_pos = drag_src_pos;
  }

  void node_window::end_background_drag()
  {
    if (current_state != state::background) {
      log_warning("current_state != state::background");
      return;
    }
    current_state   = state::neutral;
    drag_source_pos = {};
  }

  void node_window::begin_node_drag(const glm::fvec2& drag_src_pos)
  {
    if (current_state != state::neutral) {
      log_warning("current_state != state::neutral");
      return;
    }
    current_state   = state::node;
    drag_source_pos = drag_src_pos;
  }

  void node_window::end_node_drag()
  {
    if (current_state != state::node) {
      log_warning("current_state != state::node");
      return;
    }
    current_state   = state::neutral;
    drag_source_pos = {};
  }

  void node_window::begin_socket_drag(const glm::fvec2& drag_src_pos)
  {
    if (current_state != state::neutral) {
      log_warning("current_state != state::neutral");
      return;
    }
    current_state   = state::socket;
    drag_source_pos = drag_src_pos;
  }

  void node_window::end_socket_drag()
  {
    if (current_state != state::socket) {
      log_warning("current_state != state::socket");
      return;
    }
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