//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/wm/mouse_event_emitter.hpp>
#include <yave/wm/viewport_io.hpp>
#include <yave/wm/viewport_window.hpp>
#include <yave/editor/view_context.hpp>

#include <chrono>

namespace yave::wm {

  namespace {

    /// synthesized from raw input transition
    enum class mouse_button_delta_state
    {
      idle    = 0, ///< up -> up
      press   = 1, ///< up -> down
      repeat  = 2, ///< down -> down
      release = 3, ///< down -> up
    };

    struct state_data
    {
      /// raw state
      mouse_button_state raw_state = mouse_button_state::up;
      /// delta
      mouse_button_delta_state delta_state = mouse_button_delta_state::release;

      /// clicks: last pressed key
      mouse_button last_pressed_key = static_cast<mouse_button>(-1);
      /// clicks: last pressed window id
      uid last_pressed_window = {};
      /// clicks: last pressed time
      std::chrono::steady_clock::time_point last_pressed_time = {};
      /// clicks: sequential press count on same widnow.
      /// reset when double click event fired.
      int click_count = 0;
    };
  } // namespace

  class mouse_event_emitter::impl
  {
    viewport_io& io;
    window_manager& wm;

    std::array<state_data, GLFW_MOUSE_BUTTON_LAST> states;

  public:
    impl(viewport_io& io, window_manager& wm)
      : io {io}
      , wm {wm}
    {
    }

    void update_states()
    {
      for (int i = 0; i < GLFW_MOUSE_BUTTON_LAST; ++i) {
        auto& state   = states[i];
        auto next_raw = io.mouse_button_state(static_cast<wm::mouse_button>(i));

        // match/update delta state
        auto t = [&](auto p, auto n, auto d) {
          if (state.raw_state == p && next_raw == n)
            state.delta_state = d;
        };

        t(mouse_button_state::up,
          mouse_button_state::up,
          mouse_button_delta_state::idle);

        t(mouse_button_state::up,
          mouse_button_state::down,
          mouse_button_delta_state::press);

        t(mouse_button_state::down,
          mouse_button_state::up,
          mouse_button_delta_state::release);

        t(mouse_button_state::down,
          mouse_button_state::down,
          mouse_button_delta_state::repeat);

        state.raw_state = next_raw;
      }
    }

    void do_dispatch(
      viewport_window* vp,
      const editor::data_context& dctx,
      const editor::view_context& vctx)
    {
      for (int i = 0; i < GLFW_MOUSE_BUTTON_LAST; ++i) {
        auto& state = states[i];
        auto button = static_cast<wm::mouse_button>(i);

        auto reset_click_state = [&] {
          state.last_pressed_window = {};
          state.last_pressed_time   = {};
          state.click_count         = 0;
        };

        // invalid window id
        if (state.last_pressed_window != uid())
          if (!wm.exists(state.last_pressed_window))
            reset_click_state();

        // mouse press
        if (state.delta_state == mouse_button_delta_state::press) {
          auto e =
            std::make_unique<wm::events::mouse_press>(button, io.mouse_pos());
          auto d = wm::mouse_press_dispatcher(
            std::forward_as_tuple(std::move(e), dctx, vctx), std::tuple());
          d.dispatch(vp);

          // update states
          if (d.visitor().accepted()) {
            auto wid = d.visitor().reciever()->id();
            auto now = std::chrono::steady_clock::now();

            assert(wid != uid());

            using namespace std::literals::chrono_literals;

            state.click_count = 1;

            if (state.last_pressed_window == wid) {
              if (now - state.last_pressed_time < 500ms)
                state.click_count = 2;
            }

            state.last_pressed_window = wid;
            state.last_pressed_time   = now;

            // double click
            if (state.click_count == 2) {
              auto e = std::make_unique<wm::events::mouse_double_click>(
                button, io.mouse_pos());
              auto d = wm::mouse_double_click_dispatcher(
                std::forward_as_tuple(std::move(e), dctx, vctx), std::tuple());
              d.dispatch(vp);

              reset_click_state();
            }
          }
        }

        // mouse repeat
        if (state.delta_state == mouse_button_delta_state::repeat) {
          auto e =
            std::make_unique<wm::events::mouse_repeat>(button, io.mouse_pos());
          auto d = wm::mouse_repeat_dispatcher(
            std::forward_as_tuple(std::move(e), dctx, vctx), std::tuple());
          d.dispatch(vp);
        }

        // mouse release
        if (state.delta_state == mouse_button_delta_state::release) {
          auto e =
            std::make_unique<wm::events::mouse_release>(button, io.mouse_pos());
          auto d = wm::mouse_release_dispatcher(
            std::forward_as_tuple(std::move(e), dctx, vctx), std::tuple());
          d.dispatch(vp);

          if (d.visitor().accepted()) {
            auto wid = d.visitor().reciever()->id();

            assert(wid != uid());

            if (state.last_pressed_window == wid) {

              // click
              if (state.click_count == 1) {
                auto e = std::make_unique<wm::events::mouse_click>(
                  button, io.mouse_pos());
                auto d = wm::mouse_click_dispatcher(
                  std::forward_as_tuple(std::move(e), dctx, vctx),
                  std::tuple());
                d.dispatch(vp);
              }

            } else
              // press/release window missmatch
              reset_click_state();
          }
        }
      }

      // mouse hover
      if (io.window().hovered()) {
        auto e = std::make_unique<wm::events::mouse_hover>(io.mouse_pos());
        auto d = wm::mouse_hover_dispatcher(
          std::forward_as_tuple(std::move(e), dctx, vctx), std::tuple());
        d.dispatch(vp);
      }
    }

    void dispatch_events(
      viewport_window* vp,
      const editor::data_context& dctx,
      const editor::view_context& vctx)
    {
      update_states();
      do_dispatch(vp, dctx, vctx);
    }
  };

  mouse_event_emitter::mouse_event_emitter(viewport_io& io, window_manager& wm)
    : m_pimpl {std::make_unique<impl>(io, wm)}
  {
  }

  mouse_event_emitter::~mouse_event_emitter() noexcept = default;

  void mouse_event_emitter::dispatch_events(
    viewport_window* viewport,
    const editor::data_context& dctx,
    const editor::view_context& vctx)
  {
    m_pimpl->dispatch_events(viewport, dctx, vctx);
  }
} // namespace yave::wm