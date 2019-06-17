//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave-imgui/imgui_glfw_vulkan.hpp>

#include <yave-imgui/vulkan_util.hpp>

#include <imgui.h>
#include <selene/img_io/IO.hpp>
#include <selene/img/interop/DynImageToImage.hpp>
#include <selene/img_ops/ImageConversions.hpp>
#include <boost/dll/runtime_symbol_info.hpp>
#include <fmt/format.h>

using namespace yave;

int main()
{



  imgui_glfw_vulkan imgui(true);


  while (!imgui.window_context().should_close()) {
    imgui.begin();
  {
      static bool show_demo_window = true;
      ImGui::ShowMetricsWindow(&show_demo_window);



    }
    imgui.end();
    imgui.render();
  }
}