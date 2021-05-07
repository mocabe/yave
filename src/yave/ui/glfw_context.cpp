//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/ui/glfw_context.hpp>
#include <yave/ui/view_context.hpp>
#include <yave/support/log.hpp>

YAVE_DECL_LOCAL_LOGGER(ui::glfw_context)

namespace {

  bool glfw_bool(int b)
  {
    return b == GLFW_TRUE;
  }

} // namespace

namespace yave::ui {

  glfw_context::glfw_context(view_context& mctx)
    : m_vctx {mctx}
  {
    glfwSetErrorCallback([](int ec, const char* msg) {
      log_error("GLFW Error({}): {}", ec, msg);
    });

    if (glfwInit() != GLFW_TRUE)
      throw std::runtime_error("Failed to initialize GLFW");

    if (glfwVulkanSupported() != GLFW_TRUE)
      throw std::runtime_error("Vulkan is not available");
  }

  glfw_context::~glfw_context() noexcept
  {
    glfwTerminate();
  }

  auto glfw_context::view_ctx() -> view_context&
  {
    return m_vctx;
  }

  void glfw_context::wake()
  {
    glfwPostEmptyEvent();
  }

  void glfw_context::wait()
  {
    glfwWaitEvents();
  }

  void glfw_context::poll()
  {
    glfwPollEvents();
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

    glfwSetWindowUserPointer(w, new window_data {.view_ctx = m_vctx});

    // ------------------------------------------
    // Window Callbacks

    glfwSetWindowPosCallback(w, [](GLFWwindow* w, int xpos, int ypos) {
      auto& vctx = get_window_data(w).view_ctx;
      auto& wm   = vctx.window_manager();
      wm.push_glfw_event(
        glfw_pos_event {.win = w, .x = u32(xpos), .y = u32(ypos)}, {});
      vctx.poll();
    });

    glfwSetWindowSizeCallback(w, [](GLFWwindow* w, int width, int height) {
      auto& vctx = get_window_data(w).view_ctx;
      auto& wm   = vctx.window_manager();
      wm.push_glfw_event(
        glfw_size_event {.win = w, .w = u32(width), .h = u32(height)}, {});
      vctx.poll();
    });

    glfwSetWindowCloseCallback(w, [](GLFWwindow* w) {
      auto& vctx = get_window_data(w).view_ctx;
      auto& wm   = vctx.window_manager();
      wm.push_glfw_event(glfw_close_event {.win = w}, {});
      vctx.poll();
    });

    glfwSetWindowRefreshCallback(w, [](GLFWwindow* w) {
      auto& vctx = get_window_data(w).view_ctx;
      auto& wm   = vctx.window_manager();
      wm.push_glfw_event(glfw_refresh_event {.win = w}, {});
      vctx.poll();
    });

    glfwSetWindowFocusCallback(w, [](GLFWwindow* w, int focused) {
      auto& vctx = get_window_data(w).view_ctx;
      auto& wm   = vctx.window_manager();
      wm.push_glfw_event(
        glfw_focus_event {.win = w, .focused = glfw_bool(focused)}, {});
      vctx.poll();
    });

    glfwSetWindowIconifyCallback(w, [](GLFWwindow* w, int iconified) {
      auto& vctx = get_window_data(w).view_ctx;
      auto& wm   = vctx.window_manager();
      wm.push_glfw_event(
        glfw_minimize_event {.win = w, .minimized = glfw_bool(iconified)}, {});
      vctx.poll();
    });

    glfwSetWindowMaximizeCallback(w, [](GLFWwindow* w, int maximized) {
      auto& vctx = get_window_data(w).view_ctx;
      auto& wm   = vctx.window_manager();
      wm.push_glfw_event(
        glfw_maximize_event {.win = w, .maximized = glfw_bool(maximized)}, {});
      vctx.poll();
    });

    glfwSetFramebufferSizeCallback(w, [](GLFWwindow* w, int width, int height) {
      auto& vctx = get_window_data(w).view_ctx;
      auto& wm   = vctx.window_manager();
      wm.push_glfw_event(
        glfw_framebuffer_size_event {
          .win = w, .w = u32(width), .h = u32(height)},
        {});
      vctx.poll();
    });

    glfwSetWindowContentScaleCallback(
      w, [](GLFWwindow* w, float xscale, float yscale) {
        auto& vctx = get_window_data(w).view_ctx;
        auto& wm   = vctx.window_manager();
        wm.push_glfw_event(
          glfw_content_scale_event {.win = w, .xs = xscale, .ys = yscale}, {});
        vctx.poll();
      });

    // ------------------------------------------
    // Input Callbacks

    glfwSetKeyCallback(
      w, [](GLFWwindow* w, int key, int /*scancode*/, int action, int mods) {
        auto k = static_cast<ui::key>(key);
        auto a = static_cast<ui::key_action>(action);
        auto m = static_cast<ui::key_modifier_flags>(mods);

        auto& vctx = get_window_data(w).view_ctx;
        auto& wm   = vctx.window_manager();
        wm.push_glfw_event(
          glfw_key_event {.win = w, .key = k, .action = a, .mods = m}, {});
        vctx.poll();
      });

    glfwSetCharCallback(w, [](GLFWwindow* w, unsigned int codepoint) {
      auto& vctx = get_window_data(w).view_ctx;
      auto& wm   = vctx.window_manager();
      wm.push_glfw_event(
        glfw_char_event {.win = w, .codepoint = codepoint}, {});
      vctx.poll();
    });

    glfwSetMouseButtonCallback(
      w, [](GLFWwindow* w, int button, int action, int mods) {
        auto b = static_cast<ui::mouse_button>(button);
        auto a = static_cast<ui::mouse_button_action>(action);
        auto m = static_cast<ui::key_modifier_flags>(mods);

        auto& vctx = get_window_data(w).view_ctx;
        auto& wm   = vctx.window_manager();
        wm.push_glfw_event(
          glfw_button_event {.win = w, .button = b, .action = a, .mods = m},
          {});
        vctx.poll();
      });

    glfwSetCursorPosCallback(w, [](GLFWwindow* w, double xpos, double ypos) {
      auto& vctx = get_window_data(w).view_ctx;
      auto& wm   = vctx.window_manager();
      wm.push_glfw_event(
        glfw_cursor_pos_event {.win = w, .xpos = xpos, .ypos = ypos}, {});
      vctx.poll();
    });

    glfwSetCursorEnterCallback(w, [](GLFWwindow* w, int entered) {
      auto& vctx = get_window_data(w).view_ctx;
      auto& wm   = vctx.window_manager();
      wm.push_glfw_event(
        glfw_cursor_enter_event {.win = w, .entered = glfw_bool(entered)}, {});
      vctx.poll();
    });

    glfwSetScrollCallback(w, [](GLFWwindow* w, double xoffset, double yoffset) {
      auto& vctx = get_window_data(w).view_ctx;
      auto& wm   = vctx.window_manager();
      wm.push_glfw_event(
        glfw_scroll_event {.win = w, .xoffset = xoffset, .yoffset = yoffset},
        {});
      vctx.poll();
    });

    glfwSetDropCallback(
      w, [](GLFWwindow* w, int path_count, const char* paths[]) {
        auto ps = std::vector<std::u8string>();
        for (auto i = 0; i < path_count; ++i) {
          ps.emplace_back((const char8_t*)paths[i]);
        }

        auto& vctx = get_window_data(w).view_ctx;
        auto& wm   = vctx.window_manager();
        wm.push_glfw_event(
          glfw_path_drop_event {.win = w, .paths = std::move(ps)}, {});
        vctx.poll();
      });

    return w;
  }

  void glfw_context::destroy_window(GLFWwindow* w)
  {
    assert(w);

    // delete user data
    auto data = (window_data*)glfwGetWindowUserPointer(w);
    glfwSetWindowUserPointer(w, nullptr);
    delete data;

    glfwDestroyWindow(w);
  }

  void glfw_context::show_window(GLFWwindow* w)
  {
    glfwShowWindow(w);
  }

  void glfw_context::hide_window(GLFWwindow* w)
  {
    glfwHideWindow(w);
  }

  void glfw_context::rename_window(GLFWwindow* w, std::u8string_view name)
  {
    glfwSetWindowTitle(w, (const char*)name.data());
  }

  void glfw_context::resize_window(GLFWwindow* w, ui::size size)
  {
    glfwSetWindowSize(w, static_cast<int>(size.w), static_cast<int>(size.h));
  }

  void glfw_context::move_window(GLFWwindow* w, ui::vec pos)
  {
    glfwSetWindowPos(w, static_cast<int>(pos.x), static_cast<int>(pos.y));
  }

  auto glfw_context::window_size(GLFWwindow* win) -> ui::size
  {
    int w, h;
    glfwGetWindowSize(win, &w, &h);
    return ui::size(w, h);
  }

  auto glfw_context::window_pos(GLFWwindow* win) -> ui::vec
  {
    int x, y;
    glfwGetWindowPos(win, &x, &y);
    return ui::vec(x, y);
  }

  auto glfw_context::window_content_scale(GLFWwindow* win) -> ui::vec
  {
    float xs, ys;
    glfwGetWindowContentScale(win, &xs, &ys);
    return ui::vec(xs, ys);
  }

  void glfw_context::focus_window(GLFWwindow* win)
  {
    glfwFocusWindow(win);
  }

  void glfw_context::request_window_attention(GLFWwindow* win)
  {
    glfwRequestWindowAttention(win);
  }

  void glfw_context::set_window_max_size(GLFWwindow* win, ui::size size)
  {
    glfwSetWindowSizeLimits(
      win,
      GLFW_DONT_CARE,
      GLFW_DONT_CARE,
      static_cast<int>(size.w),
      static_cast<int>(size.h));
  }

  void glfw_context::set_window_min_size(GLFWwindow* win, ui::size size)
  {
    glfwSetWindowSizeLimits(
      win,
      static_cast<int>(size.w),
      static_cast<int>(size.h),
      GLFW_DONT_CARE,
      GLFW_DONT_CARE);
  }
} // namespace yave::ui