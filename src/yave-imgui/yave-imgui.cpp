//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave-imgui/imgui_context.hpp>

#include <yave-imgui/vulkan_util.hpp>

#include <imgui.h>
#include <selene/img_io/IO.hpp>
#include <selene/img/interop/DynImageToImage.hpp>
#include <selene/img_ops/ImageConversions.hpp>
#include <boost/dll/runtime_symbol_info.hpp>
#include <fmt/format.h>

using namespace yave;
using namespace yave::imgui;

int main()
{
  auto path = boost::dll::program_location().remove_filename() / "inu.jpg";
  auto file = sln::FileReader(path.string());

  if (!file.is_open())
    throw;

  auto dynImg = sln::read_jpeg(file);
  auto rgbimg = sln::to_image<sln::PixelRGB_8u>(std::move(dynImg));
  auto img = sln::convert_image<sln::PixelFormat::RGBA>(rgbimg, uint8_t(255));

  imgui_context imgui(true);

  // upload image to GPU
  auto [image, imageView, imageMemory] = vulkan::upload_image(
    img.constant_view(),
    imgui.window_context().command_pool(),
    imgui.vulkan_context().graphics_queue(),
    imgui.vulkan_context().physical_device(),
    imgui.vulkan_context().device());

  auto tex = vulkan::create_descriptor(
    imageView.get(),
    imgui.descriptor_set_layout(),
    imgui.descriptor_pool(),
    imgui.vulkan_context().device());

  auto texId = imgui.get_texture_id(tex.get());

  while (!imgui.window_context().should_close()) {
    imgui.begin();
    {
      static bool show_demo_window = true;
      ImGui::ShowDemoWindow(&show_demo_window);

      ImGui::Begin("sibiinu");
      ImGui::Image(
        texId, {(float)img.width().value(), (float)img.height().value()});
      ImGui::End();

      ImGui::Begin("something");
      ImGui::End();
    }
    imgui.end();
    imgui.render();
  }
}