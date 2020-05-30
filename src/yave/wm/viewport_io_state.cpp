//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/wm/viewport_io_state.hpp>
#include <yave/wm/window_manager.hpp>
#include <yave/lib/glfw/glfw_context.hpp>

namespace yave::wm {

  namespace {

    struct mouse_state
    {
      /// last pressed window
      uid last_pressed = {};
      /// last press type. 1 for press1, 2 for press2.
      int last_press_type = 0;
    };

    using mouse_state_map = std::array<mouse_state, GLFW_MOUSE_BUTTON_LAST>;

  } // namespace

  class viewport_io_state::impl
  {
    wm::window_manager& wm;

  private:
    /// current mouse state
    mouse_state_map mouse_states;

  private:
    /// click events
    std::vector<wm::mouse_button> clicks;
    /// double click events
    std::vector<wm::mouse_button> double_clicks;

  public:
    impl(wm::window_manager& wm)
      : wm {wm}
    {
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