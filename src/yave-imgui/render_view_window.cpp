//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave-imgui/render_view_window.hpp>

#include <yave/lib/image/image_view.hpp>
#include <yave/editor/editor_data.hpp>
#include <yave/editor/data_context.hpp>

namespace yave::editor {

  render_view_window::render_view_window(imgui::imgui_context& imctx)
    : imgui_ctx {imctx}
    , wm::window("render_view")
  {
    std::array<float, 4> bg_tex = {0, 0, 0, 1};

    (void)imgui_ctx.add_texture(
      bg_tex_name,
      {1, 1},
      sizeof(float) * 4,
      vk::Format::eR32G32B32A32Sfloat,
      (const uint8_t*)bg_tex.data());
  }

  void render_view_window::update(
    editor::data_context& data_ctx,
    editor::view_context& view_ctx)
  {
    auto lck   = data_ctx.lock();
    auto& data = data_ctx.data();

    width        = data.scene_config.width();
    height       = data.scene_config.height();
    frame_format = data.scene_config.frame_format();
    current_time = data.executor.time();

    // no update
    if (data.executor.timestamp() <= last_exec_time)
      return;

    // take execution result
    if (auto fb = data.executor.get_result()) {

      auto view = fb->const_view();

      if (!imgui_ctx.find_texture(res_tex_name))
        (void)imgui_ctx.add_texture(
          res_tex_name,
          vk::Extent2D(view.width(), view.height()),
          view.byte_size(),
          vk::Format::eR32G32B32A32Sfloat,
          (const uint8_t*)view.data());
      else
        imgui_ctx.update_texture(
          res_tex_name, (const uint8_t*)view.data(), view.byte_size());

      last_exec_time = data.executor.timestamp();
    }
  }

  void render_view_window::draw(
    editor::data_context& data_ctx,
    editor::view_context& view_ctx) const
  {
    const_image_view view(width, height, frame_format);
    auto tex = imgui_ctx.find_texture(res_tex_name);

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1, 1));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::Begin(name().c_str());
    {
      ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(64, 64, 64, 255));

      ImGui::BeginChild(
        "render_view child",
        {0, -23},
        true,
        ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove
          | ImGuiWindowFlags_NoScrollWithMouse);

      auto wsize  = ImGui::GetWindowSize();
      auto size   = ImVec2 {(float)view.width(), (float)view.height()};
      auto scroll = this->scroll;
      auto scale  = this->scale;

      ImGui::SetCursorPos(
        {scale * scroll.x + wsize.x / 2 - scale * size.x / 2,
         scale * scroll.y + wsize.y / 2 - scale * size.y / 2});

      ImGui::Image(
        imgui_ctx.find_texture(bg_tex_name), {size.x * scale, size.y * scale});

      ImGui::SetCursorPos(
        {scale * scroll.x + wsize.x / 2 - scale * size.x / 2,
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
      ImGui::PopStyleColor();

      float sec = current_time.seconds().count();
      ImGui::PushItemWidth(-1);
      ImGui::SliderFloat("s", &sec, 0, 100);
      ImGui::PopItemWidth();

      data_ctx.exec(
        make_data_command([t = yave::time::seconds(sec)](auto& ctx) {
          auto& data = ctx.data();
          if (data.executor.time() != t) {
            data.executor.set_time(t);
            data.executor.notify_execute();
          }
        }));

      view_ctx.push(make_window_view_command(*this, [=](auto& w) {
        w.scroll = scroll;
        w.scale  = scale;
      }));
    }
    ImGui::End();
    ImGui::PopStyleVar(2);
  }
} // namespace yave::editor