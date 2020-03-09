//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/lib/vulkan/vulkan_context.hpp>
#include <yave/lib/vulkan/offscreen_context.hpp>
#include <yave/lib/frame_buffer/frame_buffer.hpp>

#include <boost/gil.hpp>

namespace yave::vulkan {

  /// Vulkan offscreen rendering pass for composition pipelines using 32bit
  /// float color frame.
  class rgba32f_offscreen_render_pass
  {
  public:
    using pixel_loc_type = boost::gil::rgba32f_loc_t;
    using pixel_type     = typename pixel_loc_type::value_type;

  public:
    rgba32f_offscreen_render_pass(
      uint32_t width,
      uint32_t height,
      offscreen_context& ctx);

    ~rgba32f_offscreen_render_pass() noexcept;

  public:
    /// get parent
    [[nodiscard]] auto offscreen_ctx() -> offscreen_context&;

  public:
    /// width
    [[nodiscard]] auto width() const noexcept -> uint32_t;
    /// height
    [[nodiscard]] auto height() const noexcept -> uint32_t;
    /// format
    [[nodiscard]] auto format() const noexcept -> image_format;

  public:
    /// Store frame onto existing image.
    /// Existing image will be overwritten by this operation.
    void store_frame(const boost::gil::rgba32fc_view_t& view);
    /// Load image back to image.
    void load_frame(const boost::gil::rgba32f_view_t& view) const;

  public:
    /// frame buffer extent
    [[nodiscard]] auto frame_extent() const noexcept -> vk::Extent2D;
    /// frame buffer image.
    /// should be general layout, single layered image.
    /// specified usage:
    /// * color attachment
    /// * transfer src
    /// * transfer dst
    [[nodiscard]] auto frame_image() const noexcept -> vk::Image;
    /// frame buffer image view
    [[nodiscard]] auto frame_image_view() const noexcept -> vk::ImageView;
    /// frame buffer image format
    [[nodiscard]] auto frame_format() const noexcept -> vk::Format;
    /// frame buffer device local memory.
    [[nodiscard]] auto frame_memory() const noexcept -> vk::DeviceMemory;
    /// frame buffer
    [[nodiscard]] auto frame_buffer() const noexcept -> vk::Framebuffer;
    /// command pool
    [[nodiscard]] auto command_pool() const noexcept -> vk::CommandPool;
    /// command buffer which will be used for begin_draw() and begin_end()
    [[nodiscard]] auto command_buffer() const noexcept -> vk::CommandBuffer;
    /// render pass
    [[nodiscard]] auto render_pass() const noexcept -> vk::RenderPass;

  public:
    /// Start command buffer to record draw call
    [[nodiscard]] auto begin_pass() -> vk::CommandBuffer;
    /// End command buffer and send to GPU
    void end_pass();
    /// Wait render
    void wait_draw();

  private:
    class impl;
    std::unique_ptr<impl> m_pimpl;
  };

} // namespace yave::vulkan