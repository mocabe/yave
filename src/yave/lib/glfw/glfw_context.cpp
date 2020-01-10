//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/lib/glfw/glfw_context.hpp>
#include <yave/support/log.hpp>

#include <map>

YAVE_DECL_G_LOGGER(glfw)

namespace yave::glfw {

  /// Window data stored in user pointer
  class glfw_window_data
  {
  public:
    /// Ctor
    glfw_window_data();

  public:
    /// Add new data pointer
    [[nodiscard]] bool add(const std::string& key, void* data) noexcept;
    /// Find data pointer
    [[nodiscard]] void* find(const std::string& key) const noexcept;
    /// Remove data pointer
    void remove(const std::string& key) noexcept;

  private:
    /// map of user data
    std::map<std::string, void*> m_map;
  };

  glfw_window_data::glfw_window_data()
  {
  }

  bool glfw_window_data::add(const std::string& key, void* data) noexcept
  {
    auto r = m_map.emplace(key, data);

    if (!r.second) {
      Error(g_logger, "Failed to set window data: key {} already exists", key);
      return false;
    }

    Info(g_logger, "Set new window data: key={}", key);

    return true;
  }

  void* glfw_window_data::find(const std::string& key) const noexcept
  {
    auto iter = m_map.find(key);

    if (iter == m_map.end())
      return nullptr;

    return iter->second;
  }

  void glfw_window_data::remove(const std::string& key) noexcept
  {
    m_map.erase(key);
  }

  glfw_window::glfw_window(GLFWwindow* window)
    : m_window {window}
  {
    assert(!glfwGetWindowUserPointer(m_window));

    // set user pointer.
    // should delete window data in destructor.
    glfwSetWindowUserPointer(m_window, new glfw_window_data());

    Info(g_logger, "Created user data for window");
  }

  glfw_window::glfw_window(glfw_window&& other) noexcept
    : m_window {}
  {
    std::swap(m_window, other.m_window);
  }

  glfw_window::~glfw_window() noexcept
  {
    if (!m_window)
      return;

    // unset user pointer
    glfw_window_data* user_data =
      (glfw_window_data*)glfwGetWindowUserPointer(m_window);
    glfwSetWindowUserPointer(m_window, nullptr);
    // delete user data
    delete user_data;
    // destroy window handle
    glfwDestroyWindow(m_window);
  }

  auto glfw_window::get() const -> GLFWwindow*
  {
    return m_window;
  }

  bool glfw_window::add_user_data(
    GLFWwindow* window,
    const std::string& key,
    void* data) noexcept
  {
    if (!window)
      return false;

    auto* wd = (glfw_window_data*)glfwGetWindowUserPointer(window);

    assert(wd);

    return wd->add(key, data);
  }

  void* glfw_window::get_user_data(
    GLFWwindow* window,
    const std::string& key) noexcept
  {
    if (!window)
      return nullptr;

    auto* wd = (glfw_window_data*)glfwGetWindowUserPointer(window);

    assert(wd);

    return wd->find(key);
  }

  void glfw_window::remove_user_data(
    GLFWwindow* window,
    const std::string& key) noexcept
  {
    if (!window)
      return;

    auto* wd = (glfw_window_data*)glfwGetWindowUserPointer(window);

    assert(wd);

    wd->remove(key);
  }

  auto glfw_window::refresh_rate() const -> uint32_t
  {
    auto monitor = glfwGetWindowMonitor(m_window);

    // windowed mode
    if (!monitor)
      monitor = glfwGetPrimaryMonitor();

    auto mode = glfwGetVideoMode(monitor);
    return mode->refreshRate;
  }

  glfw_context::glfw_context()
  {
    init_logger();

    glfwSetErrorCallback([](int error, const char* msg) {
      Error(g_logger, "{}: {}", error, msg);
    });

    if (!glfwInit())
      throw std::runtime_error("Failed to initialize GLFW");

    Info(g_logger, "Initialized GLFW");
  }

  glfw_context::~glfw_context() noexcept
  {
    glfwTerminate();
    Info(g_logger, "Terminated GLFW");
  }

  auto glfw_context::create_window(
    uint32_t width,
    uint32_t height,
    const char* title) const -> glfw_window
  {
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    // enable transparent framebuffer
    glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);

    // create new window
    auto window =
      glfw_window(glfwCreateWindow(width, height, title, nullptr, nullptr));

    Info(g_logger, "Created new window: {}({}*{})", title, width, height);

    return window;
  }

  void glfw_context::poll_events() const
  {
    glfwPollEvents();
  }

  void glfw_context::wait_events() const
  {
    glfwWaitEvents();
  }

} // namespace yave::glfw