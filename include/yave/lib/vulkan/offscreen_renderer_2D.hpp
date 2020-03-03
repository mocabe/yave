//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/lib/vulkan/offscreen_render_pass.hpp>
#include <yave/lib/vulkan/draw2d.hpp>

#include <glm/glm.hpp>

namespace yave::vulkan {

  /// Basic 2D pipeline using Vulkan.
  /// Not thread safe.
  class rgba32f_offscreen_renderer_2D
  {
  public:
    /// Ctor
    rgba32f_offscreen_renderer_2D(
      uint32_t width,
      uint32_t height,
      vulkan_context& ctx);
    /// Dtor
    ~rgba32f_offscreen_renderer_2D() noexcept;

  public:
    /// width
    auto width() const noexcept -> uint32_t;
    /// height
    auto height() const noexcept -> uint32_t;
    /// format
    auto format() const noexcept -> image_format;

  public:
    /// Store frame onto existing image.
    /// Existing image will be overwritten by this operation.
    void store_frame(const boost::gil::rgba32fc_view_t& view);
    /// Load image back to image.
    void load_frame(const boost::gil::rgba32f_view_t& view) const;

  public:
    /// render commands
    void render(const draw2d_data& draw_data);

  public:
    /// Add new texture
    [[nodiscard]] auto add_texture(const boost::gil::rgba32fc_view_t& view)
      -> draw2d_tex;
    /// Remove texture
    void remove_texture(const draw2d_tex& tex);

  private:
    class impl;
    std::unique_ptr<impl> m_pimpl;
  };
}