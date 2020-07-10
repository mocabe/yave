//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/lib/imgui/imgui_context.hpp>
#include <yave/lib/vulkan/vulkan_util.hpp>
#include <yave/lib/image/image.hpp>

#include <boost/gil.hpp>

using namespace yave;
using namespace yave::imgui;

int main()
{
  vulkan::vulkan_context vkctx;
  imgui_context imgui {vkctx};

  auto image = yave::image(800, 400, image_format::rgba8);

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
      ImGui::Image(tex, {(float)view.width(), (float)view.height()});
    }
    imgui.end_frame();
    imgui.render();
  }

  imgui.window_context().device().waitIdle();
  imgui.unbind_texture(tex_data);
}