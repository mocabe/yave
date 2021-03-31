//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/ui/vulkan_allocator.hpp>
#include <yave/ui/typedefs.hpp>
#include <yave/ui/color.hpp>

namespace yave::ui {

  /// Texture
  class texture
  {
    /// Texture image.
    /// this image has following specs:
    /// layout:  eShaderReadOnlyOptimal
    /// usage:   eSampled | eTransferDst | eTransferSrc
    /// sharing: eExclusive
    /// range:   1 layer, 1 level, color
    vulkan_image m_image;
    /// create info
    vk::ImageCreateInfo m_create_info;
    /// current queue family index
    u32 m_current_queue_family;
    /// current image layout
    vk::ImageLayout m_current_layout;
    /// image view
    vk::UniqueImageView m_image_view;

  public:
    texture(const texture&)           = delete;
    texture(texture&& other) noexcept = default;
    texture& operator=(texture&&) noexcept = default;

    /// Create new texture
    texture(
      u32 width,
      u32 height,
      vk::Format format,
      vulkan_device& device,
      vulkan_allocator& allocator);

    /// Fill texture with color
    void clear_color(const vk::ClearColorValue& color, vulkan_device& device);
  };

} // namespace yave::ui