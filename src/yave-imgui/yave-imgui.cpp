
#include <yave-imgui/imgui_glfw_vulkan.hpp>
#include <imgui.h>

struct yave_imgui : yave::imgui_glfw_vulkan
{
  using yave::imgui_glfw_vulkan::imgui_glfw_vulkan;

  virtual void user_code() override
  {
    ImGui::Begin("test");
    double x, y;
    glfwGetCursorPos(m_window.get(), &x, &y);
    ImGui::Text("glfwCursorPos: %lf, %lf", x, y);
    ImGui::End();
  }
};

int main()
{
  yave_imgui app(true);
  app.draw();
}