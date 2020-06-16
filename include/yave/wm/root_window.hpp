//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/wm/window.hpp>

#include <chrono>

namespace yave::wm {

  /// root window
  class root_window final : public window
  {
  public:
    /// ctor
    root_window();

  public:
    void draw(editor::data_context&, editor::view_context&) const override;
    void update(editor::data_context&, editor::view_context&) override;

  public:
    /// add child window
    auto add_window(std::unique_ptr<window>&& win) -> window*;
    /// remove child window
    void remove_window(uid id);
  };
} // namespace yave::wm