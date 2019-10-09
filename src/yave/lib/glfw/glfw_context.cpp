//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/lib/glfw/glfw_context.hpp>
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
    uint32_t m_magic = 0xdeadbeef;
    /// flat map of (key,pointer) pairs
    std::vector<std::pair<std::string, void*>> m_map;
  };

  bool glfw_window_data::add(const std::string& key, void* data) noexcept
  {
    assert(m_magic == 0xdeadbeef);

    auto lb =
      std::lower_bound(m_map.begin(), m_map.end(), key, [](auto& l, auto& r) {
        return l.first < r;
      });

    if (lb != m_map.end() && lb->first == key) {
      Error(
        g_glfw_logger, "Failed to set window data: key {} already exists", key);
      return false;
    }

    m_map.emplace(lb, key, data);

    Info(g_glfw_logger, "Set new window data: key={}", key);

    return true;
  }

  void* glfw_window_data::find(const std::string& key) const noexcept
  {
    assert(m_magic == 0xdeadbeef);

    auto lb =
      std::lower_bound(m_map.begin(), m_map.end(), key, [](auto& l, auto& r) {
        return l.first < r;
      });

    if (lb != m_map.end() && lb->first == key) {
      return lb->second;
    }

    return nullptr;
  }

  void glfw_window_data::remove(const std::string& key) noexcept
  {
    assert(m_magic == 0xdeadbeef);

    auto lb =
      std::lower_bound(m_map.begin(), m_map.end(), key, [](auto& l, auto& r) {
        return l.first < r;
      });

    if (lb != m_map.end() && lb->first == key) {
      m_map.erase(lb);
    }
  }

  glfw_window::glfw_window(GLFWwindow* window)
    : m_window {window}
  {
    assert(!glfwGetWindowUserPointer(m_window));

    // set user pointer.
    // should delete window data in destructor.
    glfwSetWindowUserPointer(m_window, new glfw_window_data());

    Info(g_glfw_logger, "Created user data for window");
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

  auto glfw_window::add_user_data(
    GLFWwindow* window,
    const std::string& key,
    void* data) noexcept -> bool
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

  auto glfw_context::create_window(
    uint32_t width,
    uint32_t height,
    const char* title) const -> glfw_window
  {
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    // create new window
    auto window =
      glfw_window(glfwCreateWindow(width, height, title, nullptr, nullptr));

    Info(g_glfw_logger, "Created new window: {}({}*{})", title, width, height);

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

  glfw_window_data::glfw_window_data()
  {
  }

} // namespace yave::glfw