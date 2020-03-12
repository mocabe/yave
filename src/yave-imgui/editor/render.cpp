//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave-imgui/editor/render.hpp>
#include <yave/lib/image/image_view.hpp>

namespace yave::editor::imgui {

  void draw_render_window(
    yave::imgui::imgui_context& imgui_ctx,
    yave::app::editor_context&,
    const scene_config& config,
    const char* render_result_tex_name,
    const char* render_background_tex_name)
  {
    const_image_view view(
      config.width(), config.height(), config.frame_buffer_format());
    auto tex = imgui_ctx.find_texture(render_result_tex_name);

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1, 1));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(128, 128, 128, 255));

    ImGui::BeginChild(
      "render result",
      {0, 0},
      true,
      ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove
        | ImGuiWindowFlags_NoScrollWithMouse);

    auto wsize           = ImGui::GetWindowSize();
    static ImVec2 size   = {(float)view.width(), (float)view.height()};
    static ImVec2 scroll = {};
    static float scale   = 1.0;

    ImGui::SetCursorPos({scale * scroll.x + wsize.x / 2 - scale * size.x / 2,
                         scale * scroll.y + wsize.y / 2 - scale * size.y / 2});

    ImGui::Image(
      imgui_ctx.find_texture(render_background_tex_name),
      {size.x * scale, size.y * scale});

    ImGui::SetCursorPos({scale * scroll.x + wsize.x / 2 - scale * size.x / 2,
                         scale * scroll.y + wsize.y / 2 - scale * size.y / 2});

    if (tex)
      ImGui::Image(tex, {size.x * scale, size.y * scale});

    // scroll
    if (
      ImGui::IsWindowHovered() && !ImGui::IsAnyItemActive()
      && ImGui::IsMouseDragging(2, 0.0f)) {
      // need scale down input sensitivity
      auto delta = ImGui::GetIO().MouseDelta;
      scroll.x += delta.x / scale;
      scroll.y += delta.y / scale;
    }

    // scale
    if (ImGui::IsWindowHovered()) {
      // single tick = 10% zoom
      scale *= 1.0 + ImGui::GetIO().MouseWheel / 10.f;
    }

    ImGui::SetCursorPos({0, 0});
    ImGui::Text("sz: %f %f", wsize.x, wsize.y);
    ImGui::Text("scroll: %f %f", scroll.x, scroll.y);
    ImGui::Text("scale: %f", scale * 100);
    if (!tex)
      ImGui::Text("No render result");

    ImGui::EndChild();
    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor();
  }
} // namespace yave::editor::imgui