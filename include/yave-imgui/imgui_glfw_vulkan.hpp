//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/support/vulkan_context.hpp>

namespace yave {

  /// imgui application
  class imgui_glfw_vulkan
  {
  public:
    imgui_glfw_vulkan(bool enableValidation = true);
    ~imgui_glfw_vulkan();

    void run();

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
    std::vector<struct ImGuiRenderBuffer> vertexBuffers;
    std::vector<struct ImGuiRenderBuffer> indexBuffers;
  };
}