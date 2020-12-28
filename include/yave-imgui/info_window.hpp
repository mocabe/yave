//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/editor/data_context.hpp>
#include <yave/editor/view_context.hpp>
#include <yave/lib/time/time.hpp>
#include <yave/lib/image/image_format.hpp>

namespace yave::editor::imgui {

  class info_window : public wm::window
  {
  public:
    info_window();

  public:
    void update(editor::data_context& dctx, editor::view_context& vctx)
      override;

    void draw(
      const editor::data_context& dctx,
      const editor::view_context& vctx) const override;

  private:
    time m_arg_time;
    time m_arg_time_min;
    time m_arg_time_max;
    bool m_continuous;

    uint32_t m_width, m_height;
    uint32_t m_fps;
    image_format m_format;
  };
}