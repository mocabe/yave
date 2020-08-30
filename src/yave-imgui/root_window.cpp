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
    : viewport_window("editor::imgui::root_window")
    , imgui_ctx {im}
  {
    init_logger();

    layout_init = false;

    { // node canvas
      auto w      = std::make_unique<node_window>(im);
      node_canvas = w.get();
      add_child_window(children().size(), std::move(w));
    }

    { // render view
      auto w      = std::make_unique<render_view_window>(im);
      render_view = w.get();
      add_child_window(children().size(), std::move(w));
    }
  }

  root_window::~root_window() noexcept
  {
    // avoid resource destruction before finishing render tasks.
    imgui_ctx.window_context().device().waitIdle();
  }

  bool root_window::should_close() const
  {
    return imgui_ctx.window_context().should_close();
  }

  auto root_window::refresh_rate() const -> uint32_t
  {
    return 60;
  }

  void root_window::exec(editor::data_context& dctx, editor::view_context& vctx)
  {
    imgui_ctx.begin_frame();
    {
      update(dctx, vctx);
      draw(dctx, vctx);
    }
    imgui_ctx.end_frame();
    imgui_ctx.render();
  }

  void root_window::_draw_menu_bar() const
  {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(5.f, 5.f));
    // main menu bar
    if (ImGui::BeginMenuBar()) {
      if (ImGui::BeginMenu("File")) {
        if (ImGui::MenuItem("1")) {
        }
        if (ImGui::MenuItem("2")) {
        }
        if (ImGui::MenuItem("3")) {
        }
        ImGui::EndMenu();
      }
      ImGui::EndMenuBar();
    }
    ImGui::PopStyleVar(1);
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

    auto dock_right = ImGui::DockBuilderSplitNode(
      dock_main, ImGuiDir_Right, 0.2f, nullptr, &dock_main);

    // windows
    ImGui::DockBuilderDockWindow("inspector", dock_right);
    ImGui::DockBuilderDockWindow(node_canvas->name().c_str(), dock_bottom);
    ImGui::DockBuilderDockWindow("compiler", dock_bottom);
    ImGui::DockBuilderDockWindow(render_view->name().c_str(), dock_main);
    ImGui::DockBuilderFinish(dockid);

    ImGui::DockSpace(dockid);

    vctx.push(make_window_view_command(*this, [dockid](auto& w) {
      w.layout_init  = true;
      w.dockspace_id = dockid;
    }));
  }

  void root_window::update(
    editor::data_context& dctx,
    editor::view_context& vctx)
  {
    for (auto&& c : children())
      c->update(dctx, vctx);
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
                      ImGuiWindowFlags_MenuBar;      //

    // root window
    ImGui::Begin(name().c_str(), nullptr, root_flags);
    {
      // make layout
      _layout_once(vctx);
      // menu bar
      _draw_menu_bar();
    }
    ImGui::End();
    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor();

    // draw child windows
    for (auto&& c : children())
      c->draw(dctx, vctx);
  }

} // namespace yave::editor::imgui
