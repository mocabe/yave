//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/wm/viewport_io.hpp>

#include <functional>
#include <chrono>
#include <cstring>
#include <codecvt>
#include <locale>

namespace yave::wm {

  class viewport_io::impl
  {
  public:
    /// window handle
    glfw::glfw_window& glfw_win;

  public:
    // current IO states
    std::array<char, GLFW_MOUSE_BUTTON_LAST> button_state = {};
    std::array<char, GLFW_KEY_LAST> key_state             = {};
    // cursor
    fvec2 cursor_pos;
    fvec2 cursor_delta;
    // text input
    std::u32string char_buff;
    std::u8string text;

    /// name of custom data for window
    static constexpr const char* user_data_name = "wm::viewport_io";

    /// get pimpl from window custon data
    static auto get_this(GLFWwindow* window)
    {
      auto ret =
        (impl*)glfw::glfw_window::get_user_data(window, user_data_name);
      assert(ret);
      return ret;
    }

    /// cb
    static void glfw_mouse_button_callback(
      GLFWwindow* /*window*/,
      int /*button*/,
      int /*action*/,
      int /*mods*/)
    {
    }

    /// cb
    static void glfw_key_callback(
      GLFWwindow* /*window*/,
      int /*key*/,
      int /*scancode*/,
      int /*action*/,
      int /*mods*/)
    {
    }

    /// cb
    static void glfw_char_callback(GLFWwindow* window, unsigned int codepoint)
    {
      auto _this = get_this(window);
      char32_t c;
      std::memcpy(&c, &codepoint, sizeof(char32_t));
      _this->char_buff.push_back(c);
    }

    void update_mouse_input()
    {
      // update buttons
      for (int b = 0; b < std::ssize(button_state); ++b) {
        // update state
        button_state[b] = glfwGetMouseButton(glfw_win.get(), b);
      }
      // update cursor when viewport is hovered. this works even when window is
      // not focused.
      double x, y;
      glfwGetCursorPos(glfw_win.get(), &x, &y);
      // convert to virtual viewport coordinate
      auto pos     = fvec2(x, y) + fvec2(glfw_win.pos());
      cursor_delta = pos - cursor_pos;
      cursor_pos   = pos;
    }

    void update_key_input()
    {
      for (int k = GLFW_KEY_SPACE; k < std::ssize(key_state); ++k) {
        // update state
        key_state[k] = glfwGetKey(glfw_win.get(), k);
      }

      // handle text input
      {
        std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> cvt;
        auto tmp = cvt.to_bytes(char_buff);
        text     = std::u8string((const char8_t*)tmp.data(), tmp.length());
        char_buff.clear();
      }
    }

    void init_user_data()
    {
      if (!glfw::glfw_window::add_user_data(
            glfw_win.get(), user_data_name, this))
        throw std::runtime_error("Failed to init window user data");

      glfwSetKeyCallback(glfw_win.get(), glfw_key_callback);
      glfwSetCharCallback(glfw_win.get(), glfw_char_callback);
      glfwSetMouseButtonCallback(glfw_win.get(), glfw_mouse_button_callback);
    }

    void deinit_user_data()
    {
      glfwSetMouseButtonCallback(glfw_win.get(), nullptr);
      glfwSetCharCallback(glfw_win.get(), nullptr);
      glfwSetKeyCallback(glfw_win.get(), nullptr);
      glfw::glfw_window::remove_user_data(glfw_win.get(), user_data_name);
    }

  public:
    impl(glfw::glfw_window& win)
      : glfw_win {win}
    {
      init_user_data();

      // enable sticky mode
      glfwSetInputMode(glfw_win.get(), GLFW_STICKY_KEYS, GLFW_TRUE);
      glfwSetInputMode(glfw_win.get(), GLFW_STICKY_MOUSE_BUTTONS, GLFW_TRUE);
    }

    ~impl() noexcept
    {
      deinit_user_data();
    }

    void update()
    {
      glfw_win.glfw_ctx().poll_events();
      update_mouse_input();
      update_key_input();
    }

    auto get_state(key k)
    {
      switch (key_state[static_cast<int>(k)]) {
        case GLFW_RELEASE:
          return key_state::up;
        case GLFW_PRESS:
          return key_state::down;
      }
      unreachable();
    }

    auto get_state(mouse_button b)
    {
      switch (button_state[static_cast<int>(b)]) {
        case GLFW_RELEASE:
          return mouse_button_state::up;
        case GLFW_PRESS:
          return mouse_button_state::down;
      }
      unreachable();
    }
  };

  viewport_io::viewport_io(glfw::glfw_window& win)
    : m_pimpl {std::make_unique<impl>(win)}
  {
  }

  viewport_io::~viewport_io() noexcept = default;

  auto viewport_io::window() -> glfw::glfw_window&
  {
    return m_pimpl->glfw_win;
  }

  void viewport_io::update()
  {
    m_pimpl->update();
  }

  auto viewport_io::get_key_state(wm::key k) const -> key_state
  {
    return m_pimpl->get_state(k);
  }

  auto viewport_io::get_mouse_state(wm::mouse_button b) const
    -> mouse_button_state
  {
    return m_pimpl->get_state(b);
  }

  auto viewport_io::get_text_input() const -> std::u8string
  {
    return m_pimpl->text;
  }

  auto viewport_io::mouse_pos() const -> fvec2
  {
    return m_pimpl->cursor_pos;
  }

  auto viewport_io::mouse_delta() const -> fvec2
  {
    return m_pimpl->cursor_delta;
  }

} // namespace yave::wm