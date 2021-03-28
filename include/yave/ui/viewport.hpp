//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/ui/window.hpp>

namespace yave::ui {

  class root;
  class native_window;
  class window_manager;
  class layout_context;
  class render_context;

  /// Toplevel surface
  class viewport : public window
  {
    // ref to window manager
    ui::window_manager& m_wm;
    // ref to layout context
    ui::layout_context& m_lctx;
    /// ref to render context
    ui::render_context& m_rctx;
    // native window handler
    std::unique_ptr<native_window> m_nw;

  public:
    viewport(
      ui::window_manager& wm,
      ui::layout_context& lctx,
      ui::render_context& rctx,
      std::u8string name,
      ui::size size,
      passkey<root>);

    ~viewport() noexcept;

  public:
    /// get native window
    auto get_native() -> native_window*;
    auto get_native() const -> const native_window*;

  public:
    void layout(layout_scope ctx) const override;
    void render(render_scope ctx) const override;
  };

} // namespace yave::ui
