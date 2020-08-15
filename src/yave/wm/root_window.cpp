//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/wm/root_window.hpp>

#include <chrono>
#include <thread>

namespace yave::wm {

  constexpr auto fmax = std::numeric_limits<float>::max();

  root_window::root_window(window_manager& wm)
    : window(
      &wm,
      "wm::root_window",
      // virtual screen origin
      fvec2 {0, 0},
      // infinitely large area
      fvec2 {fmax, fmax})
  {
  }

  void root_window::draw(
    const editor::data_context& data_ctx,
    const editor::view_context& view_ctx) const
  {
    assert(false);
  }

  void root_window::update(
    editor::data_context& data_ctx,
    editor::view_context& view_ctx)
  {
    assert(false);
  }

  bool root_window::should_close() const
  {
    return children().empty();
  }

  void root_window::exec(editor::data_context& dctx, editor::view_context& vctx)
  {
    auto& viewports = children();

    // remove closed windows
    for (auto it = std::find_if(
           viewports.begin(),
           viewports.end(),
           [](auto& c) {
             return static_cast<const wm::viewport_window*>(c.get())
               ->should_close();
           });
         it != viewports.end();) {
      remove_any_window(it->get()->id());
    }

    if (viewports.empty())
      return;

    // wait until next frame
    {
      // get update rate
      // TODO: support per-viewport framerates
      uint32_t fps = 60;
      for (auto&& c : viewports)
        fps = std::max(
          fps,
          static_cast<const wm::viewport_window*>(c.get())->refresh_rate());

      using namespace std::chrono_literals;
      auto end_time          = std::chrono::high_resolution_clock::now();
      auto frame_time        = end_time - m_last_update;
      auto frame_time_window = std::chrono::nanoseconds(1s) / fps;
      auto sleep_time        = frame_time_window - frame_time;

      // wait based on fps
      if (sleep_time.count() > 0) {
        m_last_update = end_time + sleep_time;
        std::this_thread::sleep_for(sleep_time);
      } else {
        m_last_update = end_time;
      }
    }

    // let viewports run its event loop
    for (auto&& c : children()) {
      static_cast<viewport_window*>(as_mut_child(c))->exec(dctx, vctx);
    }
  }

  auto root_window::add_viewport(std::unique_ptr<viewport_window>&& win)
    -> viewport_window*
  {
    auto ret = win.get();
    add_any_window(children().size(), std::move(win));
    return ret;
  }

  void root_window::remove_viewport(uid id)
  {
    remove_any_window(id);
  }
} // namespace yave::wm