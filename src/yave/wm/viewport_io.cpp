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
#include <iostream>

namespace yave::wm {

  namespace {

    struct button_state_data
    {
      int state                = GLFW_RELEASE;
      std::optional<int> event = std::nullopt;
    };

    struct key_state_data
    {
      int state                = GLFW_RELEASE;
      int modifiers            = 0;
      std::optional<int> event = std::nullopt;
    };

    struct key_event_data
    {
      int action    = 0;
      int modifiers = 0;
    };

  } // namespace

  class viewport_io::impl
  {
  public:
    /// window handle
    glfw::glfw_window& glfw_win;

  public:
    // current IO states
    std::array<button_state_data, GLFW_MOUSE_BUTTON_LAST> button_states = {};
    std::array<key_state_data, GLFW_KEY_LAST> key_states                = {};
    // IO event queue
    std::array<std::vector<int>, GLFW_MOUSE_BUTTON_LAST> button_events = {};
    std::array<std::vector<key_event_data>, GLFW_KEY_LAST> key_events  = {};
    // cursor
    glm::vec2 cursor_pos;
    glm::vec2 cursor_delta;
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
      GLFWwindow* window,
      int button,
      int action,
      int /*mods*/)
    {
      auto _this = get_this(window);

      switch (action) {
        case GLFW_RELEASE:
        case GLFW_PRESS:
          _this->button_events[button].push_back(action);
          return;
      }
      unreachable();
    }

    /// cb
    static void glfw_key_callback(
      GLFWwindow* window,
      int key,
      int /*scancode*/,
      int action,
      int mods)
    {
      auto _this = get_this(window);

      switch (action) {
        case GLFW_RELEASE:
        case GLFW_PRESS:
        case GLFW_REPEAT:
          _this->key_events[key].push_back(
            {.action = action, .modifiers = mods});
          return;
      }
      unreachable();
    }

    /// cb
    static void glfw_char_callback(GLFWwindow* window, unsigned int codepoint)
    {
      auto _this = get_this(window);
      char32_t c;
      std::memcpy(&c, &codepoint, sizeof(char32_t));
      _this->char_buff.push_back(c);
    }

    void mouse_input_begin()
    {
      // update states
      for (int b = 0; b < std::ssize(button_states); ++b) {
        auto& events = button_events[b];
        auto& state  = button_states[b];

        if (!events.empty()) {
          // pop event
          auto e = events.front();
          events.erase(events.begin());
          // set event in this frame
          state.event = e;
          // update state
          if (state.state != e) {
            state.state = e;
          }
        } else
          // no event to process
          state.event = std::nullopt;
      }
      // update cursor when viewport is hovered. this works even when window is
      // not focused.
      double x, y;
      glfwGetCursorPos(glfw_win.get(), &x, &y);
      // convert to virtual viewport coordinate
      auto pos     = glm::vec2(x, y) + glm::vec2(glfw_win.pos());
      cursor_delta = pos - cursor_pos;
      cursor_pos   = pos;
    }

    void key_input_begin()
    {
      // update key state
      for (int k = GLFW_KEY_SPACE; k < std::ssize(key_states); ++k) {
        auto& events = key_events[k];
        auto& state  = key_states[k];

        if (!events.empty()) {
          // pop event
          auto e = events.front();
          events.erase(events.begin());
          // set event
          state.event     = e.action;
          state.modifiers = e.modifiers;
          // update state (ignore auto repeat)
          if (e.action != GLFW_REPEAT && state.state != e.action) {
            state.state = e.action;
          }
        } else
          // no event to process
          state.event = std::nullopt;
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
    }

    ~impl() noexcept
    {
      deinit_user_data();
    }

    void update()
    {
      glfw_win.glfw_ctx().poll_events();
      mouse_input_begin();
      key_input_begin();
    }

    auto get_event(wm::key k) const -> std::optional<wm::key_event>
    {
      auto& s = key_states[static_cast<int>(k)];

      if (!s.event.has_value())
        return std::nullopt;

      switch (auto e = *s.event) {
        case GLFW_RELEASE:
        case GLFW_PRESS:
        case GLFW_REPEAT:
          return static_cast<wm::key_event>(e);
      }
      unreachable();
    }

    auto get_state(wm::key k) const -> wm::key_state
    {
      auto& s = key_states[static_cast<int>(k)];

      switch (auto ss = s.state) {
        case GLFW_RELEASE:
        case GLFW_PRESS:
          return static_cast<wm::key_state>(ss);
      }
      unreachable();
    }

    auto get_modifiers(wm::key k) const -> wm::key_modifier_flags
    {
      auto& s = key_states[static_cast<int>(k)];
      return static_cast<wm::key_modifier_flags>(s.modifiers);
    }

    auto get_event(mouse_button b) const
      -> std::optional<wm::mouse_button_event>
    {
      auto& s = button_states[static_cast<int>(b)];

      if (!s.event.has_value())
        return std::nullopt;

      switch (auto e = *s.event) {
        case GLFW_RELEASE:
        case GLFW_PRESS:
          return static_cast<wm::mouse_button_event>(e);
      }
      unreachable();
    }

    auto get_state(wm::mouse_button b) const -> wm::mouse_button_state
    {
      auto& s = button_states[static_cast<int>(b)];

      switch (auto ss = s.state) {
        case GLFW_PRESS:
        case GLFW_RELEASE:
          return static_cast<wm::mouse_button_state>(ss);
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

  auto viewport_io::mouse_button_state(wm::mouse_button b) const
    -> wm::mouse_button_state
  {
    return m_pimpl->get_state(b);
  }

  auto viewport_io::mouse_button_event(wm::mouse_button b) const
    -> std::optional<wm::mouse_button_event>
  {
    return m_pimpl->get_event(b);
  }

  auto viewport_io::key_state(wm::key k) const -> wm::key_state
  {
    return m_pimpl->get_state(k);
  }

  auto viewport_io::key_modifiers(wm::key k) const -> wm::key_modifier_flags
  {
    return m_pimpl->get_modifiers(k);
  }

  auto viewport_io::key_event(wm::key k) const -> std::optional<wm::key_event>
  {
    return m_pimpl->get_event(k);
  }

  auto viewport_io::key_text() const -> std::u8string
  {
    return m_pimpl->text;
  }

  auto viewport_io::key_name(wm::key k) const -> std::u8string
  {
    return get_key_name(m_pimpl->glfw_win.glfw_ctx(), k);
  }

  auto viewport_io::mouse_pos() const -> glm::vec2
  {
    return m_pimpl->cursor_pos;
  }

  auto viewport_io::mouse_delta() const -> glm::vec2
  {
    return m_pimpl->cursor_delta;
  }

} // namespace yave::wm