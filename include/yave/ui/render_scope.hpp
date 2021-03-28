//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/ui/draw_list.hpp>

#include <optional>

namespace yave::ui {

  class window;
  class render_context;

  class render_scope
  {
    render_context& m_ctx;
    // current draw list
    ui::draw_list m_dl;
    // ref to output draw list
    std::optional<ui::draw_list>& m_out_dl;

    friend class render_context;

    render_scope(
      render_context& ctx,
      ui::draw_list dl,
      std::optional<ui::draw_list>& out_dl);

  public:
    ~render_scope() noexcept;
    render_scope(const render_scope&) = delete;
    render_scope(render_scope&&)      = delete;
    render_scope& operator=(const render_scope&) = delete;
    render_scope& operator=(render_scope&&) = delete;

    /// enter child window scope
    /// \param dl draw list to write onto
    /// \returns draw list written in child window
    [[nodiscard]]
    auto enter_child(const window*, ui::draw_list dl) -> ui::draw_list;

    /// enter child window scope with current draw list
    void enter_child(const window* w);

  public:
    /// Get current draw list
    auto draw_list() -> ui::draw_list&;
  };

} // namespace yave::ui