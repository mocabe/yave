//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/wm/root_window.hpp>

#include <chrono>
#include <thread>
#include <range/v3/algorithm.hpp>
#include <range/v3/view.hpp>

namespace yave::wm {

  namespace rs = ranges;
  namespace rv = ranges::views;

  constexpr auto fmax = std::numeric_limits<float>::max();

  root_window::root_window(window_manager& wm)
    : window(
      "wm::root_window",
      // virtual screen origin
      glm::vec2 {0, 0},
      // infinitely large area
      glm::vec2 {fmax, fmax})
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
    // remove closed windows
    for (;;) {
      auto vps = viewports();
      auto it  = std::find_if(
        vps.begin(), vps.end(), [](auto& vp) { return vp->should_close(); });

      if (it == vps.end())
        break;

      remove_child_window((*it)->id());
    }

    if (viewports().empty())
      return;

    // wait until next frame
    {
      // get update rate
      // TODO: support per-viewport framerates
      uint32_t fps = 60;
      for (auto&& vp : viewports())
        fps = std::max(fps, vp->refresh_rate());

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
    for (auto&& vp : viewports()) {
      vp->exec(dctx, vctx);
    }
  }

  auto root_window::viewports() const -> std::span<const viewport_window* const>
  {
    auto vs        = children();
    using elem_ptr = std::add_pointer_t<const viewport_window* const>;
    return {reinterpret_cast<elem_ptr>(vs.data()), vs.size()};
  }

  auto root_window::viewports() -> std::span<viewport_window* const>
  {
    auto vs        = children();
    using elem_ptr = std::add_pointer_t<viewport_window* const>;
    return {reinterpret_cast<elem_ptr>(vs.data()), vs.size()};
  }

  auto root_window::add_viewport(std::unique_ptr<viewport_window>&& win)
    -> viewport_window*
  {
    auto ret = win.get();
    add_child_window(children().size(), std::move(win));
    return ret;
  }

  void root_window::remove_viewport(uid id)
  {
    remove_child_window(id);
  }
} // namespace yave::wm