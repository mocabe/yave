//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/wm/viewport_graphics.hpp>
#include <yave/editor/render_context.hpp>

namespace yave::wm {
  class viewport_graphics::impl
  {
  public:
    // per window render context
    editor::render_context render_ctx;

  public:
    impl(glfw::glfw_window& gw)
      : render_ctx {gw}
    {
    }
  };

  viewport_graphics::viewport_graphics(glfw::glfw_window& glfw_win)
    : m_pimpl(std::make_unique<impl>(glfw_win))
  {
  }

  viewport_graphics::~viewport_graphics() noexcept = default;

  auto viewport_graphics::render_context() -> editor::render_context&
  {
    return m_pimpl->render_ctx;
  }

} // namespace yave::wm