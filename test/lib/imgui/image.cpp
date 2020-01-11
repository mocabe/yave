//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/lib/imgui/imgui_context.hpp>
#include <yave/lib/vulkan/vulkan_util.hpp>
#include <selene/img/typed/ImageTypeAliases.hpp>
#include <selene/img/typed/TypedLayout.hpp>

#include <iostream>

using namespace yave;
using namespace yave::imgui;

int main()
{
  imgui_context imgui {1280, 720, "imgui"};

  auto layout = sln::TypedLayout(sln::PixelLength(800), sln::PixelLength(450));
  sln::ImageRGBA_8u image {layout};

  for (auto&& strd : image) {
    for (auto&& pix : strd) {
      pix[0] = 0;
      pix[1] = 255;
      pix[2] = 0;
      pix[3] = 255;
    }
  }

  auto view = image.constant_view();

  static auto tex = imgui.add_texture(
    "tex",
    {(uint32_t)view.width().value(), (uint32_t)view.height().value()},
    view.total_bytes(),
    vk::Format::eR8G8B8A8Unorm,
    view.byte_ptr());

  while (!imgui.window_context().should_close()) {
    imgui.begin();
    {
      ImGui::Image(
        tex, {(float)layout.width.value(), (float)layout.height.value()});
    }
    imgui.end();
    imgui.render();
  }

  imgui.vulkan_context().device().waitIdle();

  imgui.remove_texture("tex");
}