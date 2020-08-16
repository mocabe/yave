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

    struct state_data
    {
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

    void dispatch_events(
      const editor::data_context& dctx,
      const editor::view_context& vctx)
    {
      auto focus = wm.get_key_focus();

      if (!focus)
        return;

      for (int i = GLFW_KEY_SPACE; i < GLFW_KEY_LAST; ++i) {

        auto key = static_cast<wm::key>(i);

        // no event to process
        if (!io.key_event(key).has_value())
          continue;

        auto event = *io.key_event(key);

        // key press/repeat
        if (event == key_event::press || event == key_event::repeat) {
          auto e = std::make_unique<wm::events::key_press>(key, event);
          auto d = wm::key_press_dispatcher(
            std::forward_as_tuple(std::move(e), dctx, vctx), std::tuple());
          d.dispatch(focus);
        }

        // key release
        if (event == key_event::release) {
          auto e = std::make_unique<wm::events::key_release>(key);
          auto d = wm::key_release_dispatcher(
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