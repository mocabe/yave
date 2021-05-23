//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/ui/viewport.hpp>
#include <yave/ui/native_window.hpp>
#include <yave/ui/window_manager.hpp>
#include <yave/ui/layout_context.hpp>
#include <yave/ui/render_context.hpp>
#include <yave/ui/window_event_controller.hpp>

#include <yave/support/log.hpp>

YAVE_DECL_LOCAL_LOGGER(ui::viewport)

namespace yave::ui {

  viewport::viewport(
    ui::window_manager& wm,
    ui::layout_context& lctx,
    ui::render_context& rctx,
    std::u8string name,
    ui::size size,
    passkey<root>&&)
    : m_lctx {lctx}
    , m_rctx {rctx}
    , m_nw {std::make_unique<ui::native_window>(
        wm.view_ctx(),
        std::move(name),
        size,
        passkey<viewport>())}
  {
    // setup native window
    m_nw->set_viewport(*this, {});

    // init viewport
    m_lctx.init_viewport(*this, {});
    m_rctx.init_viewport(*this, {});

    // init window controller
    {
      auto c              = ui::make_unique<controllers::window>();
      m_window_controller = c.get();
      add_controller(std::move(c));
    }
  }

  viewport::~viewport() noexcept = default;

  auto viewport::native_window() -> ui::native_window&
  {
    return *m_nw;
  }

  auto viewport::native_window() const -> const ui::native_window&
  {
    return *m_nw;
  }

  void viewport::layout(layout_scope ctx) const
  {
    auto& native = native_window();
    // set new window size
    ctx.set_size(native.size());
    ctx.set_offset(native.pos());

    log_info("new layout: {}, {}", native.size().w, native.size().h);

    // use tight constraits for window size
    for (auto&& c : children()) {
      if (c.visible())
        ctx.enter_child(c, {native.size()});
    }
  }

  void viewport::render(render_scope ctx) const
  {
    for (auto&& c : children()) {
      if (c.visible())
        ctx.enter_child(c);
    }
  }

  bool viewport::has_child() const
  {
    return !children().empty();
  }

  auto viewport::set_child(unique<window> c) -> window&
  {
    remove_child();
    return window::add_child(0, std::move(c));
  }

  auto viewport::get_child() -> window&
  {
    assert(has_child());
    return children().front();
  }

  void viewport::remove_child()
  {
    if (has_child())
      window::remove_child(children().front());
  }

} // namespace yave::ui