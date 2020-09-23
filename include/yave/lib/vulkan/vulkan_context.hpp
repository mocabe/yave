//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/lib/vulkan/vulkan_util.hpp>

namespace yave::vulkan {

  /// Vulkan API context.
  class vulkan_context
  {
    class impl;
    std::unique_ptr<impl> m_pimpl;

  public:
    struct create_info
    {
      /// validation layer
      bool enable_validation = true;
    };

  private:
    static auto _init_create_info() -> create_info
    {
      return create_info();
    }

  public:
    /// Ctor.
    vulkan_context(create_info info = _init_create_info());
    /// Dtor.
    ~vulkan_context() noexcept;
    /// Deleted
    vulkan_context(const vulkan_context&) = delete;

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
  };

} // namespace yave::vulkan