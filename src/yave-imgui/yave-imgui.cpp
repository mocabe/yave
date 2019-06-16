//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave-imgui/imgui_glfw_vulkan.hpp>
#include <imgui.h>

struct yave_imgui : yave::imgui_glfw_vulkan
{
  using yave::imgui_glfw_vulkan::imgui_glfw_vulkan;

  virtual void draw() override
  {
    yave::imgui_glfw_vulkan::draw();
  }
};

int main()
{
  yave_imgui app(true);
  app.exec();
}