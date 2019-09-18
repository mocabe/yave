//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/lib/vulkan/vulkan_context.hpp>

#include <imgui.h>
#include <imgui_freetype.h> // use freetype for font rendering
#include <chrono>

namespace yave::imgui {

  // clang-format off

  /// imgui application
  class imgui_context
  {
  public:
    /// Ctor
    imgui_context(bool enableValidation = true);
    /// Dtor
    ~imgui_context();
  
  public:
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
    [[nodiscard]] auto get_texture_id(vk::DescriptorSet& tex) const -> ImTextureID;

    // clang-format off
  public:
    [[nodiscard]] auto glfw_context() const   -> const glfw::glfw_context&;
    [[nodiscard]] auto vulkan_context() const -> const vulkan::vulkan_context&;
    [[nodiscard]] auto window_context() const -> const vulkan::vulkan_context::window_context&;
    // clang-format on

  public:
    [[nodiscard]] auto font_sampler() const          -> vk::Sampler;
    [[nodiscard]] auto descriptor_pool() const       -> vk::DescriptorPool;
    [[nodiscard]] auto descriptor_set_layout() const -> vk::DescriptorSetLayout;
    [[nodiscard]] auto descriptor_set() const        -> vk::DescriptorSet;
    [[nodiscard]] auto pipeline_cache() const        -> vk::PipelineCache;
    [[nodiscard]] auto pipeline_layout() const       -> vk::PipelineLayout;
    [[nodiscard]] auto pipeline() const              -> vk::Pipeline;
    [[nodiscard]] auto font_image_memory() const     -> vk::DeviceMemory;
    [[nodiscard]] auto font_image() const            -> vk::Image;
    [[nodiscard]] auto font_image_view() const       -> vk::ImageView;

  private:
    imgui_context(const imgui_context&) = delete;

  private:
    glfw::glfw_context                     m_glfwCtx;
    vulkan::vulkan_context                 m_vulkanCtx;
    glfw::unique_glfw_window               m_window;
    vulkan::vulkan_context::window_context m_windowCtx;

  private:
    vk::UniqueSampler             m_fontSampler;
    vk::UniqueDescriptorPool      m_descriptorPool;
    vk::UniqueDescriptorSetLayout m_descriptorSetLayout;
    vk::UniqueDescriptorSet       m_descriptorSet;
    vk::UniquePipelineCache       m_pipelineCache;
    vk::UniquePipelineLayout      m_pipelineLayout;
    vk::UniquePipeline            m_pipeline;
    vk::UniqueDeviceMemory        m_fontImageMemory;
    vk::UniqueImage               m_fontImage;
    vk::UniqueImageView           m_fontImageView;

  private:
    std::vector<class ImGuiRenderBuffer> m_vertexBuffers;
    std::vector<class ImGuiRenderBuffer> m_indexBuffers;

  private:
    std::chrono::high_resolution_clock::time_point m_lastTime;
  };

  // clang-format on
}