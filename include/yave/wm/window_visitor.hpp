//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/wm/window.hpp>

namespace yave::wm {

  /// window visitor
  struct window_visitor
  {
  public:
    /// dtor
    virtual ~window_visitor() noexcept = default;

    /// Visit window
    /// \param win window to visit
    /// \returns continuation flag (true for stop, false to continue visiting
    /// other windows)
    virtual bool visit(window* win) = 0;
  };

} // namespace yave::wm