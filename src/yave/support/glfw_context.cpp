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

} // namespace

namespace yave {

  void glfw_window_deleter::operator()(GLFWwindow* window) noexcept
  {
    // delete user data
    glfw_window_data* user_data =
      (glfw_window_data*)glfwGetWindowUserPointer(window);
    glfwSetWindowUserPointer(window, nullptr);
    delete user_data;
    Info(g_glfw_logger, "Deleted window user data");

    // destroy window handle
    glfwDestroyWindow(window);
  }

  glfw_context::glfw_context()
  {
    init_glfw_logger();

    glfwSetErrorCallback([](int error, const char* msg) {
      using namespace yave;
      (void)error;
      Error(g_glfw_logger, "{}", msg);
    });

    if (!glfwInit())
      throw std::runtime_error("Failed to initialize GLFW");

    Info(g_glfw_logger, "Initialized GLFW");
  }

  glfw_context::~glfw_context() noexcept
  {
    glfwTerminate();
    Info(g_glfw_logger, "Terminated GLFW");
  }

  unique_glfw_window glfw_context::create_window(
    uint32_t width,
    uint32_t height,
    const char* title) const
  {
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    // create new window
    auto window = std::unique_ptr<GLFWwindow, glfw_window_deleter>(
      glfwCreateWindow(width, height, title, nullptr, nullptr));

    assert(!glfwGetWindowUserPointer(window.get()));

    // set user pointer.
    // should delete window data in destructor.
    glfwSetWindowUserPointer(window.get(), new glfw_window_data());
    Info(g_glfw_logger, "Created user data for window");

    Info(g_glfw_logger, "Created new window: {}({}*{})", title, width, height);

    return window;
  }

  void glfw_context::poll_events() const
  {
    glfwPollEvents();
  }

  glfw_window_data::glfw_window_data()
  {
  }

  bool glfw_window_data::add(const std::string& key, void* data)
  {
    using namespace yave;
    auto b = m_map.emplace(key, data).second;
    if (b)
      Info(g_glfw_logger, "Set new window data: key={}", key);
    else
      Warning(g_glfw_logger, "Failed to set window data: key={}", key);
    return b;
  }

  void* glfw_window_data::find(const std::string& str) const
  {
    auto it = m_map.find(str);

    if (it == m_map.end())
      return nullptr;
    else
      return it->second;
  }

} // namespace yave