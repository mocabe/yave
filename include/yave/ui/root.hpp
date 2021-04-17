//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/ui/window.hpp>
#include <yave/ui/viewport.hpp>

#include <chrono>

namespace yave::ui {

  class window_manager;

  /// Root (toplevel) window.
  /// Window manager has single instance of this class as a root of entire
  /// window tree. Root window can have multiple viewports which represent
  /// "window"s in standard desktop GUI systems.
  class root final : public window
  {
    ui::window_manager& m_wm;

  public:
    // initialized from wm
    root(ui::window_manager& wm, passkey<ui::window_manager>&&);

  public:
    void layout(layout_scope ctx) const override;
    void render(render_scope ctx) const override;

  public:
    /// no active viewport?
    bool should_close() const;

    /// add native window
    auto add_viewport(std::u8string name, ui::size size) -> ui::viewport*;

    /// remove native window
    void remove_viewport(const ui::viewport* v);

    /// get viewports
    auto viewports() const
    {
      using namespace ranges;
      return children() | views::transform([](auto* w) {
               return static_cast<const viewport*>(w);
             });
    }

    /// get viewports
    auto viewports()
    {
      using namespace ranges;
      return children() | views::transform([](auto* w) {
               return static_cast<viewport*>(w);
             });
    }
  };

} // namespace yave::ui