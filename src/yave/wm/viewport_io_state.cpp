//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/wm/viewport_io_state.hpp>
#include <yave/wm/viewport_io.hpp>
#include <yave/wm/window_manager.hpp>

#include <chrono>

namespace yave::wm {

  namespace {

    /// synthesized from raw input transition
    enum class mouse_delta_state
    {
      idle    = 0, ///< up -> up
      press   = 1, ///< up -> down
      repeat  = 2, ///< down -> down
      release = 3, ///< down -> up
    };

    /// mouse button state
    struct mouse_state
    {
      /// raw io state
      int raw_state = GLFW_RELEASE;
      /// delta state
      mouse_delta_state delta_state = mouse_delta_state::idle;

      /// last pressed window
      uid window_pressed = {};
      /// last press time
      std::chrono::steady_clock::time_point time_pressed = {};
    };

  } // namespace

  class viewport_io_state::impl
  {
  public:
    glfw::glfw_window& win;
    wm::window_manager& wm;

  public:
    wm::viewport_io io;

  private:
    /// map of mouse states
    std::array<mouse_state, GLFW_MOUSE_BUTTON_LAST> mouse_states;

  private:
    /// window currently grabbed
    window* grabbed_window = nullptr;

  private:
    /// click events
    std::vector<wm::mouse_button> clicks;
    /// double click events
    std::vector<wm::mouse_button> double_clicks;

  public:
    impl(glfw::glfw_window& win, wm::window_manager& wm)
      : win {win}
      , wm {wm}
      , io {win}
    {
    }

  private:
    void update()
    {
      io.update();

      for (int b = 0; b < GLFW_MOUSE_BUTTON_LAST; ++b) {
        auto& state = mouse_states[b];
        auto down   = io.is_mouse_button_down(static_cast<wm::mouse_button>(b));
        auto up     = io.is_mouse_button_up(static_cast<wm::mouse_button>(b));

        // update delta events
        switch (state.raw_state) {
          case GLFW_PRESS:
            if (down)
              state.delta_state = mouse_delta_state::repeat;
            if (up)
              state.delta_state = mouse_delta_state::release;
            break;
          case GLFW_RELEASE:
            if (down)
              state.delta_state = mouse_delta_state::press;
            if (up)
              state.delta_state = mouse_delta_state::idle;
            break;
        }
        // update raw event
        state.raw_state = down ? GLFW_PRESS : GLFW_RELEASE;
      }
    }

  private:
    auto& get_mouse_state(wm::mouse_button& button)
    {
      auto idx = static_cast<size_t>(
        static_cast<std::underlying_type_t<wm::mouse_button>>(button));
      return mouse_states[idx];
    }

  public:
    void register_mouse_press1(uid id, wm::mouse_button button)
    {
      auto& state = get_mouse_state(button);

      state.last_pressed    = id;
      state.last_press_type = 1;
    }

    void register_mouse_press2(uid id, wm::mouse_button button)
    {
      auto& state = get_mouse_state(button);

      if (wm.exists(state.last_pressed) && state.last_pressed == id) {
        state.last_press_type = 2;
      } else {
        state.last_pressed    = id;
        state.last_press_type = 1;
      }
    }

    void register_mouse_release(uid id, wm::mouse_button button)
    {
      auto& state = get_mouse_state(button);

      int click = 0;

      if (wm.exists(state.last_pressed) && state.last_pressed == id) {
        click = state.last_press_type;
        if (state.last_press_type == 2) {
          state = {};
        }
      } else {
        state = {};
      }

      if (click == 1)
        clicks.push_back(button);

      if (click == 2)
        double_clicks.push_back(button);
    }

  public:
    auto query_mouse_click()
    {
      return std::move(clicks);
    }

    auto query_mouse_double_click()
    {
      return std::move(double_clicks);
    }
  };

  viewport_io_state::viewport_io_state(wm::window_manager& wm)
    : m_pimpl {std::make_unique<impl>(wm)}
  {
  }

  viewport_io_state::~viewport_io_state() noexcept = default;

  void viewport_io_state::register_mouse_press1(
    uid window_id,
    wm::mouse_button button)
  {
    m_pimpl->register_mouse_press1(window_id, button);
  }

  void viewport_io_state::register_mouse_press2(
    uid window_id,
    wm::mouse_button button)
  {
    m_pimpl->register_mouse_press2(window_id, button);
  }

  void viewport_io_state::register_mouse_release(
    uid window_id,
    wm::mouse_button button)
  {
    m_pimpl->register_mouse_release(window_id, button);
  }

  auto viewport_io_state::query_mouse_click() //
    -> std::vector<wm::mouse_button>
  {
    return m_pimpl->query_mouse_click();
  }

  auto viewport_io_state::query_mouse_double_click()
    -> std::vector<wm::mouse_button>
  {
    return m_pimpl->query_mouse_double_click();
  }

} // namespace yave::wm