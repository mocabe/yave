//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/wm/window.hpp>
#include <yave/config/config.hpp>
#include <yave/lib/imgui/imgui_context.hpp>
#include <yave/editor/data_context.hpp>
#include <yave/editor/view_context.hpp>

namespace yave::editor::imgui {

  /// For main menu and docking layout
  class root_window : public wm::window
  {
    // imgui
    yave::imgui::imgui_context& imgui_ctx;

    // first time layout
    bool layout_init;
    // root dockspace ID
    ImGuiID dockspace_id;

    // child window refs
    wm::window* node_canvas;
    wm::window* render_view;
    wm::window* info_view;

    bool open_save_modal  = false;
    bool open_load_modal  = false;
    bool open_usage_modal = false;
    bool open_about_modal = false;
    bool open_demo_window = false;

    // for save/load
    std::string filename = "yave.json";

  public:
    root_window(yave::imgui::imgui_context& im);
    ~root_window() noexcept;

  public:
    void draw(const editor::data_context&, const editor::view_context&)
      const override;
    void update(editor::data_context&, editor::view_context&) override;

  private:
    auto _name() const -> std::string;
    void _draw_menu_bar(const editor::view_context& vctx) const;

    void _draw_save_modal(
      const editor::data_context& dctx,
      const editor::view_context& vctx) const;

    void _draw_load_modal(
      const editor::data_context& dctx,
      const editor::view_context& vctx) const;

    void _draw_usage_modal(const editor::view_context& vctx) const;
    void _draw_about_modal(const editor::view_context& vctx) const;
    void _draw_imgui_demo(const editor::view_context& vctx) const;
    void _layout_once(const editor::view_context& vctx) const;
  };

} // namespace yave::editor::imgui