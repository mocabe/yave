//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/ui/render_scope.hpp>
#include <yave/ui/render_context.hpp>

namespace yave::ui {

  render_scope::render_scope(
    render_context& ctx,
    ui::draw_list dl,
    std::optional<ui::draw_list>& out_dl)
    : m_ctx {ctx}
    , m_dl {std::move(dl)}
    , m_out_dl {out_dl}
  {
  }

  render_scope::~render_scope() noexcept
  {
    m_out_dl = std::move(m_dl);
  }

  auto render_scope::enter_child(const window* w, ui::draw_list dl)
    -> ui::draw_list
  {
    return m_ctx.render_window(w, std::move(dl));
  }

  void render_scope::enter_child(const window* w)
  {
    m_dl = enter_child(w, std::move(m_dl));
  }

} // namespace yave::ui