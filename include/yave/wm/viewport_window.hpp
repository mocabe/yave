//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/wm/window.hpp>

namespace yave::wm {

  /// abstract viewport window
  class viewport_window : public window
  {
  public:
    /// ctor
    using window::window;

    /// can be closed?
    [[nodiscard]] virtual bool should_close() const = 0;
    /// maximum update frequency
    [[nodiscard]] virtual auto refresh_rate() const -> uint32_t = 0;
    /// execute event loop for viewport
    /// \requires !should_close()
    virtual void exec(editor::data_context&, editor::view_context&) = 0;
  };
} // namespace yave::wm