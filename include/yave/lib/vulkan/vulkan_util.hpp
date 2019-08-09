//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/lib/vulkan/vulkan_context.hpp>

#include <selene/img/typed/ImageView.hpp>
#include <selene/img/pixel/PixelTypeAliases.hpp>

namespace yave::vulkan {

  /// Upload image to GPU.
  auto upload_image(
    const sln::ConstantImageView<sln::PixelRGBA_8u>& inView,
    const vk::CommandPool& commandPool,
    const vk::Queue queue,
    const vk::PhysicalDevice& physicalDevice,
    const vk::Device& device)
    -> std::tuple<vk::UniqueImage, vk::UniqueImageView, vk::UniqueDeviceMemory>;

  /// Create descriptor set from image view.
  auto create_descriptor(
    const vk::ImageView& image,
    const vk::DescriptorSetLayout& layout,
    const vk::DescriptorPool& pool,
    const vk::Device& device) -> vk::UniqueDescriptorSet;

} // namespace yave