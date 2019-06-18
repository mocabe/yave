//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/support/glfw_context.hpp>
#include <vulkan/vulkan.hpp>

namespace yave::vulkan {

  /// single time command
  class single_time_command
  {
  public:
    /// Create command buffer and start recording.
    single_time_command(
      const vk::Device& device,
      const vk::Queue& queue,
      const vk::CommandPool& pool);
    /// End command buffer and submit.
    ~single_time_command();
    /// Move single time command.
    single_time_command(single_time_command&& other) noexcept;
    /// Get recording command buffer.
    [[nodiscard]] vk::CommandBuffer command_buffer() const;

  private:
    single_time_command(const single_time_command&) = delete;
    vk::Device m_device;
    vk::Queue m_queue;
    vk::CommandPool m_pool;
    vk::UniqueCommandBuffer m_buffer;
    vk::UniqueFence m_fence;
  };

  /// Vulkan API context.
  class vulkan_context
  {
  public:
    /// Ctor.
    vulkan_context(
      glfw::glfw_context& glfw_ctx,
      bool enable_validation_layer = true);
    /// Dtor.
    ~vulkan_context() noexcept;

    /* instance, devices */

    /// Get instance.
    [[nodiscard]] vk::Instance instance() const;
    /// Get physical device.
    [[nodiscard]] vk::PhysicalDevice physical_device() const;
    /// Get device.
    [[nodiscard]] vk::Device device() const;
    /// Get graphics queue index
    [[nodiscard]] uint32_t graphics_queue_family_index() const;
    /// Get graphics queue
    [[nodiscard]] vk::Queue graphics_queue() const;
    /// Get present queue index
    [[nodiscard]] uint32_t present_queue_family_index() const;
    /// Get present queue
    [[nodiscard]] vk::Queue present_queue() const;

    /* window and surface helpers */

    /// glfw/vulkan window context
    class window_context
    {
      friend class vulkan_context;
      window_context();

    public: /* data access */
      /// Move window context.
      window_context(window_context&& other) noexcept;
      /// Dtor
      ~window_context() noexcept;
      /// Get surface
      [[nodiscard]] vk::SurfaceKHR surface() const;
      /// Get swapchain
      [[nodiscard]] vk::SwapchainKHR swapchain() const;
      /// Get swapchain images
      [[nodiscard]] std::vector<vk::Image> swapchain_images() const;
      /// Get swapchain image views
      [[nodiscard]] std::vector<vk::ImageView> swapchain_image_views() const;
      /// Get frame buffers
      [[nodiscard]] std::vector<vk::Framebuffer> frame_buffers() const;
      /// Get current swapchain format
      [[nodiscard]] vk::SurfaceFormatKHR swapchain_format() const;
      /// Get current swapchain extent
      [[nodiscard]] vk::Extent2D swapchain_extent() const;
      /// Get render pass
      [[nodiscard]] vk::RenderPass render_pass() const;
      /// Get command pool
      [[nodiscard]] vk::CommandPool command_pool() const;
      /// Get command buffers
      [[nodiscard]] std::vector<vk::CommandBuffer> command_buffers() const;

    public: /* window state */
      /// Get window
      [[nodiscard]] GLFWwindow* window() const;
      /// Check if frame buffer is resized.
      [[nodiscard]] bool resized() const;
      /// Check if widnow should close.
      [[nodiscard]] bool should_close() const;

    public: /* window settings */
      /// Set clear color
      void set_clear_color(float r, float g, float b, float a);

    public: /* framebuffer update */
      /// Rebuild resources related to frame buffer.
      /// \note: Not internally synchronized.
      /// \note: Does not poll glfw event. Use resized() to check resize event.
      /// \note: This function will block rendering thread when frame buffer
      /// size is zero. Happens when window is minimized on Windows platform.
      void rebuild_frame_buffers();

    public: /* render operations */
      /// Update frame/semaphore indicies and acquire new image.
      /// \note: command_recorder will call this automatically.
      void begin_frame();
      /// Submit current frame buffer.
      /// \note: command_recorder will call this automatically.
      void end_frame() const;
      /// Begin recording command.
      /// \note: command_recorder will call this automatically.
      [[nodiscard]] vk::CommandBuffer begin_record() const;
      /// End recording command.
      /// \note: command_recorder will call this automatically.
      void end_record(const vk::CommandBuffer& buffer) const;
      /// Get current swapchain index.
      /// \note: Swapchain index is given by driver every frame, and there's no
      /// guarantee about order of index. This index should only be used for
      /// specifying swapchain resources like image or frame buffer.
      /// \note: Maximum value of swapchain index is swapchain_image_count()-1
      /// or swapchain_index_count()-1;
      [[nodiscard]] uint32_t swapchain_index() const;
      /// Get number of swapchain index.
      /// \returns swapchain_image_count()
      [[nodiscard]] uint32_t swapchain_index_count() const;
      /// Get current frame index. This index can be used for resources for each
      /// render operation.
      [[nodiscard]] uint32_t frame_index() const;
      /// Get number of frame index. This value also represents maximum number
      /// of in-flight render operations.
      [[nodiscard]] uint32_t frame_index_count() const;

    public:
      /// RAII frame context
      class command_recorder
      {
      public:
        command_recorder(command_recorder&& other) noexcept;
        /// calls end_record() and end_frame()
        ~command_recorder();
        [[nodiscard]] vk::CommandBuffer command_buffer() const;

      private:
        command_recorder(const command_recorder&) = delete;
        /// calls begin_frame() and begin_record()
        command_recorder(window_context* window_ctx);

      private:
        friend class window_context;
        window_context* m_window_ctx;
        vk::CommandBuffer m_buffer;
      };

      /// create RAII frame recorder
      [[nodiscard]] command_recorder new_recorder();

    public:
      /// Create single time command
      [[nodiscard]] vulkan::single_time_command single_time_command() const;

    private:
      class impl;
      std::unique_ptr<impl> m_pimpl;
    };

    /// Create new window context
    [[nodiscard]] vulkan_context::window_context
      create_window_context(glfw::unique_glfw_window& window) const;

  private:
    class impl;
    std::unique_ptr<impl> m_pimpl;
  };
}