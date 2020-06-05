//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/wm/root_window.hpp>
#include <yave/wm/window_manager.hpp>

#include <thread>
#include <limits>
#include <algorithm>

namespace yave::wm {

  root_window::root_window(wm::window_manager& wmngr)
    : window(
      u8"root_window",
      // virtual screen origin
      {0, 0},
      // has infinitely large size: useful for hit detection
      {std::numeric_limits<float>::max(), std::numeric_limits<float>::max()})
    , m_wm {wmngr}
    , m_last_update {}
  {
  }

  void root_window::render(
    editor::data_context&,
    editor::view_context&,
    editor::render_context&) const
  {
    assert(false);
  }

  void root_window::resize(const fvec2&, const fvec2&)
  {
    assert(false);
  }

  void root_window::update(
    editor::data_context& data_ctx,
    editor::view_context& view_ctx)
  {
    auto& viewports = children();

    // remove closed windows
    auto it = std::remove_if(viewports.begin(), viewports.end(), [&](auto& c) {
      return c->template as<wm::viewport_window>()->should_close();
    });
    viewports.erase(it, viewports.end());

    if (viewports.empty())
      return;

    // wait until next frame
    {
      // get update rate
      // TODO: support per-viewport framerates
      uint32_t fps = 60;
      for (auto&& c : viewports)
        fps = std::max(fps, c->as<wm::viewport_window>()->fps());

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

    // dispatch viewport events
    for (auto&& c : children()) {
      as_mut_child(c)->update(data_ctx, view_ctx);
    }
  }

  void root_window::render(
    editor::data_context& dctx,
    editor::view_context& vctx)
  {
    for (auto&& c : children())
      as_mut_child(c)->as<wm::viewport_window>()->render(dctx, vctx);
  }

  void root_window::events(
    editor::data_context& data_ctx,
    editor::view_context& view_ctx)
  {
    for (auto&& c : children())
      as_mut_child(c)->as<wm::viewport_window>()->events(data_ctx, view_ctx);
  }

  auto root_window::add_viewport(
    uint32_t width,
    uint32_t height,
    std::u8string name,
    vulkan::vulkan_context& vulkan_ctx,
    glfw::glfw_context& glfw_ctx) -> viewport_window*
  {
    // create new viewoprt
    auto vp = std::make_unique<wm::viewport_window>(
      m_wm,
      vulkan_ctx,
      glfw_ctx.create_window(width, height, (const char*)name.c_str()));

    auto ret = vp.get();
    add_any_window(children().end(), std::move(vp));
    return ret;
  }

  void root_window::remove_viewport(uid id)
  {
    remove_any_window(id);
  }

  bool root_window::should_close() const
  {
    return children().empty();
  }
} // namespace yave::wm
