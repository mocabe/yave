//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/ui/vulkan_device.hpp>
#include <yave/ui/typedefs.hpp>

namespace yave::ui {

  class render_context;
  class native_window;

  /// Vulkan window surface
  class vulkan_surface
  {
    render_context& m_rctx;
    native_window& m_win;

    // clang-format off

    vk::UniqueSurfaceKHR             m_surface;
    vk::UniqueSwapchainKHR           m_swapchain;
    vk::SurfaceFormatKHR             m_swapchain_format;
    vk::PresentModeKHR               m_swapchain_present_mode;
    vk::Extent2D                     m_swapchain_extent;
    u32                              m_swapchain_image_count;
    std::vector<vk::Image>           m_swapchain_images; // owned by swapchain
    std::vector<vk::UniqueImageView> m_swapchain_image_views;

    vk::UniqueRenderPass    m_render_pass;
    vk::UniqueCommandPool   m_command_pool;

    std::vector<vk::UniqueCommandBuffer> m_command_buffers;
    std::vector<vk::UniqueFramebuffer>   m_frame_buffers;
    std::vector<vk::UniqueSemaphore>     m_acquire_semaphores;
    std::vector<vk::UniqueSemaphore>     m_complete_semaphores;
    std::vector<vk::UniqueFence>         m_in_flight_fences;
    vk::UniqueFence                      m_acquire_fence;
    vk::ClearColorValue                  m_clear_color;

    u32 m_image_index = 0;
    u32 m_frame_index = 0;

    // clang-format on

  public:
    vulkan_surface(render_context& rctx, native_window& win);
    ~vulkan_surface() noexcept;

    /// Set clear color
    void set_clear_color(float r, float g, float b, float a);

    auto swapchain_extent() const
    {
      return m_swapchain_extent;
    }

    auto render_pass() const
    {
      return m_render_pass.get();
    }

  public:
    /// Surface size is outdated
    bool rebuild_required() const;

    /// Rebuild resources related to frame buffer.
    void rebuild();

    /// begin new frame
    void begin_frame();

    /// end current frame
    void end_frame();

    /// Begin recording command.
    /// \note should be called between begin_frame() and end_frame()
    auto begin_record() -> vk::CommandBuffer;

    /// End recording command.
    /// \note should be called between begin_frame() and end_frame()
    void end_record(const vk::CommandBuffer& buffer);

    /// Get current swapchain index.
    /// \note: Swapchain index is given by driver every frame, and there's no
    /// guarantee about order of index. This index should only be used for
    /// specifying swapchain resources like image or frame buffer.
    /// \note: Maximum value of swapchain index is swapchain_image_count()-1
    /// or swapchain_index_count()-1;
    auto swapchain_index() const -> u32;

    /// Get number of swapchain index.
    /// \returns swapchain_image_count()
    auto swapchain_index_count() const -> u32;

    /// Get current frame index. This index can be used for resources for each
    /// render operation.
    auto frame_index() const -> u32;

    /// Get number of frame index. This value also represents maximum number
    /// of in-flight render operations.
    auto frame_index_count() const -> u32;
  };
} // namespace yave::ui