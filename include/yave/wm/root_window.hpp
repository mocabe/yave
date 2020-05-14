//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/wm/window.hpp>

namespace yave::wm {

  /// Application root window.
  class root_window final : public window
  {
  public:
    root_window();

  public:
    void render(editor::render_context& render_ctx) const override;

    void resize(const fvec2& pos, const fvec2& size) override;

    void update(editor::data_context& data_ctx, editor::view_context& view_ctx)
      override;
  };
} // namespace yave::wm