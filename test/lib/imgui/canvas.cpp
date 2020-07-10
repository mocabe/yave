//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/lib/imgui/imgui_context.hpp>
#include <yave/lib/imgui/extension.hpp>
#include <yave/lib/vulkan/vulkan_util.hpp>
#include <yave/lib/image/image.hpp>

#include <boost/gil.hpp>
#include <cmath>

using namespace yave;
using namespace yave::imgui;
using namespace ImGui;

void image_canvas(
  imgui_context& imgui,
  ImTextureID tex,
  vulkan::texture_data& tex_data)
{
  static bool show_test_canvas2 = true;

  if (!ImGui::Begin("image canvas", &show_test_canvas2)) {
    ImGui::End();
    return;
  }

  ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1, 1));
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
  ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(128, 128, 128, 255));

  ImGui::BeginChild(
    "canvas",
    {0, 0},
    true,
    ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove
      | ImGuiWindowFlags_NoScrollWithMouse);

  auto wsize         = ImGui::GetWindowSize();
  static ImVec2 size = {
    (float)tex_data.extent.width, (float)tex_data.extent.height};
  static ImVec2 scroll = {};
  static float scale   = 1.0;

  ImGui::SetCursorPos(
    {scale * scroll.x + wsize.x / 2 - scale * size.x / 2,
     scale * scroll.y + wsize.y / 2 - scale * size.y / 2});

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

  ImGui::EndChild();
  ImGui::PopStyleVar(2);
  ImGui::PopStyleColor();
  ImGui::End();
}

int main()
{
  vulkan::vulkan_context vkctx;
  imgui_context imgui {vkctx};

  auto image = yave::image(800, 450, image_format::rgba8);

  auto view = boost::gil::interleaved_view(
    image.width(),
    image.height(),
    (boost::gil::rgba8_ptr_t)image.data(),
    image.stride());

  for (auto&& pix : view) {
    pix[0] = 0;
    pix[1] = 255;
    pix[2] = 0;
    pix[3] = 255;
  }

  auto tex_data = imgui.create_texture(
    vk::Extent2D(view.width(), view.height()), vk::Format::eR8G8B8A8Unorm);

  imgui.write_texture(
    tex_data, {}, vk::Extent2D(view.width(), view.height()), image.data());

  auto tex = imgui.bind_texture(tex_data);

  while (!imgui.window_context().should_close()) {
    imgui.begin_frame();
    {
      image_canvas(imgui, tex, tex_data);
    }
    imgui.end_frame();
    imgui.render();
  }

  imgui.window_context().device().waitIdle();
  imgui.unbind_texture(tex_data);
}