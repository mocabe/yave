//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/ui/vulkan_allocator.hpp>
#include <yave/ui/typedefs.hpp>
#include <yave/ui/color.hpp>

namespace yave::ui {

  /// Texture data
  class texture
  {
    /// allocator ref
    vulkan_allocator& m_allocator;
    /// Buffer is allocated on GPU.
    /// The image has following specs:
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
    // Non-copiable and movable. Expected to be used with smart pointers.
    texture(const texture&)           = delete;
    texture(texture&& other) noexcept = delete;

    /// Create new texture
    texture(
      u32 width,
      u32 height,
      vk::Format format,
      vulkan_allocator& allocator);

    /// Fill texture with color
    void clear_color(const color& color);

  public:
    auto image_view() const
    {
      return m_image_view.get();
    }

    auto format() const
    {
      return m_create_info.format;
    }

    auto width() const
    {
      return m_create_info.extent.width;
    }

    auto height() const
    {
      return m_create_info.extent.height;
    }

    auto byte_size() const
    {
      return vulkan::format_texel_size(format());
    }
  };

} // namespace yave::ui