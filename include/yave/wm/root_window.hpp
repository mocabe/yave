//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/wm/window.hpp>
#include <yave/wm/viewport_window.hpp>

#include <chrono>

namespace yave::wm {

  class window_manager;

  /// root window
  class root_window final : public window
  {
    // last update time
    std::chrono::high_resolution_clock::time_point m_last_update;

  public:
    /// ctor
    root_window(window_manager& wm);

  public:
    void draw(const editor::data_context&, const editor::view_context&)
      const override;
    void update(editor::data_context&, editor::view_context&) override;

  public:
    /// no active viewport?
    [[nodiscard]] bool should_close() const;
    /// draw viewports
    void exec(editor::data_context&, editor::view_context&);

  public:
    /// add viewport
    auto add_viewport(std::unique_ptr<viewport_window>&& win)
      -> viewport_window*;
    /// remove viewport
    void remove_viewport(uid id);
  };
} // namespace yave::wm