//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/lib/vulkan/vulkan_context.hpp>

namespace yave::vulkan {

  /// offscreen vulkan device context
  class offscreen_context
  {
  public:
    offscreen_context(vulkan_context& ctx);
    ~offscreen_context() noexcept;
    offscreen_context(offscreen_context&&) noexcept = default;
    offscreen_context& operator=(offscreen_context&&) noexcept = default;

  public:
    /// Get parent
    [[nodiscard]] auto vulkan_ctx() -> vulkan_context&;

  public:
    /// Get logical device
    [[nodiscard]] auto device() const -> vk::Device;
    /// Get queue index
    [[nodiscard]] auto graphics_queue_index() const -> uint32_t;
    /// Get queue
    [[nodiscard]] auto graphics_queue() const -> vk::Queue;

  private:
    class impl;
    std::unique_ptr<impl> m_pimpl;
  };
} // namespace yave::vulkan
