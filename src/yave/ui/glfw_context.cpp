//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/ui/glfw_context.hpp>
#include <yave/ui/view_context.hpp>
#include <yave/support/log.hpp>

YAVE_DECL_LOCAL_LOGGER(ui::glfw_context)

namespace yave::ui {

  void glfw_context::ensure_on_main()
  {
    assert(m_tid == std::this_thread::get_id());
  }

  glfw_context::glfw_context(main_context& mctx)
    : m_mctx {mctx}
  {
    glfwSetErrorCallback([](int ec, const char* msg) {
      log_error("GLFW Error({}): {}", ec, msg);
    });

    if (!glfwInit())
      throw std::runtime_error("Failed to initialize GLFW");

    if (!glfwVulkanSupported())
      throw std::runtime_error("Vulkan is not available");

    m_tid = std::this_thread::get_id();
  }

  glfw_context::~glfw_context() noexcept
  {
    glfwTerminate();
  }

  auto glfw_context::main_ctx() -> main_context&
  {
    return m_mctx;
  }

  void glfw_context::wake()
  {
    glfwPostEmptyEvent();
  }

  void glfw_context::wait()
  {
    glfwWaitEvents();
  }

  auto glfw_context::get_window_data(GLFWwindow* w) -> window_data&
  {
    assert(w);

    auto up = glfwGetWindowUserPointer(w);
    assert(up);

    return *(window_data*)up;
  }

  auto glfw_context::create_window(std::u8string_view u8title, ui::size size)
    -> GLFWwindow*
  {
    ensure_on_main();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

    auto width  = static_cast<int>(size.w);
    auto height = static_cast<int>(size.h);
    auto title  = reinterpret_cast<const char*>(u8title.data());

    auto w = glfwCreateWindow(width, height, title, nullptr, nullptr);

    log_info("New window: '{}'({}x{})", title, width, height);

    if (!w) {
      log_error("Failed to create window!");
      throw std::runtime_error("Failed to create new window");
    }

    glfwSetWindowUserPointer(
      w, new window_data {.view_ctx = m_mctx.view_ctx()});

    // ------------------------------------------
    // Window Callbacks

    glfwSetWindowPosCallback(w, [](GLFWwindow* w, int xpos, int ypos) {
      get_window_data(w).view_ctx.post_window_pos_event(w, xpos, ypos);
    });

    glfwSetWindowSizeCallback(w, [](GLFWwindow* w, int width, int height) {
      get_window_data(w).view_ctx.post_window_size_event(w, width, height);
    });

    glfwSetWindowCloseCallback(w, [](GLFWwindow* w) {
      get_window_data(w).view_ctx.post_window_close_event(w);
    });

    glfwSetWindowRefreshCallback(w, [](GLFWwindow* w) {
      get_window_data(w).view_ctx.post_window_refresh_event(w);
    });

    glfwSetWindowFocusCallback(w, [](GLFWwindow* w, int focused) {
      get_window_data(w).view_ctx.post_window_focus_event(w, focused);
    });

    glfwSetWindowIconifyCallback(w, [](GLFWwindow* w, int iconified) {
      get_window_data(w).view_ctx.post_window_minimize_event(w, iconified);
    });

    glfwSetWindowMaximizeCallback(w, [](GLFWwindow* w, int maximized) {
      get_window_data(w).view_ctx.post_window_maximize_event(w, maximized);
    });

    glfwSetFramebufferSizeCallback(w, [](GLFWwindow* w, int width, int height) {
      get_window_data(w).view_ctx.post_window_framebuffer_size_event(
        w, width, height);
    });

    glfwSetWindowContentScaleCallback(
      w, [](GLFWwindow* w, float xscale, float yscale) {
        get_window_data(w).view_ctx.post_window_content_scale_event(
          w, xscale, yscale);
      });

    // ------------------------------------------
    // Input Callbacks

    glfwSetKeyCallback(
      w, [](GLFWwindow* w, int key, int scancode, int action, int mods) {
        auto k = static_cast<ui::key>(key);
        auto a = static_cast<ui::key_action>(action);
        auto m = static_cast<ui::key_modifier_flags>(mods);
        get_window_data(w).view_ctx.post_key_event(w, k, a, m);
      });

    glfwSetCharCallback(w, [](GLFWwindow* w, unsigned int codepoint) {
      get_window_data(w).view_ctx.post_char_event(w, codepoint);
    });

    glfwSetMouseButtonCallback(
      w, [](GLFWwindow* w, int button, int action, int mods) {
        auto k = static_cast<ui::mouse_button>(button);
        auto a = static_cast<ui::mouse_button_action>(action);
        auto m = static_cast<ui::key_modifier_flags>(mods);
        get_window_data(w).view_ctx.post_mouse_event(w, k, a, m);
      });

    glfwSetCursorPosCallback(w, [](GLFWwindow* w, double xpos, double ypos) {
      get_window_data(w).view_ctx.post_cursor_pos_event(w, xpos, ypos);
    });

    glfwSetCursorEnterCallback(w, [](GLFWwindow* w, int entered) {
      get_window_data(w).view_ctx.post_cursor_enter_event(w, entered);
    });

    glfwSetScrollCallback(w, [](GLFWwindow* w, double xoffset, double yoffset) {
      get_window_data(w).view_ctx.post_scroll_event(w, xoffset, yoffset);
    });

    glfwSetDropCallback(
      w, [](GLFWwindow* w, int path_count, const char* paths[]) {
        auto ps = std::vector<std::u8string>();
        for (auto i = 0; i < path_count; ++i) {
          ps.emplace_back((const char8_t*)paths[i]);
        }
        get_window_data(w).view_ctx.post_path_drop_event(w, std::move(ps));
      });

    return w;
  }

  void glfw_context::destroy_window(GLFWwindow* w)
  {
    ensure_on_main();
    assert(w);

    // delete user data
    auto data = (window_data*)glfwGetWindowUserPointer(w);
    glfwSetWindowUserPointer(w, nullptr);
    delete data;

    glfwDestroyWindow(w);
  }

  void glfw_context::show_window(GLFWwindow* w)
  {
    ensure_on_main();
    glfwShowWindow(w);
  }

  void glfw_context::hide_window(GLFWwindow* w)
  {
    ensure_on_main();
    glfwHideWindow(w);
  }

  void glfw_context::rename_window(GLFWwindow* w, std::u8string_view name)
  {
    ensure_on_main();
    glfwSetWindowTitle(w, (const char*)name.data());
  }

  void glfw_context::resize_window(GLFWwindow* w, ui::size size)
  {
    ensure_on_main();
    glfwSetWindowSize(w, static_cast<int>(size.w), static_cast<int>(size.h));
  }

  void glfw_context::move_window(GLFWwindow* w, ui::vec pos)
  {
    ensure_on_main();
    glfwSetWindowPos(w, static_cast<int>(pos.x), static_cast<int>(pos.y));
  }

  auto glfw_context::window_size(GLFWwindow* win) -> ui::size
  {
    ensure_on_main();
    int w, h;
    glfwGetWindowSize(win, &w, &h);
    return ui::size(w, h);
  }

  auto glfw_context::window_pos(GLFWwindow* win) -> ui::vec
  {
    ensure_on_main();
    int x, y;
    glfwGetWindowPos(win, &x, &y);
    return ui::vec(x, y);
  }

  auto glfw_context::window_content_scale(GLFWwindow* win) -> ui::vec
  {
    ensure_on_main();
    float xs, ys;
    glfwGetWindowContentScale(win, &xs, &ys);
    return ui::vec(xs, ys);
  }

  void glfw_context::focus_window(GLFWwindow* win)
  {
    ensure_on_main();
    glfwFocusWindow(win);
  }

  void glfw_context::request_window_attention(GLFWwindow* win)
  {
    ensure_on_main();
    glfwRequestWindowAttention(win);
  }

  void glfw_context::set_window_max_size(GLFWwindow* win, ui::size size)
  {
    ensure_on_main();
    glfwSetWindowSizeLimits(
      win,
      GLFW_DONT_CARE,
      GLFW_DONT_CARE,
      static_cast<int>(size.w),
      static_cast<int>(size.h));
  }

  void glfw_context::set_window_min_size(GLFWwindow* win, ui::size size)
  {
    ensure_on_main();
    glfwSetWindowSizeLimits(
      win,
      static_cast<int>(size.w),
      static_cast<int>(size.h),
      GLFW_DONT_CARE,
      GLFW_DONT_CARE);
  }
}