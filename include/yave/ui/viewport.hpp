//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/ui/window.hpp>

namespace yave::ui {

  namespace controllers {
    class close;
  }

  class root;
  class native_window;
  class window_manager;
  class layout_context;
  class render_context;

  /// Toplevel surface
  class viewport final : public generic_window<viewport>
  {
    // ref to layout context
    ui::layout_context& m_lctx;
    /// ref to render context
    ui::render_context& m_rctx;
    // native window handler
    std::unique_ptr<ui::native_window> m_nw;
    // close controller
    controllers::close* m_close_controller;

  public:
    viewport(
      ui::window_manager& wm,
      ui::layout_context& lctx,
      ui::render_context& rctx,
      std::u8string name,
      ui::size size,
      passkey<root>&&);

    ~viewport() noexcept;

  public:
    /// get native window
    auto native_window() -> ui::native_window&;
    auto native_window() const -> const ui::native_window&;

  public:
    void layout(layout_scope ctx) const override;
    void render(render_scope ctx) const override;

  public:
    /// Has child?
    bool has_child() const;
    /// Set child window
    void set_child(ui::unique<window> c);
    /// Get child window (if exists)
    auto get_child() -> window&;
    /// Remove child window
    void remove_child();

  public:
    auto& close_controller()
    {
      return *m_close_controller;
    }
  };

} // namespace yave::ui
