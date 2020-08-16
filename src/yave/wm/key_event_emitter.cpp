//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/wm/key_event_emitter.hpp>
#include <yave/wm/viewport_io.hpp>
#include <yave/wm/viewport_window.hpp>
#include <yave/editor/view_context.hpp>

namespace yave::wm {

  namespace {

    enum class key_delta_state
    {
      idle    = 0, ///< up -> up
      press   = 1, ///< up -> down
      repeat  = 2, ///< down -> down
      release = 3, ///< down -> up
    };

    struct state_data
    {
      key_state raw_state         = key_state::up;
      key_delta_state delta_state = key_delta_state::idle;
    };
  } // namespace

  class key_event_emitter::impl
  {
    viewport_io& io;
    window_manager& wm;

    std::array<state_data, GLFW_KEY_LAST> states;

  public:
    impl(viewport_io& io, window_manager& wm)
      : io {io}
      , wm {wm}
    {
    }

    void update_states()
    {
      for (int i = GLFW_KEY_SPACE; i < GLFW_KEY_LAST; ++i) {
        auto& state   = states[i];
        auto next_raw = io.key_state(static_cast<wm::key>(i));

        // match/update delta state
        auto t = [&](auto p, auto n, auto d) {
          if (state.raw_state == p && next_raw == n)
            state.delta_state = d;
        };

        t(key_state::up, key_state::up, key_delta_state::idle);
        t(key_state::up, key_state::down, key_delta_state::press);
        t(key_state::down, key_state::up, key_delta_state::release);
        t(key_state::down, key_state::down, key_delta_state::repeat);

        state.raw_state = next_raw;
      }
    }

    void dispatch_events(
      const editor::data_context& dctx,
      const editor::view_context& vctx)
    {
      update_states();

      auto focus = wm.get_key_focus();

      if (!focus)
        return;

      for (int i = GLFW_KEY_SPACE; i < GLFW_KEY_LAST; ++i) {
        auto& state = states[i];
        auto key    = static_cast<wm::key>(i);

        // key press
        if (state.delta_state == key_delta_state::press) {
          auto e = std::make_unique<wm::events::key_press>(key);
          auto d = wm::key_press_dispatcher(
            std::forward_as_tuple(std::move(e), dctx, vctx), std::tuple());
          d.dispatch(focus);
        }

        // key release
        if (state.delta_state == key_delta_state::release) {
          auto e = std::make_unique<wm::events::key_release>(key);
          auto d = wm::key_release_dispatcher(
            std::forward_as_tuple(std::move(e), dctx, vctx), std::tuple());
          d.dispatch(focus);
        }

        // key repeat
        if (state.delta_state == key_delta_state::repeat) {
          auto e = std::make_unique<wm::events::key_repeat>(key);
          auto d = wm::key_repeat_dispatcher(
            std::forward_as_tuple(std::move(e), dctx, vctx), std::tuple());
          d.dispatch(focus);
        }
      }

      // key char
      if (auto txt = io.key_text(); !txt.empty()) {
        auto e = std::make_unique<wm::events::key_char>(std::move(txt));
        auto d = wm::key_char_dispatcher(
          std::forward_as_tuple(std::move(e), dctx, vctx), std::tuple());
        d.dispatch(focus);
      }
    }
  };

  key_event_emitter::key_event_emitter(viewport_io& io, window_manager& wm)
    : m_pimpl {std::make_unique<impl>(io, wm)}
  {
  }

  key_event_emitter::~key_event_emitter() noexcept = default;

  void key_event_emitter::dispatch_events(
    viewport_window* /*viewport*/,
    const editor::data_context& dctx,
    const editor::view_context& vctx)
  {
    m_pimpl->dispatch_events(dctx, vctx);
  }

} // namespace yave::wm