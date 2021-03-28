//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/ui/root.hpp>
#include <yave/ui/window_manager.hpp>
#include <yave/ui/view_context.hpp>

namespace yave::ui {

  root::root(ui::window_manager& wm, passkey<ui::window_manager>)
    : m_wm {wm}
  {
  }

  bool root::should_close() const
  {
    return children().empty();
  }

  void root::layout(layout_scope) const
  {
    assert(false);
  }

  void root::render(render_scope) const
  {
    assert(false);
  }

  auto root::add_viewport(std::u8string name, ui::size size) -> ui::viewport*
  {
    assert(is_registered());

    auto v = std::make_unique<ui::viewport>(
      m_wm,
      m_wm.view_ctx().layout_ctx(),
      m_wm.view_ctx().render_ctx(),
      std::move(name),
      size,
      passkey<root>());

    auto ret = v.get();
    add_child(size_t(-1), std::move(v));
    return ret;
  }

  void root::remove_viewport(const ui::viewport* v)
  {
    assert(is_registered());
    remove_child(v);

    if (children().empty())
      m_wm.signals.on_last_window_close();
  }

} // namespace yave::ui