//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/support/vulkan_context.hpp>

#include <imgui.h>
#include <chrono>

namespace yave {

  /// imgui application
  class imgui_glfw_vulkan
  {
  public:
    /// Ctor
    imgui_glfw_vulkan(bool enableValidation = true);

    /// Dtor
    ~imgui_glfw_vulkan();

    /// Begin ImGui frame
    void begin();

    /// End ImGui frame
    void end();

    /// Render ImGui frame with Vulkan
    void render();

  public:
    /// underlying type of ImTextureID
    using texture_type = const vk::DescriptorSet*;
    /// get texture ID from image descriptor
    [[nodiscard]] ImTextureID get_texture_id(vk::DescriptorSet& tex) const;

    // clang-format off
  public:
    [[nodiscard]] const yave::glfw_context& glfw_context() const;
    [[nodiscard]] const yave::vulkan_context& vulkan_context() const;
    [[nodiscard]] const yave::vulkan_context::window_context& window_context() const;
    // clang-format on

  public:
    [[nodiscard]] vk::Sampler font_sampler() const;
    [[nodiscard]] vk::DescriptorPool descriptor_pool() const;
    [[nodiscard]] vk::DescriptorSetLayout descriptor_set_layout() const;
    [[nodiscard]] vk::DescriptorSet descriptor_set() const;
    [[nodiscard]] vk::PipelineCache pipeline_cache() const;
    [[nodiscard]] vk::PipelineLayout pipeline_layout() const;
    [[nodiscard]] vk::Pipeline pipeline() const;
    [[nodiscard]] vk::DeviceMemory font_image_memory() const;
    [[nodiscard]] vk::Image font_image() const;
    [[nodiscard]] vk::ImageView font_image_view() const;

  private:
    imgui_glfw_vulkan(const imgui_glfw_vulkan&) = delete;

  private:
    yave::glfw_context m_glfwCtx;
    yave::vulkan_context m_vulkanCtx;
    yave::unique_glfw_window m_window;
    yave::vulkan_context::window_context m_windowCtx;

  private:
    vk::UniqueSampler m_fontSampler;
    vk::UniqueDescriptorPool m_descriptorPool;
    vk::UniqueDescriptorSetLayout m_descriptorSetLayout;
    vk::UniqueDescriptorSet m_descriptorSet;
    vk::UniquePipelineCache m_pipelineCache;
    vk::UniquePipelineLayout m_pipelineLayout;
    vk::UniquePipeline m_pipeline;
    vk::UniqueDeviceMemory m_fontImageMemory;
    vk::UniqueImage m_fontImage;
    vk::UniqueImageView m_fontImageView;

  private:
    std::vector<class ImGuiRenderBuffer> m_vertexBuffers;
    std::vector<class ImGuiRenderBuffer> m_indexBuffers;

  private:
    std::chrono::high_resolution_clock::time_point m_lastTime;
  };
}