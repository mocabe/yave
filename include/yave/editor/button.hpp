//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/wm/window.hpp>
#include <yave/editor/render_context.hpp>

#include <iostream>

namespace yave::editor {

  /// push button
  class button : public wm::window
  {
  public:
    button(const std::u8string& name, const fvec2& pos, const fvec2& size)
      : window(name, pos, size)
    {
    }

    virtual void render(
      editor::data_context& dc,
      editor::view_context& vc,
      editor::render_context& rc) const override
    {
      auto drawer = rc.create_window_drawer(this);
      {
        drawer.add_rect_filled({0, 0}, size(), fvec4 {0, 1, 0, 1});
      }
    }

    virtual void resize(const fvec2& pos, const fvec2& size) override
    {
    }

    virtual void update(
      editor::data_context& data_ctx,
      editor::view_context& view_ctx) override
    {
    }
  };
} // namespace yave::editor