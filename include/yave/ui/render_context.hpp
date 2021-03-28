//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/ui/passkey.hpp>
#include <yave/ui/draw_list.hpp>

#include <glm/glm.hpp>

#include <cstdint>
#include <memory>

namespace yave::ui {

  class window;
  class window_manager;
  class view_context;
  class viewport;
  class viewport_renderer;
  class main_context;
  class vulkan_device;

  /// Per window render data
  struct window_render_data
  {
    // renderer for viewport, otherwise nullptr
    std::unique_ptr<viewport_renderer> renderer;

    window_render_data();
    ~window_render_data() noexcept;
  };

  class render_context
  {
    class impl;
    std::unique_ptr<impl> m_pimpl;

  public:
    render_context(main_context& mctx);
    ~render_context() noexcept;

    // get vulkan device
    auto vulkan_device() -> ui::vulkan_device&;

  public:
    /// Process render stage
    void do_render(window_manager& wm, passkey<view_context>);
    /// Render window
    auto render_window(const window* w, draw_list dl) -> draw_list;

  public:
    /// Setup viewport window
    void init_viewport(viewport* vp, passkey<viewport>);
  };

} // namespace yave::ui