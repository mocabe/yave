//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/ui/window_geometry.hpp>
#include <yave/ui/size_constraints.hpp>
#include <yave/ui/layout_constraints.hpp>

namespace yave::ui {

  class layout_context;
  class window;

  /// Layout scope context
  class layout_scope
  {
    layout_context& m_ctx;
    const ui::window& m_win;
    size_constraints m_constr;

    friend class layout_context;

    layout_scope(
      layout_context& ctx,
      const ui::window& win,
      const size_constraints& c);

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
    auto constraints() const -> const ui::size_constraints&;

    /// Enter child window scope
    auto enter_child(const ui::window& w, const size_constraints& c) -> size;

    /// Set new size
    void set_size(const size& s);
    void set_size(const ui::window& w, size s);
    /// Set offset (optional, parent can ignore)
    void set_offset(const ui::vec& o);
    void set_offset(const ui::window& w, vec o);
    /// Set new constraints
    void set_anchor(const ui::layout_constraints& c);
    void set_anchor(const ui::window& w, const ui::layout_constraints& c);
  };

} // namespace yave::ui