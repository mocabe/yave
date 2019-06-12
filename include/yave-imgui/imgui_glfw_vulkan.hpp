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

  private:
    yave::glfw_context m_glfwCtx;
    yave::vulkan_context m_vulkanCtx;
    yave::unique_glfw_window m_window;
    yave::vulkan_context::window_context m_windowCtx;

  private:
    vk::UniquePipelineLayout m_pipelineLayout;
    vk::UniquePipelineCache m_pipelineCache;
    vk::UniquePipeline m_pipeline;
    vk::UniqueFence m_fence;
    vk::UniqueDescriptorPool m_descriptorPool;
  };
}