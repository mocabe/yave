//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/lib/vulkan/offscreen_render_pass.hpp>
#include <yave/lib/vulkan/texture.hpp>

namespace yave::vulkan {

  /// image composition pipeline
  class rgba32f_offscreen_compositor
  {
    class impl;
    std::unique_ptr<impl> m_pimpl;

  public:
    rgba32f_offscreen_compositor(
      uint32_t width,
      uint32_t height,
      offscreen_context& ctx);
    ~rgba32f_offscreen_compositor() noexcept;

  public:
    /// get render pass used for this pipeline
    auto render_pass() const -> const rgba32f_offscreen_render_pass&;
    /// get render pass used for this pipeline
    auto render_pass() -> rgba32f_offscreen_render_pass&;

  public:
    /// compose image onto current frame buffer.
    /// blend factors: src=1, dst=0
    /// \requres tex should have same extent to frame buffer.
    void compose_source(const texture_data& tex);
    /// compose image onto current frame buffer.
    /// blend factors: src=1, dst=1-src
    /// \requres tex should have same extent to frame buffer.
    void compose_over(const texture_data& tex);
  };
}