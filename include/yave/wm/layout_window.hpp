//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/wm/window.hpp>

namespace yave::wm {

  /// window type for layout
  class layout_window : public window
  {
  public:
    using window::window;

    /// add new window to layout
    virtual auto add_window(std::unique_ptr<window>&& win) -> window* = 0;
    /// detach window
    virtual auto detach_window(uid id) -> std::unique_ptr<window> = 0;
  };

} // namespace yave::wm