//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include "yave/ui/window_events.hpp"
#include <yave/ui/viewport.hpp>
#include <yave/ui/native_window.hpp>
#include <yave/ui/window_manager.hpp>
#include <yave/ui/layout_context.hpp>
#include <yave/ui/render_context.hpp>
#include <yave/ui/window_events.hpp>

#include <yave/support/log.hpp>

YAVE_DECL_LOCAL_LOGGER(ui::viewport)

namespace yave::ui {

  viewport::viewport(
    ui::window_manager& wm,
    ui::layout_context& lctx,
    ui::render_context& rctx,
    std::u8string name,
    ui::size size,
    passkey<root>)
    : m_lctx {lctx}
    , m_rctx {rctx}
    , m_nw {std::make_unique<native_window>(
        wm.view_ctx(),
        std::move(name),
        size,
        passkey<viewport>())}
  {
    // setup native window
    m_nw->set_viewport(this, {});

    // init viewport
    m_lctx.init_viewport(this, {});
    m_rctx.init_viewport(this, {});

    // init close controller
    {
      auto closeController = std::make_unique<controllers::close>();
      m_close_controller   = closeController.get();
      add_controller(std::move(closeController));
    }
  }

  viewport::~viewport() noexcept = default;

  auto viewport::get_native() -> native_window*
  {
    return m_nw.get();
  }

  auto viewport::get_native() const -> const native_window*
  {
    return m_nw.get();
  }

  void viewport::layout(layout_scope ctx) const
  {
    auto native = get_native();
    // set new window size
    ctx.set_size(native->size());
    ctx.set_offset(native->pos());

    log_info("new layout: {}, {}", native->size().w, native->size().h);

    // use tight constraits for window size
    for (auto&& c : children())
      ctx.enter_child(c, {native->size()});
  }

  void viewport::render(render_scope ctx) const
  {
    for (auto&& c : children())
      ctx.enter_child(c);
  }

} // namespace yave::ui