//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/wm/viewport_io.hpp>
#include <yave/wm/key_events.hpp>
#include <yave/wm/mouse_events.hpp>

#include <functional>
#include <chrono>

namespace yave::wm {

  namespace {

    /// key input map
    using key_map = std::array<
      char, // input state
      GLFW_KEY_LAST>;

    /// mouse input map
    using mouse_map = std::array<
      std::pair<
        char,                                   // input state
        std::chrono::system_clock::time_point>, // press timestamp
      GLFW_MOUSE_BUTTON_LAST>;

  } // namespace

  class viewport_io::impl
  {
    /// ctx
    glfw::glfw_window& glfw_win;
    /// keys
    key_map keys_prev;
    /// mouse
    mouse_map mouse_prev;
    /// pos
    fvec2 cursor_prev;

  public:
    impl(glfw::glfw_window& win)
      : glfw_win {win}
    {
      for (auto&& s : keys_prev)
        s = GLFW_RELEASE;

      for (auto&& [s, t] : mouse_prev) {
        s = GLFW_RELEASE;
        t = {};
      }

      double x, y;
      glfwGetCursorPos(glfw_win.get(), &x, &y);
      cursor_prev = fvec2(x, y);
    }

    auto query_key_input()
    {
      key_inputs ret;

      for (int k = GLFW_KEY_SPACE; k < GLFW_KEY_LAST; ++k) {

        auto next = glfwGetKey(glfw_win.get(), k);
        auto diff = next - keys_prev[k];

        if (diff == 0 && next == GLFW_PRESS)
          diff = GLFW_REPEAT;

        if (diff == GLFW_PRESS - GLFW_RELEASE)
          ret.press.push_back(static_cast<wm::key>(k));

        if (diff == GLFW_RELEASE - GLFW_PRESS)
          ret.release.push_back(static_cast<wm::key>(k));

        if (diff == GLFW_REPEAT)
          ret.repeat.push_back(static_cast<wm::key>(k));

        keys_prev[k] = next;
      }

      return ret;
    }

    auto query_mouse_input()
    {
      mouse_inputs ret;

      // buttons
      for (int b = 0; b < GLFW_MOUSE_BUTTON_LAST; ++b) {

        auto next = glfwGetMouseButton(glfw_win.get(), b);
        auto diff = next - mouse_prev[b].first;

        if (diff == 0 && next == GLFW_PRESS)
          diff = GLFW_REPEAT;

        using namespace std::chrono_literals;
        auto now = std::chrono::system_clock::now();

        if (diff == GLFW_PRESS - GLFW_RELEASE) {

          if ((now - mouse_prev[b].second) > 0.25s)
            ret.press1.push_back(static_cast<wm::mouse_button>(b));
          else
            ret.press2.push_back(static_cast<wm::mouse_button>(b));

          mouse_prev[b].second = now;
        }

        if (diff == GLFW_RELEASE - GLFW_PRESS)
          ret.release.push_back(static_cast<wm::mouse_button>(b));

        if (diff == GLFW_REPEAT)
          ret.repeat.push_back(static_cast<wm::mouse_button>(b));

        mouse_prev[b].first = next;
      }

      // mouse cursor
      double x, y;
      glfwGetCursorPos(glfw_win.get(), &x, &y);
      ret.cursor_pos   = fvec2(x, y);
      ret.cursor_delta = ret.cursor_pos - cursor_prev;
      cursor_prev      = ret.cursor_pos;

      // convert to virtual screen pos
      ret.cursor_pos += fvec2(glfw_win.pos());

      // hovered?
      ret.hovered = glfwGetWindowAttrib(glfw_win.get(), GLFW_HOVERED);

      return ret;
    }
  };

  viewport_io::viewport_io(glfw::glfw_window& win)
    : m_pimpl {std::make_unique<impl>(win)}
  {
  }

  viewport_io::~viewport_io() noexcept = default;

  auto viewport_io::query_key_input() -> key_inputs
  {
    return m_pimpl->query_key_input();
  }

  auto viewport_io::query_mouse_input() -> mouse_inputs
  {
    return m_pimpl->query_mouse_input();
  }

} // namespace yave::wm