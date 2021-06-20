//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave-imgui/root_window.hpp>
#include <yave-imgui/node_window.hpp>
#include <yave-imgui/render_view_window.hpp>
#include <yave-imgui/info_window.hpp>

#include <yave-imgui/data_commands.hpp>

#include <yave/core/log.hpp>
#include <imgui_internal.h>
#include <imgui_stdlib.h>

YAVE_DECL_LOCAL_LOGGER(im_root_window)

namespace yave::editor::imgui {

  root_window::root_window(yave::imgui::imgui_context& im)
    : window("editor::imgui::root_window")
    , imgui_ctx {im}
  {
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

    { // info view
      auto w    = std::make_unique<info_window>();
      info_view = w.get();
      add_any_window(children().end(), std::move(w));
    }
  }

  root_window::~root_window() noexcept
  {
  }

  void root_window::_draw_menu_bar(const editor::view_context& vctx) const
  {
    bool open_save  = false;
    bool open_load  = false;
    bool open_usage = false;
    bool open_about = false;
    bool open_demo  = false;

    // main menu bar
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(5.f, 5.f));
    if (ImGui::BeginMenuBar()) {
      if (ImGui::BeginMenu("Edit")) {
        ImGui::MenuItem("save", nullptr, &open_save);
        ImGui::MenuItem("load", nullptr, &open_load);
        ImGui::EndMenu();
      }
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
      if (open_save) w.open_save_modal = true;
      if (open_load) w.open_load_modal = true;
      if (open_usage) w.open_usage_modal = true;
      if (open_about) w.open_about_modal = true;
      if (open_demo) w.open_demo_window = true;
      // clang-format on
    }));
  }

  void root_window::_draw_save_modal(
    const editor::data_context& dctx,
    const editor::view_context& vctx) const
  {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10.f, 10.f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 5.f);
    {
      auto flags = ImGuiWindowFlags_NoResize;
      if (ImGui::BeginPopupModal("save", nullptr, flags)) {

        ImGui::Text("Save current project");
        ImGui::Separator();

        ImGui::Text("filename:");
        ImGui::SetNextItemWidth(300);
        std::string tmp = filename;
        if (ImGui::InputText("", &tmp)) {
          vctx.cmd(make_window_view_command(
            *this, [=](auto& w) { w.filename = tmp; }));
        }

        if (ImGui::Button("ok")) {
          dctx.cmd(std::make_unique<dcmd_save>(filename));
          ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();

        if (ImGui::Button("cancel"))
          ImGui::CloseCurrentPopup();

        ImGui::EndPopup();
      }
    }
    ImGui::PopStyleVar(2);

    if (open_save_modal) {
      ImGui::OpenPopup("save");
      vctx.cmd(make_window_view_command(
        *this, [](auto& w) { w.open_save_modal = false; }));
    }
  }

  void root_window::_draw_load_modal(
    const editor::data_context& dctx,
    const editor::view_context& vctx) const
  {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10.f, 10.f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 5.f);
    {
      auto flags = ImGuiWindowFlags_NoResize;
      if (ImGui::BeginPopupModal("load", nullptr, flags)) {

        ImGui::Text("Load node grpah");
        ImGui::Spacing();

        ImGui::Text("filename:");
        ImGui::SetNextItemWidth(300);
        std::string tmp = filename;
        if (ImGui::InputText("", &tmp)) {
          vctx.cmd(make_window_view_command(
            *this, [=](auto& w) { w.filename = tmp; }));
        }

        if (ImGui::Button("ok")) {
          dctx.cmd(std::make_unique<dcmd_load>(filename));
          ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();

        if (ImGui::Button("cancel"))
          ImGui::CloseCurrentPopup();

        ImGui::EndPopup();
      }
    }
    ImGui::PopStyleVar(2);

    if (open_load_modal) {
      ImGui::OpenPopup("load");
      vctx.cmd(make_window_view_command(
        *this, [](auto& w) { w.open_load_modal = false; }));
    }
  }

  void root_window::_draw_usage_modal(const editor::view_context& vctx) const
  {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(5.f, 5.f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 5.f);
    {
      auto flags = ImGuiWindowFlags_NoResize;
      if (ImGui::BeginPopupModal("usage", nullptr, flags)) {

        ImGui::Text("Keyboard shortcuts");
        ImGui::Separator();

        ImGui::Bullet();
        ImGui::Text("Ctrl + G: Make new group from selected nodes.");

        ImGui::Bullet();
        ImGui::Text("Ctrl + U: Move to upper group.");

        ImGui::Bullet();
        ImGui::Text("Del: Delete selected nodes.");

        if (ImGui::Button("ok"))
          ImGui::CloseCurrentPopup();

        ImGui::EndPopup();
      }
    }
    ImGui::PopStyleVar(2);

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
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(5.f, 5.f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 5.f);
    {
      auto flags = ImGuiWindowFlags_NoResize;
      if (ImGui::BeginPopupModal("about", nullptr, flags)) {

        ImGui::Text("yave-imgui");
        ImGui::Separator();
        ImGui::Text("author: mocabe");
        ImGui::Text("version: 0.0.0");
        ImGui::Text("source: https://github.com/mocabe/yave");

        if (ImGui::Button("ok"))
          ImGui::CloseCurrentPopup();

        ImGui::EndPopup();
      }
    }
    ImGui::PopStyleVar(2);

    if (open_about_modal) {
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

    bool show = open_demo_window;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(5.f, 5.f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 5.f);
    {
      // demo window
      ImGui::ShowDemoWindow(&show);
    }
    ImGui::PopStyleVar(2);

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

    auto dock_root = ImGui::GetID("root_dockspace");

    ImGui::DockBuilderRemoveNode(dock_root);
    ImGui::DockBuilderAddNode(dock_root, ImGuiDockNodeFlags_DockSpace);
    ImGui::DockBuilderSetNodeSize(dock_root, ImGui::GetWindowSize());

    ImGuiID dock_node;
    ImGuiID dock_render;
    ImGuiID dock_info;

    ImGui::DockBuilderSplitNode(
      dock_root, ImGuiDir_Down, 0.5f, &dock_node, &dock_render);

    ImGui::DockBuilderSplitNode(
      dock_render, ImGuiDir_Left, 0.2f, &dock_info, &dock_render);

    // layout windows
    ImGui::DockBuilderDockWindow(node_canvas->name().c_str(), dock_node);
    ImGui::DockBuilderDockWindow(render_view->name().c_str(), dock_render);
    ImGui::DockBuilderDockWindow(info_view->name().c_str(), dock_info);
    ImGui::DockBuilderFinish(dock_root);

    ImGui::DockSpace(dock_root);

    vctx.cmd(make_window_view_command(*this, [=](auto& w) {
      w.layout_init  = true;
      w.dockspace_id = dock_root;
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
    ImGui::PushStyleColor(ImGuiCol_WindowBg, {0.1f, 0.1f, 0.1f, 1.f});

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
      _draw_save_modal(dctx, vctx);
      _draw_load_modal(dctx, vctx);
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
