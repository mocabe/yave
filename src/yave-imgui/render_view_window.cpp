//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave-imgui/render_view_window.hpp>

#include <yave/lib/image/image.hpp>
#include <yave/lib/image/image_view.hpp>
#include <yave/editor/editor_data.hpp>
#include <yave/editor/data_context.hpp>

namespace yave::editor {

  render_view_window::render_view_window(imgui::imgui_context& imctx)
    : wm::window("render_view")
    , imgui_ctx {imctx}
    , res_tex_id {0}
  {
    bg_tex_data =
      imgui_ctx.create_texture({1, 1}, vk::Format::eR32G32B32A32Sfloat);

    bg_tex_id = imgui_ctx.bind_texture(bg_tex_data);

    auto col = std::array<float, 4> {0, 0, 0, 1};
    imgui_ctx.clear_texture(bg_tex_data, vk::ClearColorValue(col));
  }

  render_view_window::~render_view_window() noexcept
  {
    if (res_tex_id)
      imgui_ctx.unbind_texture(res_tex_data);

    imgui_ctx.unbind_texture(bg_tex_data);
  }

  void render_view_window::update(
    editor::data_context& data_ctx,
    editor::view_context& view_ctx)
  {
    (void)view_ctx;

    auto lck   = data_ctx.lock();
    auto& data = data_ctx.data();

    width        = data.scene_config.width();
    height       = data.scene_config.height();
    frame_format = data.scene_config.frame_format();
    current_time = data.executor.time();

    // no update
    if (data.executor.timestamp() <= last_timestamp)
      return;

    // take execution result
    if (auto& fb = data.executor.get_result()) {

      if (!res_tex_id) {
        res_tex_data = imgui_ctx.create_texture(
          {fb->width(), fb->height()}, vk::Format::eR32G32B32A32Sfloat);
        res_tex_id = imgui_ctx.bind_texture(res_tex_data);
      }

      imgui_ctx.write_texture(
        res_tex_data, {0, 0}, res_tex_data.extent, fb->data());

      last_timestamp = data.executor.timestamp();
      exec_time      = data.executor.exec_time();
    }
  }

  void render_view_window::draw(
    editor::data_context& data_ctx,
    editor::view_context& view_ctx) const
  {
    const_image_view view(width, height, frame_format);

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
      auto scroll = this->tex_scroll;
      auto scale  = this->tex_scale;

      ImGui::SetCursorPos(
        {scale * scroll.x + wsize.x / 2 - scale * size.x / 2,
         scale * scroll.y + wsize.y / 2 - scale * size.y / 2});

      ImGui::Image(bg_tex_id, {size.x * scale, size.y * scale});

      ImGui::SetCursorPos(
        {scale * scroll.x + wsize.x / 2 - scale * size.x / 2,
         scale * scroll.y + wsize.y / 2 - scale * size.y / 2});

      if (res_tex_id)
        ImGui::Image(res_tex_id, {size.x * scale, size.y * scale});

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

      auto ms =
        std::chrono::duration_cast<std::chrono::milliseconds>(exec_time);
      ImGui::Text("%ld ms", ms.count());

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
        w.tex_scroll = scroll;
        w.tex_scale  = scale;
      }));
    }
    ImGui::End();
    ImGui::PopStyleVar(2);
  }
} // namespace yave::editor