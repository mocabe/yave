//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/ui/box_constraints.hpp>
#include <yave/ui/window_geometry.hpp>

namespace yave::ui {

  class layout_context;
  class window;

  /// Layout scope context
  class layout_scope
  {
    layout_context& m_ctx;
    const ui::window& m_win;
    box_constraints m_constr;

    friend class layout_context;

    layout_scope(
      layout_context& ctx,
      const ui::window& win,
      const box_constraints& c);

  public:
    ~layout_scope() noexcept          = default;
    layout_scope(const layout_scope&) = delete;
    layout_scope(layout_scope&&)      = delete;
    layout_scope& operator=(const layout_scope&) = delete;
    layout_scope& operator=(layout_scope&&) = delete;

    /// Access global context
    auto layout_ctx() -> layout_context&;

    /// Get current window
    auto window() const -> const ui::window&;

    /// Current constraints
    auto constraints() const -> const box_constraints&;

    /// Enter child window scope
    auto enter_child(const ui::window& w, const box_constraints& c) -> ui::size;

    /// Set new size
    void set_size(ui::size new_size);
    void set_size(const ui::window& w, ui::size new_size);
    /// Set offset (optional, parent can ignore)
    void set_offset(ui::vec new_offset);
    void set_offset(const ui::window& w, ui::vec new_offset);
  };

} // namespace yave::ui