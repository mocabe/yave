//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/wm/root_window.hpp>

namespace yave::wm {

  root_window::root_window()
    : window("root_window", {}, {})
  {
  }

  void root_window::render(editor::render_context& render_ctx) const
  {
    for (auto&& c : children())
      c->render(render_ctx);
  }

  void root_window::resize(const fvec2& pos, const fvec2& size)
  {
    for (auto&& c : children())
      c->resize(pos, size);
  }

  void root_window::update(
    editor::data_context& data_ctx,
    editor::view_context& view_ctx)
  {
    for (auto&& c : children())
      c->update(data_ctx, view_ctx);
  }

} // namespace yave::wm
