//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/support/glfw_context.hpp>
#include <yave/support/log.hpp>

namespace {

  // logger
  std::shared_ptr<spdlog::logger> g_glfw_logger;

  /// init
  void init_glfw_logger()
  {
    [[maybe_unused]] static auto init_logger = [] {
      g_glfw_logger = yave::add_logger("glfw");
      return 1;
    }();
  }

  /// GLFW erro callback
  void glfwErrorCallback(int error, const char* msg)
  {
    using namespace yave;
    (void)error;
    Error(g_glfw_logger, "{}", msg);
  }

} // namespace

namespace yave {

  void glfw_window_deleter::operator()(GLFWwindow* window)
  {
    glfwDestroyWindow(window);
  }

  glfw_context::glfw_context()
  {
    init_glfw_logger();

    glfwSetErrorCallback(glfwErrorCallback);

    if (!glfwInit())
      throw std::runtime_error("Failed to initialize GLFW");

    Info(g_glfw_logger, "Initialized GLFW");
  }

  glfw_context::~glfw_context() noexcept
  {
    glfwTerminate();
    Info(g_glfw_logger, "Terminated GLFW");
  }

  std::unique_ptr<GLFWwindow, glfw_window_deleter> glfw_context::create_window(
    uint32_t width,
    uint32_t height,
    const char* title) const
  {
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    auto window = std::unique_ptr<GLFWwindow, glfw_window_deleter>(
      glfwCreateWindow(width, height, title, nullptr, nullptr));

    Info(g_glfw_logger, "Created new window: {}({}*{})", title, width, height);

    return window;
  }
}