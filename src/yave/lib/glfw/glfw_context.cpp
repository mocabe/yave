//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/lib/glfw/glfw_context.hpp>
#include <yave/core/log.hpp>

#include <map>

YAVE_DECL_LOCAL_LOGGER(glfw)

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
      log_error("Failed to set window data: key {} already exists", key);
      return false;
    }

    log_info("Set new window data: key={}", key);
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

  glfw_window::glfw_window(
    glfw_context& ctx,
    std::string title,
    uint32_t width,
    uint32_t height)
    : m_ctx {ctx}
    , m_window {glfwCreateWindow(
        width,
        height,
        title.c_str(),
        nullptr,
        nullptr)}
    , m_title {title}
  {
    assert(!glfwGetWindowUserPointer(m_window));

    // set user pointer.
    // should delete window data in destructor.
    glfwSetWindowUserPointer(m_window, new glfw_window_data());

    log_info("Created user data for window");
  }

  glfw_window::glfw_window(glfw_window&& other) noexcept
    : m_ctx {other.m_ctx}
    , m_window {std::exchange(other.m_window, nullptr)}
    , m_title {std::exchange(other.m_title, "")}
  {
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

  auto glfw_window::glfw_ctx() -> glfw::glfw_context&
  {
    return m_ctx;
  }

  auto glfw_window::glfw_ctx() const -> const glfw::glfw_context&
  {
    return m_ctx;
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

  auto glfw_window::pos() const -> glm::u32vec2
  {
    int x, y;
    glfwGetWindowPos(m_window, &x, &y);
    return {x, y};
  }

  auto glfw_window::size() const -> glm::u32vec2
  {
    int w, h;
    glfwGetWindowSize(m_window, &w, &h);
    return {w, h};
  }

  auto glfw_window::buffer_size() const -> glm::u32vec2
  {
    int width, height;
    glfwGetFramebufferSize(m_window, &width, &height);
    return {width, height};
  }

  auto glfw_window::title() const -> std::string
  {
    return m_title;
  }

  bool glfw_window::should_close() const
  {
    return glfwWindowShouldClose(m_window);
  }

  bool glfw_window::focused() const
  {
    return glfwGetWindowAttrib(m_window, GLFW_FOCUSED);
  }

  bool glfw_window::hovered() const
  {
    return glfwGetWindowAttrib(m_window, GLFW_HOVERED);
  }

  glfw_context::glfw_context(create_info info)
  {
    glfwSetErrorCallback(
      [](int error, const char* msg) { log_error("{}: {}", error, msg); });

    if (!glfwInit())
      throw std::runtime_error("Failed to initialize GLFW");

    log_info("Initialized GLFW");
  }

  glfw_context::~glfw_context() noexcept
  {
    glfwTerminate();
    log_info("Terminated GLFW");
  }

  auto glfw_context::create_window(
    uint32_t width,
    uint32_t height,
    const char* title) -> glfw_window
  {
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    // create new window
    auto window = glfw_window(*this, title, width, height);

    log_info("Created new window: {}({}*{})", title, width, height);

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