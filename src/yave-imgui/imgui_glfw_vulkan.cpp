//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave-imgui/imgui_glfw_vulkan.hpp>
#include <imgui/imgui.h>

namespace {

  vk::UniqueFence createFence(const vk::Device& device)
  {
    vk::FenceCreateInfo info;
    info.flags = vk::FenceCreateFlagBits::eSignaled;

    auto fence = device.createFenceUnique(info);

    if (!fence)
      throw std::runtime_error("Failed to create fence");

    return fence;
  }

  vk::UniqueSemaphore createSemaphore(const vk::Device& device)
  {
    vk::SemaphoreCreateInfo info;
    info.flags = vk::SemaphoreCreateFlags();

    auto semaphore = device.createSemaphoreUnique(info);

    if (!semaphore)
      throw std::runtime_error("Failed to create semaphore");

    return semaphore;
  }

  vk::UniquePipelineLayout createPipelineLayout(const vk::Device& device)
  {
    vk::PipelineLayoutCreateInfo info;
    info.flags                  = vk::PipelineLayoutCreateFlags();
    info.setLayoutCount         = 0;
    info.pSetLayouts            = nullptr;
    info.pushConstantRangeCount = 0;
    info.pPushConstantRanges    = nullptr;

    auto cache = device.createPipelineLayoutUnique(info);

    if (!cache)
      throw std::runtime_error("Failed to create pipeline cache");

    return cache;
  }

  vk::UniqueShaderModule createShaderModule(
    const std::vector<std::byte>& code,
    const vk::Device& device)
  {
    vk::ShaderModuleCreateInfo info;
    info.flags    = vk::ShaderModuleCreateFlags();
    info.codeSize = code.size();
    info.pCode    = reinterpret_cast<const uint32_t*>(code.data());

    auto module = device.createShaderModuleUnique(info);
    if (!module)
      throw std::runtime_error("Failed to create shader module");
    return module;
  }

  std::vector<std::byte> createVertexShader()
  {
    assert(!"TODO");
  }

  std::vector<std::byte> createFragmentShader()
  {
    assert(!"TODO");
  }

  vk::UniquePipeline createPipeline(
    const vk::Extent2D& swapchainExtent,
    const vk::RenderPass& renderPass,
    const vk::PipelineCache& pipelineCache,
    const vk::PipelineLayout& pipelineLayout,
    const vk::Device& device)
  {

    /* shader stages */

    // vertex shader
    vk::UniqueShaderModule vertShaderModule =
      createShaderModule(createVertexShader(), device);

    vk::PipelineShaderStageCreateInfo vertShaderStageInfo;
    vertShaderStageInfo.flags  = vk::PipelineShaderStageCreateFlags();
    vertShaderStageInfo.stage  = vk::ShaderStageFlagBits::eVertex;
    vertShaderStageInfo.module = *vertShaderModule;
    vertShaderStageInfo.pName  = "main";
    vertShaderStageInfo.pSpecializationInfo = nullptr;

    // fragment shader
    vk::UniqueShaderModule fragShaderModule =
      createShaderModule(createFragmentShader(), device);

    vk::PipelineShaderStageCreateInfo fragShaderStageInfo;
    fragShaderStageInfo.flags  = vk::PipelineShaderStageCreateFlags();
    fragShaderStageInfo.stage  = vk::ShaderStageFlagBits::eFragment;
    fragShaderStageInfo.module = *fragShaderModule;
    fragShaderStageInfo.pName  = "main";
    fragShaderStageInfo.pSpecializationInfo = nullptr;

    // stages
    std::array shaderStages = {vertShaderStageInfo, fragShaderStageInfo};

    /* vertex input */

    // ImDrawVert
    std::array<vk::VertexInputBindingDescription, 1> vertBindingDesc;
    vertBindingDesc[0].stride    = sizeof(ImDrawVert);
    vertBindingDesc[0].inputRate = vk::VertexInputRate::eVertex;

    std::array<vk::VertexInputAttributeDescription, 3> vertAttrDesc;
    // ImDrawVert::pos
    vertAttrDesc[0].location = 0;
    vertAttrDesc[0].binding  = vertBindingDesc[0].binding;
    vertAttrDesc[0].format   = vk::Format::eR32G32Sfloat;
    vertAttrDesc[0].offset   = offsetof(ImDrawVert, pos);
    // ImDarwVert::uv
    vertAttrDesc[1].location = 1;
    vertAttrDesc[1].binding  = vertBindingDesc[0].binding;
    vertAttrDesc[1].format   = vk::Format::eR32G32Sfloat;
    vertAttrDesc[1].offset   = offsetof(ImDrawVert, uv);
    // ImDrawVert::col
    vertAttrDesc[2].location = 2;
    vertAttrDesc[2].binding  = vertBindingDesc[0].binding;
    vertAttrDesc[2].format   = vk::Format::eR8G8B8A8Unorm;
    vertAttrDesc[2].offset   = offsetof(ImDrawVert, col);

    vk::PipelineVertexInputStateCreateInfo vertInputStateInfo;
    vertInputStateInfo.flags = vk::PipelineVertexInputStateCreateFlags();
    vertInputStateInfo.vertexBindingDescriptionCount   = vertBindingDesc.size();
    vertInputStateInfo.pVertexBindingDescriptions      = vertBindingDesc.data();
    vertInputStateInfo.vertexAttributeDescriptionCount = vertBindingDesc.size();
    vertInputStateInfo.pVertexAttributeDescriptions    = vertAttrDesc.data();

    /* input assembler */

    vk::PipelineInputAssemblyStateCreateInfo inputAsmStateInfo;
    inputAsmStateInfo.flags    = vk::PipelineInputAssemblyStateCreateFlags();
    inputAsmStateInfo.topology = vk::PrimitiveTopology::eTriangleList;
    inputAsmStateInfo.primitiveRestartEnable = VK_FALSE;

    /* viewport */

    std::array<vk::Viewport, 1> viewports;
    viewports[0].x        = 0.f;
    viewports[0].y        = 0.f;
    viewports[0].width    = swapchainExtent.width;
    viewports[0].height   = swapchainExtent.height;
    viewports[0].minDepth = 0.f;
    viewports[0].maxDepth = 1.f;

    std::array<vk::Rect2D, 1> scissors;
    scissors[0].offset.x = 0.f;
    scissors[0].offset.y = 0.f;
    scissors[0].extent   = swapchainExtent;

    static_assert(viewports.size() == scissors.size());

    vk::PipelineViewportStateCreateInfo viewportStateInfo;
    viewportStateInfo.flags         = vk::PipelineViewportStateCreateFlags();
    viewportStateInfo.viewportCount = viewports.size();
    viewportStateInfo.pViewports    = viewports.data();
    viewportStateInfo.scissorCount  = scissors.size();
    viewportStateInfo.pScissors     = scissors.data();

    /* rasterization */

    vk::PipelineRasterizationStateCreateInfo rasterStateInfo;
    rasterStateInfo.flags = vk::PipelineRasterizationStateCreateFlags();
    rasterStateInfo.depthClampEnable        = VK_FALSE;
    rasterStateInfo.rasterizerDiscardEnable = VK_FALSE;
    rasterStateInfo.polygonMode             = vk::PolygonMode::eFill;
    rasterStateInfo.cullMode                = vk::CullModeFlagBits::eNone;
    rasterStateInfo.frontFace               = vk::FrontFace::eClockwise;
    rasterStateInfo.depthBiasEnable         = VK_FALSE;
    rasterStateInfo.depthBiasConstantFactor = 0.f;
    rasterStateInfo.depthBiasClamp          = 0.f;
    rasterStateInfo.depthBiasSlopeFactor    = 0.f;
    rasterStateInfo.lineWidth               = 1.f;

    /* sample */

    vk::PipelineMultisampleStateCreateInfo multisampleStateInfo;
    multisampleStateInfo.flags = vk::PipelineMultisampleStateCreateFlags();
    multisampleStateInfo.rasterizationSamples  = vk::SampleCountFlagBits::e1;
    multisampleStateInfo.sampleShadingEnable   = VK_FALSE;
    multisampleStateInfo.minSampleShading      = 1.f;
    multisampleStateInfo.pSampleMask           = nullptr;
    multisampleStateInfo.alphaToCoverageEnable = VK_FALSE;
    multisampleStateInfo.alphaToOneEnable      = VK_FALSE;

    /* color blend */

    std::array<vk::PipelineColorBlendAttachmentState, 1> colAttachments;
    colAttachments[0].colorWriteMask =
      vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
      vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
    colAttachments[0].blendEnable         = VK_TRUE;
    colAttachments[0].srcColorBlendFactor = vk::BlendFactor::eSrcAlpha;
    colAttachments[0].dstColorBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha;
    colAttachments[0].colorBlendOp        = vk::BlendOp::eAdd;
    colAttachments[0].srcAlphaBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha;
    colAttachments[0].dstAlphaBlendFactor = vk::BlendFactor::eZero;
    colAttachments[0].alphaBlendOp        = vk::BlendOp::eAdd;

    vk::PipelineDepthStencilStateCreateInfo depthStencilStateInfo;
    depthStencilStateInfo.flags = vk::PipelineDepthStencilStateCreateFlags();
    depthStencilStateInfo.depthTestEnable       = VK_FALSE;
    depthStencilStateInfo.depthWriteEnable      = VK_FALSE;
    depthStencilStateInfo.depthCompareOp        = vk::CompareOp::eLess;
    depthStencilStateInfo.depthBoundsTestEnable = VK_FALSE;
    depthStencilStateInfo.stencilTestEnable     = VK_FALSE;
    depthStencilStateInfo.front                 = vk::StencilOpState {};
    depthStencilStateInfo.back                  = vk::StencilOpState {};
    depthStencilStateInfo.minDepthBounds        = 0.f;
    depthStencilStateInfo.maxDepthBounds        = 1.f;

    vk::PipelineColorBlendStateCreateInfo colorBlendStateInfo;
    colorBlendStateInfo.flags = vk::PipelineColorBlendStateCreateFlags();
    colorBlendStateInfo.logicOpEnable     = VK_FALSE;
    colorBlendStateInfo.logicOp           = vk::LogicOp::eCopy;
    colorBlendStateInfo.attachmentCount   = colAttachments.size();
    colorBlendStateInfo.pAttachments      = colAttachments.data();
    colorBlendStateInfo.blendConstants[0] = 0.f;
    colorBlendStateInfo.blendConstants[1] = 0.f;
    colorBlendStateInfo.blendConstants[2] = 0.f;
    colorBlendStateInfo.blendConstants[3] = 0.f;

    /* dynamic state */

    std::array dynamicStates = {vk::DynamicState::eViewport,
                                vk::DynamicState::eScissor,
                                vk::DynamicState::eLineWidth};

    vk::PipelineDynamicStateCreateInfo dynamicStateInfo;
    dynamicStateInfo.flags             = vk::PipelineDynamicStateCreateFlags();
    dynamicStateInfo.dynamicStateCount = dynamicStates.size();
    dynamicStateInfo.pDynamicStates    = dynamicStates.data();

    /* pipeline */

    vk::GraphicsPipelineCreateInfo info;
    info.flags               = vk::PipelineCreateFlags();
    info.stageCount          = shaderStages.size();
    info.pStages             = shaderStages.data();
    info.pVertexInputState   = &vertInputStateInfo;
    info.pInputAssemblyState = &inputAsmStateInfo;
    info.pViewportState      = &viewportStateInfo;
    info.pRasterizationState = &rasterStateInfo;
    info.pMultisampleState   = &multisampleStateInfo;
    info.pColorBlendState    = &colorBlendStateInfo;
    info.pDynamicState       = &dynamicStateInfo;
    info.renderPass          = renderPass;
    info.layout              = pipelineLayout;

    auto pipeline = device.createGraphicsPipelineUnique(pipelineCache, info);

    if (!pipeline)
      throw std::runtime_error("Failed to create graphic pipeline");

    return pipeline;
  }

} // namespace

namespace yave {

  imgui_glfw_vulkan::imgui_glfw_vulkan(bool enableValidation)
    : m_glfwCtx {}
    , m_vulkanCtx {m_glfwCtx, enableValidation}
    , m_window {m_glfwCtx.create_window(1280, 720, "imgui_glfw_vulkan")}
    , m_windowCtx {m_vulkanCtx.create_window_context(m_window)}
  {
    m_fence                   = createFence(m_vulkanCtx.device());
    m_imageAcquiredSemaphore  = createSemaphore(m_vulkanCtx.device());
    m_renderCompleteSemaphore = createSemaphore(m_vulkanCtx.device());
    m_pipelineLayout          = createPipelineLayout(m_vulkanCtx.device());
    m_pipeline                = createPipeline(
      m_windowCtx.swapchain_extent(),
      m_windowCtx.render_pass(),
      m_windowCtx.pipeline_cache(),
      m_pipelineLayout.get(),
      m_vulkanCtx.device());
  }

} // namespace yave