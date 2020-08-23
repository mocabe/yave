//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/wm/mouse_event_emitter.hpp>
#include <yave/wm/viewport_io.hpp>
#include <yave/wm/viewport_window.hpp>
#include <yave/wm/root_window.hpp>
#include <yave/editor/view_context.hpp>

#include <chrono>

#include <glm/gtx/string_cast.hpp>
#include <yave/support/log.hpp>

YAVE_DECL_G_LOGGER(mouse_event_emitter);

namespace yave::wm {

  namespace {

    enum class click_state
    {
      idle    = 0,
      pressed = 1,
      clicked = 2,
    };

    struct state_data
    {
      /// clicks: last pressed key
      mouse_button last_pressed_key = static_cast<mouse_button>(-1);
      /// clicks: last pressed window id
      uid last_pressed_window = {};
      /// clicks: last pressed time
      std::chrono::steady_clock::time_point last_pressed_time = {};
      /// clicks: current click state
      wm::click_state click_state = wm::click_state::idle;
    };
  } // namespace

  class mouse_event_emitter::impl
  {
    viewport_io& io;
    window_manager& wm;

  public:
    impl(viewport_io& io, window_manager& wm)
      : io {io}
      , wm {wm}
    {
      init_logger();
    }

  private:
    /// last leaf window id
    uid last_region = {};

    void dispatch_cursor_events(
      viewport_window* vp,
      const editor::data_context& dctx,
      const editor::view_context& vctx)
    {
      auto prev_region = wm.get_window(last_region);
      auto root        = wm.root();

      // window no longer exists!
      if (!prev_region) {
        last_region = root->id();
        prev_region = root;
      }

      window* next_region;

      // mouse hover
      {
        auto e = std::make_unique<wm::events::mouse_move>(
          io.mouse_pos(), io.mouse_delta());
        auto d = wm::mouse_move_dispatcher(
          std::forward_as_tuple(std::move(e), dctx, vctx), std::tuple());
        // dispatch from root
        d.dispatch(root);
        // get new region
        next_region = d.visitor().region();
      }

      assert(prev_region && next_region);
      assert(wm.exists(prev_region->id()) && wm.exists(next_region->id()));

      if (prev_region != next_region) {
        // parent -> child
        if (wm.is_child(next_region, prev_region)) {

          // child: enter
          {
            auto e = std::make_unique<wm::events::mouse_enter>(io.mouse_pos());
            auto d = wm::mouse_enter_dispatcher(
              std::forward_as_tuple(std::move(e), dctx, vctx),
              std::forward_as_tuple(false));
            d.dispatch(next_region);
          }
          // child: over
          {
            auto e = std::make_unique<wm::events::mouse_over>(io.mouse_pos());
            auto d = wm::mouse_over_dispatcher(
              std::forward_as_tuple(std::move(e), dctx, vctx),
              std::forward_as_tuple(true));
            d.dispatch(next_region);
          }
          // parent: out
          {
            auto e = std::make_unique<wm::events::mouse_out>(io.mouse_pos());
            auto d = wm::mouse_out_dispatcher(
              std::forward_as_tuple(std::move(e), dctx, vctx),
              std::forward_as_tuple(true));
            d.dispatch(prev_region);
          }
        }
        // child -> parent
        else if (wm.is_parent(next_region, prev_region)) {

          // child: leave
          {
            auto e = std::make_unique<wm::events::mouse_leave>(io.mouse_pos());
            auto d = wm::mouse_leave_dispatcher(
              std::forward_as_tuple(std::move(e), dctx, vctx),
              std::forward_as_tuple(false));
            d.dispatch(prev_region);
          }
          // child: out
          {
            auto e = std::make_unique<wm::events::mouse_out>(io.mouse_pos());
            auto d = wm::mouse_out_dispatcher(
              std::forward_as_tuple(std::move(e), dctx, vctx),
              std::forward_as_tuple(true));
            d.dispatch(prev_region);
          }
          // parent: over
          {
            auto e = std::make_unique<wm::events::mouse_over>(io.mouse_pos());
            auto d = wm::mouse_over_dispatcher(
              std::forward_as_tuple(std::move(e), dctx, vctx),
              std::forward_as_tuple(true));
            d.dispatch(next_region);
          }
        }
        // other cases
        else {
          // prev: leave
          {
            auto e = std::make_unique<wm::events::mouse_leave>(io.mouse_pos());
            auto d = wm::mouse_leave_dispatcher(
              std::forward_as_tuple(std::move(e), dctx, vctx),
              std::forward_as_tuple(false));
            d.dispatch(prev_region);
          }
          // prev: out
          {
            auto e = std::make_unique<wm::events::mouse_out>(io.mouse_pos());
            auto d = wm::mouse_out_dispatcher(
              std::forward_as_tuple(std::move(e), dctx, vctx),
              std::forward_as_tuple(true));
            d.dispatch(prev_region);
          }

          // next: enter
          {
            auto e = std::make_unique<wm::events::mouse_enter>(io.mouse_pos());
            auto d = wm::mouse_enter_dispatcher(
              std::forward_as_tuple(std::move(e), dctx, vctx),
              std::forward_as_tuple(false));
            d.dispatch(next_region);
          }
          // next: over
          {
            auto e = std::make_unique<wm::events::mouse_over>(io.mouse_pos());
            auto d = wm::mouse_over_dispatcher(
              std::forward_as_tuple(std::move(e), dctx, vctx),
              std::forward_as_tuple(true));
            d.dispatch(next_region);
          }
        }

        // update
        last_region = next_region->id();
      }
    }

  private:
    // button states
    std::array<state_data, GLFW_MOUSE_BUTTON_LAST> states;

    void dispatch_button_events(
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
          state.click_state         = click_state::idle;
        };

        // invalid window id
        if (state.last_pressed_window != uid())
          if (!wm.exists(state.last_pressed_window))
            reset_click_state();

        // no event to process
        if (!io.mouse_button_event(button).has_value())
          continue;

        auto event = *io.mouse_button_event(button);

        // mouse press
        if (event == mouse_button_event::press) {
          auto e_press =
            std::make_unique<wm::events::mouse_press>(button, io.mouse_pos());
          auto d_press = wm::mouse_press_dispatcher(
            std::forward_as_tuple(std::move(e_press), dctx, vctx),
            std::tuple());
          d_press.dispatch(vp);

          // update states
          if (d_press.visitor().accepted()) {
            auto wid = d_press.visitor().reciever()->id();
            auto now = std::chrono::steady_clock::now();

            assert(wid != uid());

            using namespace std::literals::chrono_literals;

            auto start_new_click = [&]() {
              state.last_pressed_window = wid;
              state.last_pressed_time   = now;
              state.click_state         = click_state::pressed;
            };

            // idle
            if (state.click_state == click_state::idle)
              start_new_click();

            // pressed
            if (state.click_state == click_state::pressed)
              start_new_click();

            // clicked
            if (state.click_state == click_state::clicked) {
              if (state.last_pressed_window == wid) {
                if (now - state.last_pressed_time < 500ms) {
                  auto e_dbl = std::make_unique<wm::events::mouse_double_click>(
                    button, io.mouse_pos());
                  auto d_dbl = wm::mouse_double_click_dispatcher(
                    std::forward_as_tuple(std::move(e_dbl), dctx, vctx),
                    std::tuple());
                  d_dbl.dispatch(vp);
                  // clicked -> idle
                  reset_click_state();
                } else
                  // timeout, fallback to click
                  start_new_click();
              } else
                // different window, back to click
                start_new_click();
            }
          }
        }

        // mouse release
        if (event == mouse_button_event::release) {
          auto e_rel =
            std::make_unique<wm::events::mouse_release>(button, io.mouse_pos());
          auto d_rel = wm::mouse_release_dispatcher(
            std::forward_as_tuple(std::move(e_rel), dctx, vctx), std::tuple());
          d_rel.dispatch(vp);

          if (d_rel.visitor().accepted()) {
            auto wid = d_rel.visitor().reciever()->id();

            assert(wid != uid());

            // idle
            if (state.click_state == click_state::idle)
              reset_click_state();

            // pressed
            if (state.click_state == click_state::pressed) {

              if (state.last_pressed_window == wid) {
                auto e_clk = std::make_unique<wm::events::mouse_click>(
                  button, io.mouse_pos());
                auto d_clk = wm::mouse_click_dispatcher(
                  std::forward_as_tuple(std::move(e_clk), dctx, vctx),
                  std::tuple());
                d_clk.dispatch(vp);
                // pressed -> clicked
                state.click_state = click_state::clicked;
              } else
                // window missmatch
                reset_click_state();
            }

            // clicked
            if (state.click_state == click_state::clicked) {
              // double release on different windows
              if (state.last_pressed_window != wid)
                reset_click_state();
            }
          }
        }
      }
    }

  public:
    void dispatch_events(
      viewport_window* vp,
      const editor::data_context& dctx,
      const editor::view_context& vctx)
    {
      dispatch_cursor_events(vp, dctx, vctx);
      dispatch_button_events(vp, dctx, vctx);
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