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
  imgui_context imgui(true);

  auto layout = sln::TypedLayout(sln::PixelLength(1280), sln::PixelLength(720));
  sln::ImageRGBA_8u image {layout};

  for (auto&& strd : image) {
    for (auto&& pix : strd) {
      pix[0] = 0;
      pix[1] = 255;
      pix[2] = 0;
      pix[3] = 255;
    }
  }

  auto view  = image.constant_view();
  auto& wCtx = imgui.window_context();
  auto& vCtx = imgui.vulkan_context();

  auto [vImg, vView, vMem] = vulkan::upload_image(
    view,
    wCtx.command_pool(),
    vCtx.graphics_queue(),
    vCtx.physical_device(),
    vCtx.device());

  auto vDsc = vulkan::create_descriptor(
    vView.get(),
    imgui.descriptor_set_layout(),
    imgui.descriptor_pool(),
    vCtx.device());

  auto set = vDsc.get();

  while (!imgui.window_context().should_close()) {
    imgui.begin();
    {
      ImGui::Image(&set, {(float)layout.width.value(), (float)layout.height.value()});
    }
    imgui.end();
    imgui.render();
  }

  imgui.vulkan_context().device().waitIdle();
}