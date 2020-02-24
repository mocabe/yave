//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/lib/glfw/glfw_context.hpp>
#include <yave/lib/vulkan/vulkan_util.hpp>
#include <yave/support/enum_flag.hpp>

namespace yave::vulkan {

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

  private:
    class impl;
    std::unique_ptr<impl> m_pimpl;
  };

} // namespace yave::vulkan

YAVE_DECL_ENUM_FLAG(yave::vulkan::vulkan_context::init_flags);