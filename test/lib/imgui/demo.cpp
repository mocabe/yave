//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/lib/imgui/imgui_context.hpp>

using namespace yave;
using namespace yave::imgui;

int main()
{
  vulkan::vulkan_context vkctx;
  imgui_context imgui {vkctx};

  while (!imgui.window_context().should_close()) {
    imgui.begin_frame();
    {
      static bool show_demo_window = true;
      ImGui::ShowDemoWindow(&show_demo_window);
    }
    imgui.end_frame();
    imgui.render();
  }
}