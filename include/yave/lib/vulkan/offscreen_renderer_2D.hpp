//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/lib/vulkan/offscreen_render_pass.hpp>
#include <yave/lib/vulkan/texture.hpp>
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
    /// Get default texture
    [[nodiscard]] auto default_texture() const -> draw2d_tex;

  public: /* texture utility */
    /// Add texture data
    [[nodiscard]] auto create_texture(
      const vk::Extent2D& extent,
      const vk::Format& format) -> vulkan::texture_data;

    /// Update texture data
    void write_texture(
      vulkan::texture_data& tex,
      const uint8_t* srcData,
      const vk::DeviceSize& srcSize);

    /// Clear texture
    void clear_texture(
      vulkan::texture_data& tex,
      const vk::ClearColorValue& color);

  public: /* texture management */
    /// Bind texture to context.
    /// User should manually unbind textures.
    [[nodiscard]] auto bind_texture(const vulkan::texture_data& tex)
      -> draw2d_tex;
    /// Unbind texture to context
    void unbind_texture(const vulkan::texture_data& tex);

  private:
    class impl;
    std::unique_ptr<impl> m_pimpl;
  };
}