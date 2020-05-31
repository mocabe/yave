//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/wm/window.hpp>
#include <yave/wm/viewport_window.hpp>

#include <chrono>

namespace yave::wm {

  /// Application root window.
  class root_window final : public window
  {
    friend class window_manager;

    /// window manager owning this window
    window_manager& m_wm;

    /// last update time
    std::chrono::high_resolution_clock::time_point m_last_update;

  public:
    root_window(window_manager& wmngr);

  private: /* unused */
    void render(
      editor::data_context&,
      editor::view_context&,
      editor::render_context&) const override;
    void resize(const fvec2&, const fvec2&) override;

  private:
    /// wait next frame, update viewports
    void update(editor::data_context& dc, editor::view_context& vc) override;
    /// rendering entry point
    void render(editor::data_context& dc, editor::view_context& vc);
    /// dispatch viewport events
    void events(editor::data_context& dc, editor::view_context& vc);

  private:
    /// add new viewport
    auto add_viewport(
      uint32_t width,
      uint32_t height,
      std::u8string name,
      glfw::glfw_context& glfw_ctx) -> viewport_window*;

    /// remove viewport
    void remove_viewport(uid id);

  private:
    /// shoud close event roop?
    bool should_close() const;
  };
} // namespace yave::wm