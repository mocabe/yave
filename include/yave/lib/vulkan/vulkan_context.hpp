//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

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
    vulkan_context(init_flags flags = _init_flags());
    /// Dtor.
    ~vulkan_context() noexcept;

  public: /* instance, devices */
    /// Get instance.
    [[nodiscard]] auto instance() const -> vk::Instance;
    /// Get physical device.
    [[nodiscard]] auto physical_device() const -> vk::PhysicalDevice;

  public:
    [[nodiscard]] auto physical_device_properties() const
      -> const vk::PhysicalDeviceProperties&;
    [[nodiscard]] auto physical_device_queue_properties() const
      -> const std::vector<vk::QueueFamilyProperties>&;

  public: /* utility */
    /// validate extensions
    [[nodiscard]] bool check_physical_device_extension_support(
      const std::vector<std::string>& extensions) const;
    /// validate extensions
    [[nodiscard]] bool check_physical_device_layer_support(
      const std::vector<std::string>& layers) const;
    /// create logical deviec
    [[nodiscard]] auto create_device(
      const std::vector<uint32_t> queue_indicies,
      const std::vector<std::string>& extensions,
      const std::vector<std::string>& layers) -> vk::UniqueDevice;

  private:
    class impl;
    std::unique_ptr<impl> m_pimpl;
  };

} // namespace yave::vulkan

YAVE_DECL_ENUM_FLAG(yave::vulkan::vulkan_context::init_flags)