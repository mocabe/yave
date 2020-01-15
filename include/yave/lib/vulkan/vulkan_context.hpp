//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/lib/glfw/glfw_context.hpp>
#include <yave/lib/vulkan/vulkan_util.hpp>
#include <yave/support/enum_flag.hpp>

namespace yave::vulkan {

  class vulkan_context;
  class window_context;
  class command_recorder;

  /// RAII command recorder
  class command_recorder
  {
    friend class window_context;

  public:
    /// move
    command_recorder(command_recorder&& other) noexcept;
    /// calls end_record() and end_frame()
    ~command_recorder();

  public:
    /// get command buffer
    [[nodiscard]] vk::CommandBuffer command_buffer() const;

  private:
    command_recorder(const command_recorder&) = delete;
    /// calls begin_frame() and begin_record()
    command_recorder(window_context* window_ctx);

  private:
    window_context* m_window_ctx;
    vk::CommandBuffer m_buffer;
  };

  /// glfw/vulkan window context
  class window_context
  {
    friend class vulkan_context;
    window_context();

  public:
    /// Move window context.
    window_context(window_context&& other) noexcept;
    /// Dtor
    ~window_context() noexcept;

  public: /* data access */
    /// Get surface
    [[nodiscard]] auto surface() const -> vk::SurfaceKHR;
    /// Get swapchain
    [[nodiscard]] auto swapchain() const -> vk::SwapchainKHR;
    /// Get swapchain images
    [[nodiscard]] auto swapchain_images() const -> std::vector<vk::Image>;
    /// Get swapchain image views
    [[nodiscard]] auto swapchain_image_views() const
      -> std::vector<vk::ImageView>;
    /// Get frame buffers
    [[nodiscard]] auto frame_buffers() const -> std::vector<vk::Framebuffer>;
    /// Get current swapchain format
    [[nodiscard]] auto swapchain_format() const -> vk::SurfaceFormatKHR;
    /// Get current swapchain extent
    [[nodiscard]] auto swapchain_extent() const -> vk::Extent2D;
    /// Get render pass
    [[nodiscard]] auto render_pass() const -> vk::RenderPass;
    /// Get command pool
    [[nodiscard]] auto command_pool() const -> vk::CommandPool;
    /// Get command buffers
    [[nodiscard]] auto command_buffers() const
      -> std::vector<vk::CommandBuffer>;

  public: /* window state */
    /// Get window
    [[nodiscard]] auto window() const -> GLFWwindow*;
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
    [[nodiscard]] auto begin_record() const -> vk::CommandBuffer;

    /// End recording command.
    /// \note: command_recorder will call this automatically.
    void end_record(const vk::CommandBuffer& buffer) const;

    /// Get current swapchain index.
    /// \note: Swapchain index is given by driver every frame, and there's no
    /// guarantee about order of index. This index should only be used for
    /// specifying swapchain resources like image or frame buffer.
    /// \note: Maximum value of swapchain index is swapchain_image_count()-1
    /// or swapchain_index_count()-1;
    [[nodiscard]] auto swapchain_index() const -> uint32_t;

    /// Get number of swapchain index.
    /// \returns swapchain_image_count()
    [[nodiscard]] auto swapchain_index_count() const -> uint32_t;

    /// Get current frame index. This index can be used for resources for each
    /// render operation.
    [[nodiscard]] auto frame_index() const -> uint32_t;

    /// Get number of frame index. This value also represents maximum number
    /// of in-flight render operations.
    [[nodiscard]] auto frame_index_count() const -> uint32_t;

  public:
    /// create RAII frame recorder
    [[nodiscard]] auto new_recorder() -> command_recorder;

  public:
    /// Create single time command
    [[nodiscard]] auto single_time_command() const
      -> vulkan::single_time_command;

  private:
    class impl;
    std::unique_ptr<impl> m_pimpl;
  };
  /// Vulkan API context.
  class vulkan_context
  {
  public:
    enum class init_flags
    {
      enable_validation = 1 << 0,
      enable_logging    = 1 << 1,
    };

  private:
    static auto _init_flags() noexcept -> init_flags;

  public:
    /// Ctor.
    vulkan_context(
      glfw::glfw_context& glfw_ctx,
      init_flags flags = _init_flags());
    /// Dtor.
    ~vulkan_context() noexcept;

  public: /* instance, devices */
    /// Get instance.
    [[nodiscard]] auto instance() const -> vk::Instance;
    /// Get physical device.
    [[nodiscard]] auto physical_device() const -> vk::PhysicalDevice;
    /// Get device.
    [[nodiscard]] auto device() const -> vk::Device;
    /// Get graphics queue index
    [[nodiscard]] auto graphics_queue_family_index() const -> uint32_t;
    /// Get graphics queue
    [[nodiscard]] auto graphics_queue() const -> vk::Queue;
    /// Get present queue index
    [[nodiscard]] auto present_queue_family_index() const -> uint32_t;
    /// Get present queue
    [[nodiscard]] auto present_queue() const -> vk::Queue;

  public: /* window and surface helpers */
    /// Create new window context
    [[nodiscard]] auto create_window_context(glfw::glfw_window& window) const
      -> window_context;

  private:
    class impl;
    std::unique_ptr<impl> m_pimpl;
  };

} // namespace yave::vulkan

namespace yave {

  template <>
  struct is_enum_flag<vulkan::vulkan_context::init_flags> : std::true_type
  {
  };
}