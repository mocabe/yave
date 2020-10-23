//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave-imgui/root_window.hpp>
#include <yave-imgui/node_window.hpp>
#include <yave-imgui/render_view_window.hpp>

#include <yave/support/log.hpp>
#include <imgui_internal.h>

YAVE_DECL_G_LOGGER(im_root_window)

namespace yave::editor::imgui {

  root_window::root_window(yave::imgui::imgui_context& im)
    : window("editor::imgui::root_window")
    , imgui_ctx {im}
  {
    init_logger();

    layout_init = false;

    { // node canvas
      auto w      = std::make_unique<node_window>(im);
      node_canvas = w.get();
      add_any_window(children().end(), std::move(w));
    }

    { // render view
      auto w      = std::make_unique<render_view_window>(im);
      render_view = w.get();
      add_any_window(children().end(), std::move(w));
    }
  }

  root_window::~root_window() noexcept
  {
  }

  void root_window::_draw_menu_bar(const editor::view_context& vctx) const
  {
    bool open_usage = false;
    bool open_about = false;
    bool open_demo  = false;

    // main menu bar
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(5.f, 5.f));
    if (ImGui::BeginMenuBar()) {
      if (ImGui::BeginMenu("Help")) {
        ImGui::MenuItem("usage", nullptr, &open_usage);
        ImGui::MenuItem("about", nullptr, &open_about);
        ImGui::MenuItem("imgui demo", nullptr, &open_demo);
        ImGui::EndMenu();
      }
      ImGui::EndMenuBar();
    }
    ImGui::PopStyleVar(1);

    // set model
    vctx.cmd(make_window_view_command(*this, [=](auto& w) {
      // clang-format off
      if (open_usage) w.open_usage_modal = true;
      if (open_about) w.open_about_modal = true;
      if (open_demo) w.open_demo_window = true;
      // clang-format on
    }));
  }

  void root_window::_draw_usage_modal(const editor::view_context& vctx) const
  {
    if (ImGui::BeginPopupModal("usage")) {

      if (ImGui::Button("close"))
        ImGui::CloseCurrentPopup();

      ImGui::EndPopup();
    }

    if (open_usage_modal) {
      // open popup
      ImGui::OpenPopup("usage");
      // clear flag
      vctx.cmd(make_window_view_command(
        *this, [](auto& w) { w.open_usage_modal = false; }));
    }
  }

  void root_window::_draw_about_modal(const editor::view_context& vctx) const
  {
    if (ImGui::BeginPopupModal("about")) {

      if (ImGui::Button("close"))
        ImGui::CloseCurrentPopup();

      ImGui::EndPopup();
    }

    if (open_about_modal) {
      Info(g_logger, "about!");
      // open popup
      ImGui::OpenPopup("about");
      // clear flag
      vctx.cmd(make_window_view_command(
        *this, [](auto& w) { w.open_about_modal = false; }));
    }
  }

  void root_window::_draw_imgui_demo(const editor::view_context& vctx) const
  {
    if (!open_demo_window)
      return;

    // open demo window
    bool show = open_demo_window;
    ImGui::ShowDemoWindow(&show);

    // turn off flag when closed
    vctx.cmd(make_window_view_command(
      *this, [=](auto& w) { w.open_demo_window = show; }));
  }

  void root_window::_layout_once(const editor::view_context& vctx) const
  {
    // already initialized
    if (layout_init) {
      assert(dockspace_id);
      ImGui::DockSpace(dockspace_id);
      return;
    }

    auto dockid = ImGui::GetID("root_dockspace");

    ImGui::DockBuilderRemoveNode(dockid);
    ImGui::DockBuilderAddNode(dockid, ImGuiDockNodeFlags_DockSpace);
    ImGui::DockBuilderSetNodeSize(dockid, ImGui::GetWindowSize());

    auto dock_main = dockid;

    auto dock_bottom = ImGui::DockBuilderSplitNode(
      dock_main, ImGuiDir_Down, 0.5f, nullptr, &dock_main);

    // windows
    ImGui::DockBuilderDockWindow(node_canvas->name().c_str(), dock_bottom);
    ImGui::DockBuilderDockWindow(render_view->name().c_str(), dock_main);
    ImGui::DockBuilderFinish(dockid);

    ImGui::DockSpace(dockid);

    vctx.cmd(make_window_view_command(*this, [dockid](auto& w) {
      w.layout_init  = true;
      w.dockspace_id = dockid;
    }));
  }

  void root_window::update(
    editor::data_context& dctx,
    editor::view_context& vctx)
  {
    for (auto&& c : children())
      as_mut_child(c)->update(dctx, vctx);
  }

  void root_window::draw(
    const editor::data_context& dctx,
    const editor::view_context& vctx) const
  {
    auto& glfw_win = imgui_ctx.window_context().glfw_win();

    auto root_pos = ImVec2 {};
    auto root_size =
      ImVec2 {(float)glfw_win.size().x, (float)glfw_win.size().y};

    ImGui::SetNextWindowPos(root_pos);
    ImGui::SetNextWindowSize(root_size);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.f, 0.f));
    ImGui::PushStyleColor(ImGuiCol_WindowBg, {0.1, 0.1, 0.1, 1.f});

    auto root_flags = ImGuiWindowFlags_NoTitleBar |  //
                      ImGuiWindowFlags_NoScrollbar | //
                      ImGuiWindowFlags_NoResize |    //
                      ImGuiWindowFlags_MenuBar |     //
                      ImGuiWindowFlags_NoBringToFrontOnFocus;

    // root window
    ImGui::Begin(name().c_str(), nullptr, root_flags);
    {
      _layout_once(vctx);
      _draw_menu_bar(vctx);
      _draw_about_modal(vctx);
      _draw_usage_modal(vctx);
      _draw_imgui_demo(vctx);
    }
    ImGui::End();
    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor();

    // draw child windows
    for (auto&& c : children())
      c->draw(dctx, vctx);
  }

} // namespace yave::editor::imgui
