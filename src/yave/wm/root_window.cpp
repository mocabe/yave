//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/wm/root_window.hpp>

namespace yave::wm {

  root_window::root_window()
    : window("root_window")
  {
  }

  void root_window::draw(
    editor::data_context& data_ctx,
    editor::view_context& view_ctx) const
  {
    for (auto&& c : children())
      c->draw(data_ctx, view_ctx);
  }

  void root_window::update(
    editor::data_context& data_ctx,
    editor::view_context& view_ctx)
  {
    for (auto&& c : children())
      as_mut_child(c)->update(data_ctx, view_ctx);
  }

  auto root_window::add_window(std::unique_ptr<window>&& win) -> window*
  {
    auto ret = win.get();
    add_any_window(children().end(), std::move(win));
    return ret;
  }

  void root_window::remove_window(uid id)
  {
    remove_any_window(id);
  }
}