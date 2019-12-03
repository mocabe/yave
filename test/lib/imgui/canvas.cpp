//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/lib/imgui/imgui_context.hpp>
#include <yave/lib/imgui/extension.hpp>
#include <selene/img/typed/ImageTypeAliases.hpp>
#include <selene/img/typed/TypedLayout.hpp>
#include <yave/lib/vulkan/vulkan_util.hpp>

#include <cmath>

using namespace yave;
using namespace yave::imgui;
using namespace ImGui;


void image_canvas(imgui_context& ctx)
{
  static bool show_test_canvas2 = true;

  // dummy image
  static sln::ImageRGBA_8u image = [] {
    sln::ImageRGBA_8u image {
      sln::TypedLayout {sln::PixelLength {800}, sln::PixelLength {450}}};

    for (auto&& strd : image) {
      for (auto&& pix : strd) {
        pix[0] = 0;
        pix[1] = 255;
        pix[2] = 0;
        pix[3] = 255;
      }
    }
    return image;
  }();

  static auto view = image.constant_view();

  // texture
  static auto tex = ctx.add_texture(
    "img",
    {(uint32_t)view.width().value(), (uint32_t)view.height().value()},
    view.total_bytes(),
    vk::Format::eR8G8B8A8Unorm,
    view.byte_ptr());

  assert(tex);

  if (!ImGui::Begin("image canvas", &show_test_canvas2)) {
    ImGui::End();
    return;
  }

  ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1, 1));
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
  ImGui::PushStyleColor(ImGuiCol_ChildWindowBg, IM_COL32(128, 128, 128, 255));

  ImGui::BeginChild(
    "canvas",
    {0, 0},
    true,
    ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove |
      ImGuiWindowFlags_NoScrollWithMouse);

  auto wsize           = ImGui::GetWindowSize();
  static ImVec2 size   = {(float)view.width(), (float)view.height()};
  static ImVec2 scroll = {};
  static float scale   = 1.0;

  ImGui::SetCursorPos({scale * scroll.x + wsize.x / 2 - scale * size.x / 2,
                       scale * scroll.y + wsize.y / 2 - scale * size.y / 2});

  ImGui::Image(tex, {size.x * scale, size.y * scale});

  // scroll
  if (
    ImGui::IsWindowHovered() && !ImGui::IsAnyItemActive() &&
    ImGui::IsMouseDragging(2, 0.0f)) {
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
  imgui_context imgui {};
  while (!imgui.window_context().should_close()) {
    imgui.begin();
    {
      image_canvas(imgui);
    }
    imgui.end();
    imgui.render();
  }
}