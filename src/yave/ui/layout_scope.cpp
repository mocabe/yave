//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/ui/layout_scope.hpp>
#include <yave/ui/layout_context.hpp>
#include <yave/ui/window.hpp>

namespace yave::ui {

  layout_scope::layout_scope(
    layout_context& ctx,
    const ui::window* win,
    const box_constraints& c)
    : m_ctx {ctx}
    , m_win {win}
    , m_constr {c}
  {
  }

  auto layout_scope::layout_ctx() -> layout_context&
  {
    return m_ctx;
  }

  auto layout_scope::window() const -> const ui::window*
  {
    return m_win;
  }

  auto layout_scope::constraints() const -> const box_constraints&
  {
    return m_constr;
  }

  auto layout_scope::enter_child(const ui::window* w, const box_constraints& c)
    -> ui::size
  {
    assert(w->parent() == m_win);
    return m_ctx.layout_window(w, c, {});
  }

  void layout_scope::set_size(ui::size new_size)
  {
    m_ctx.set_size(m_win, new_size, {});
  }

  void layout_scope::set_size(const ui::window* w, ui::size new_size)
  {
    m_ctx.set_size(w, new_size, {});
  }

  void layout_scope::set_offset(ui::vec new_offset)
  {
    m_ctx.set_offset(m_win, new_offset, {});
  }

  void layout_scope::set_offset(const ui::window* w, ui::vec new_offset)
  {
    m_ctx.set_offset(w, new_offset, {});
  }
}