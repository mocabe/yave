//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/lib/vulkan/vulkan_context.hpp>
#include <yave/lib/frame_buffer/frame_buffer.hpp>

#include <boost/gil.hpp>

namespace yave::vulkan {

  /// Vulkan rendering pass for composition pipelines using 32bit float color
  /// frame.
  class rgba32f_composition_pass
  {
  public:
    using pixel_loc_type = boost::gil::rgba32f_loc_t;
    using pixel_type     = typename pixel_loc_type::value_type;

  public:
    rgba32f_composition_pass(
      uint32_t width,
      uint32_t height,
      vulkan_context& ctx);

    ~rgba32f_composition_pass() noexcept;

  public:
    /// Store frame onto existing image.
    /// Existing image will be overwritten by this operation.
    void store_frame(const boost::gil::rgba32fc_view_t& view);
    /// Load image back to image.
    void load_frame(const boost::gil::rgba32f_view_t& view) const;

  public:
    /// frame buffer extent
    auto frame_extent() const -> vk::Extent2D;
    /// frame buffer image.
    /// should be general layout, single layered image.
    /// specified usage:
    /// * color attachment
    /// * transfer src
    /// * transfer dst
    auto frame_image() const -> vk::Image;
    /// frame buffer image view
    auto frame_image_view() const -> vk::ImageView;
    /// frame buffer image format
    auto frame_format() const -> vk::Format;
    /// frame buffer device local memory.
    auto frame_memory() const -> vk::DeviceMemory;
    /// frame buffer
    auto frame_buffer() const -> vk::Framebuffer;
    /// command pool
    auto command_pool() const -> vk::CommandPool;
    /// command buffer which will be used for begin_draw() and begin_end()
    auto command_buffer() const -> vk::CommandBuffer;
    /// render pass
    auto render_pass() const -> vk::RenderPass;

  public:
    /// Start command buffer to record draw call
    auto begin_draw() -> vk::CommandBuffer;
    /// End command buffer and send to GPU
    void end_draw();
    /// Wait render
    void wait_draw();

  private:
    class impl;
    std::unique_ptr<impl> m_pimpl;
  };

} // namespace yave::vulkan