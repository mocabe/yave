//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave-imgui/imgui_glfw_vulkan.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>

#include <yave/support/log.hpp>
#include <chrono>

#include "shader.hpp"

namespace {

  std::shared_ptr<spdlog::logger> g_logger;

  void init_logger()
  {
    [[maybe_unused]] static auto init_logger = [] {
      g_logger = yave::add_logger("imgui_glfw_vulkan");
      return 1;
    }();
  }

  vk::UniqueFence createFence(const vk::Device& device)
  {
    vk::FenceCreateInfo info;
    info.flags = vk::FenceCreateFlagBits::eSignaled;

    auto fence = device.createFenceUnique(info);

    if (!fence)
      throw std::runtime_error("Failed to create fence");

    return fence;
  }

  vk::UniquePipelineLayout createImGuiPipelineLayout(const vk::Device& device)
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

  vk::UniquePipelineCache createPipelineCache(const vk::Device& device)
  {
    vk::PipelineCacheCreateInfo info;
    info.flags           = vk::PipelineCacheCreateFlags();
    info.initialDataSize = 0;
    info.pInitialData    = nullptr;

    auto cache = device.createPipelineCacheUnique(info);

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

  std::vector<std::byte> createImGuiVertexShader()
  {
    std::vector<std::byte> code(sizeof(yave::vertex_shader_code));
    memcpy(code.data(), yave::vertex_shader_code, code.size());
    return code;
  }

  std::vector<std::byte> createImGuiFragmentShader()
  {
    std::vector<std::byte> code(sizeof(yave::fragment_shader_code));
    memcpy(code.data(), yave::fragment_shader_code, code.size());
    return code;
  }

  vk::UniquePipeline createImGuiPipeline(
    const vk::Extent2D& swapchainExtent,
    const vk::RenderPass& renderPass,
    const vk::PipelineCache& pipelineCache,
    const vk::PipelineLayout& pipelineLayout,
    const vk::Device& device)
  {

    /* shader stages */

    // vertex shader
    vk::UniqueShaderModule vertShaderModule =
      createShaderModule(createImGuiVertexShader(), device);

    vk::PipelineShaderStageCreateInfo vertShaderStageInfo;
    vertShaderStageInfo.flags  = vk::PipelineShaderStageCreateFlags();
    vertShaderStageInfo.stage  = vk::ShaderStageFlagBits::eVertex;
    vertShaderStageInfo.module = *vertShaderModule;
    vertShaderStageInfo.pName  = "main";
    vertShaderStageInfo.pSpecializationInfo = nullptr;

    // fragment shader
    vk::UniqueShaderModule fragShaderModule =
      createShaderModule(createImGuiFragmentShader(), device);

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

  vk::UniqueDescriptorPool createImGuiDescriptorPool(const vk::Device& device)
  {
    std::array poolSizes = {
      vk::DescriptorPoolSize {vk::DescriptorType::eSampler, 1000},
      vk::DescriptorPoolSize {vk::DescriptorType::eCombinedImageSampler, 1000},
      vk::DescriptorPoolSize {vk::DescriptorType::eSampledImage, 1000},
      vk::DescriptorPoolSize {vk::DescriptorType::eStorageImage, 1000},
      vk::DescriptorPoolSize {vk::DescriptorType::eUniformTexelBuffer, 1000},
      vk::DescriptorPoolSize {vk::DescriptorType::eStorageTexelBuffer, 1000},
      vk::DescriptorPoolSize {vk::DescriptorType::eUniformBuffer, 1000},
      vk::DescriptorPoolSize {vk::DescriptorType::eStorageBuffer, 1000},
      vk::DescriptorPoolSize {vk::DescriptorType::eUniformBufferDynamic, 1000},
      vk::DescriptorPoolSize {vk::DescriptorType::eStorageBufferDynamic, 1000},
      vk::DescriptorPoolSize {vk::DescriptorType::eInputAttachment, 1000}};

    vk::DescriptorPoolCreateInfo info;
    info.flags         = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet;
    info.maxSets       = 1000 * poolSizes.size();
    info.poolSizeCount = poolSizes.size();
    info.pPoolSizes    = poolSizes.data();

    auto pool = device.createDescriptorPoolUnique(info);

    if (!pool)
      throw std::runtime_error("Failed to create descriptor pool");

    return pool;
  }

  uint32_t findMemoryType(
    uint32_t typeFilter,
    vk::MemoryPropertyFlags properties,
    const vk::PhysicalDevice& physicalDevice)
  {
    auto memProperties = physicalDevice.getMemoryProperties();
    for (uint32_t i = 0; i < memProperties.memoryTypeCount; ++i) {
      if (
        typeFilter & (1 << i) && (memProperties.memoryTypes[i].propertyFlags &
                                  properties) == properties) {
        return i;
      }
    }
    throw std::runtime_error("Failed to find suitable memory type");
  }

  std::pair<vk::UniqueImage, vk::UniqueImageView> createImGuiFontTexture(
    const yave::vulkan_context::window_context& windowCtx,
    const vk::PhysicalDevice& physicalDevice,
    const vk::Device& device)
  {
    /* get texture data */
    ImGuiIO& io = ImGui::GetIO();
    unsigned char* pixels;
    int width, height;
    io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
    size_t upload_size = width * height * 4 * sizeof(char);

    vk::Extent3D imageExtent {
      static_cast<uint32_t>(width), static_cast<uint32_t>(height), 1};

    /* create staging buffer */
    vk::UniqueBuffer buffer;
    {
      vk::BufferCreateInfo info;
      info.flags       = vk::BufferCreateFlags();
      info.size        = upload_size;
      info.usage       = vk::BufferUsageFlagBits::eTransferSrc;
      info.sharingMode = vk::SharingMode::eExclusive;

      buffer = device.createBufferUnique(info);
    }
    /* create staging buffer memory */
    vk::UniqueDeviceMemory bufferMemory;
    {
      auto memReq = device.getBufferMemoryRequirements(buffer.get());
      vk::MemoryAllocateInfo info;
      info.allocationSize  = memReq.size;
      info.memoryTypeIndex = findMemoryType(
        memReq.memoryTypeBits,
        vk::MemoryPropertyFlagBits::eHostVisible |
          vk::MemoryPropertyFlagBits::eHostCoherent,
        physicalDevice);

      bufferMemory = device.allocateMemoryUnique(info);
      device.bindBufferMemory(buffer.get(), bufferMemory.get(), 0);
    }

    /* create image */
    vk::UniqueImage image;
    {
      vk::ImageCreateInfo info;
      info.imageType    = vk::ImageType::e2D;
      info.format       = vk::Format::eR8G8B8A8Unorm;
      info.extent       = imageExtent;
      info.mipLevels    = 1;
      info.arrayLayers  = 1;
      info.samples      = vk::SampleCountFlagBits::e1;
      info.tiling       = vk::ImageTiling::eOptimal;
      info.usage =
        vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst;
      info.sharingMode   = vk::SharingMode::eExclusive;
      info.initialLayout = vk::ImageLayout::eUndefined;

      image = device.createImageUnique(info);
    }
    /* map image to memory */
    vk::UniqueDeviceMemory imageMemory;
    {
      auto memReq = device.getImageMemoryRequirements(image.get());
      vk::MemoryAllocateInfo info;
      info.allocationSize  = memReq.size;
      info.memoryTypeIndex = findMemoryType(
        memReq.memoryTypeBits,
        vk::MemoryPropertyFlagBits::eDeviceLocal,
        physicalDevice);
      imageMemory = device.allocateMemoryUnique(info);
      device.bindImageMemory(image.get(), imageMemory.get(), 0);
    }
    /* create subresource range */
    vk::ImageSubresourceRange subresourceRange;
    {
      subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
      subresourceRange.levelCount = 1;
      subresourceRange.layerCount = 1;
    }
    /* create image view */
    vk::UniqueImageView imageView;
    {
      vk::ImageViewCreateInfo info;
      info.image            = image.get();
      info.viewType         = vk::ImageViewType::e2D;
      info.format           = vk::Format::eR8G8B8A8Unorm;
      info.subresourceRange = subresourceRange;
      imageView             = device.createImageViewUnique(info);
    }

    /* upload data */
    {
      void* ptr = device.mapMemory(bufferMemory.get(), 0, upload_size);
      std::memcpy(ptr, pixels, upload_size);
      device.unmapMemory(bufferMemory.get());
    }

    auto singleTimeCommand = windowCtx.single_time_command();
    auto cmd               = singleTimeCommand.command_buffer();

    /* image layout: undefined -> transfer dst optimal */
    {
      vk::ImageMemoryBarrier barrier;
      barrier.oldLayout           = vk::ImageLayout::eUndefined;
      barrier.newLayout           = vk::ImageLayout::eTransferDstOptimal;
      barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
      barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
      barrier.image               = image.get();
      barrier.subresourceRange    = subresourceRange;
      barrier.dstAccessMask       = vk::AccessFlagBits::eTransferWrite;

      vk::PipelineStageFlags srcStage = vk::PipelineStageFlagBits::eTopOfPipe;
      vk::PipelineStageFlags dstStage = vk::PipelineStageFlagBits::eTransfer;

      cmd.pipelineBarrier(
        srcStage, dstStage, vk::DependencyFlags(), {}, {}, barrier);
    }

    /* copy: buffer -> image */
    {
      vk::BufferImageCopy region;
      region.imageSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
      region.imageSubresource.layerCount = 1;
      region.imageExtent                 = imageExtent;

      cmd.copyBufferToImage(
        buffer.get(),
        image.get(),
        vk::ImageLayout::eTransferDstOptimal,
        region);
    }
    /* image layout: transfer dst optimal -> shader read only */
    {
      vk::ImageMemoryBarrier barrier;
      barrier.oldLayout           = vk::ImageLayout::eTransferDstOptimal;
      barrier.newLayout           = vk::ImageLayout::eShaderReadOnlyOptimal;
      barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
      barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
      barrier.image               = image.get();
      barrier.subresourceRange    = subresourceRange;
      barrier.srcAccessMask       = vk::AccessFlagBits::eTransferWrite;
      barrier.dstAccessMask       = vk::AccessFlagBits::eShaderRead;

      vk::PipelineStageFlags srcStage = vk::PipelineStageFlagBits::eTransfer;
      vk::PipelineStageFlags dstStage =
        vk::PipelineStageFlagBits::eFragmentShader;

      cmd.pipelineBarrier(
        srcStage, dstStage, vk::DependencyFlags(), {}, {}, barrier);
    }

    return {std::move(image), std::move(imageView)};
  }

  vk::UniqueSampler createImGuiFontSampler(const vk::Device& device)
  {
    vk::SamplerCreateInfo info;
    info.magFilter     = vk::Filter::eLinear;
    info.minFilter     = vk::Filter::eLinear;
    info.mipmapMode    = vk::SamplerMipmapMode::eLinear;
    info.addressModeU  = vk::SamplerAddressMode::eRepeat;
    info.addressModeV  = vk::SamplerAddressMode::eRepeat;
    info.addressModeW  = vk::SamplerAddressMode::eRepeat;
    info.minLod        = -1000;
    info.maxLod        = 1000;
    info.maxAnisotropy = 1.f;

    return device.createSamplerUnique(info);
  }

  vk::UniqueDescriptorSetLayout createImGuiDescriptorSetLayout(
    const vk::Sampler& sampler,
    const vk::Device& device)
  {
    vk::DescriptorSetLayoutBinding bind;
    bind.descriptorType     = vk::DescriptorType::eCombinedImageSampler;
    bind.descriptorCount    = 1;
    bind.stageFlags         = vk::ShaderStageFlagBits::eFragment;
    bind.pImmutableSamplers = &sampler;

    vk::DescriptorSetLayoutCreateInfo info;
    info.bindingCount = 1;
    info.pBindings    = &bind;

    return device.createDescriptorSetLayoutUnique(info);
  }

  vk::UniqueDescriptorSet createImGuiDescriptorSet(
    const vk::DescriptorPool& pool,
    const vk::DescriptorSetLayout& layout,
    const vk::Device& device)
  {
    vk::DescriptorSetAllocateInfo info;
    info.descriptorPool     = pool;
    info.descriptorSetCount = 1;
    info.pSetLayouts        = &layout;
    return std::move(device.allocateDescriptorSetsUnique(info).front());
  }

  void renderImGuiDrawData(
    ImDrawData* drawData,
    const yave::vulkan_context::window_context& windowCtx,
    const vk::CommandBuffer& commandBuffer)
  {
  }

} // namespace

namespace yave {

  imgui_glfw_vulkan::imgui_glfw_vulkan(bool enableValidation)
    : m_glfwCtx {}
    , m_vulkanCtx {m_glfwCtx, enableValidation}
    , m_window {m_glfwCtx.create_window(1280, 720, "imgui_glfw_vulkan")}
    , m_windowCtx {m_vulkanCtx.create_window_context(m_window)}
  {
    using namespace yave;
    init_logger();

    /* init ImGui */

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    /* setup GLFW input binding */
    ImGui_ImplGlfw_InitForVulkan(m_window.get(), true);

    /* setup vulkan binding */
    {
      ImGuiIO& io            = ImGui::GetIO();
      io.BackendRendererName = "yave::imgui_glfw_vulkan";
    }

    Info(g_logger, "Initialized ImGui context");

    m_pipelineCache  = createPipelineCache(m_vulkanCtx.device());
    m_pipelineLayout = createImGuiPipelineLayout(m_vulkanCtx.device());
    m_pipeline    = createImGuiPipeline(
      m_windowCtx.swapchain_extent(),
      m_windowCtx.render_pass(),
      m_pipelineCache.get(),
      m_pipelineLayout.get(),
      m_vulkanCtx.device());

    Info(g_logger, "Created ImGui pipeline");

    m_fontSampler = createImGuiFontSampler(m_vulkanCtx.device());

    Info(g_logger, "Created ImGui font sampler");

    m_descriptorPool = createImGuiDescriptorPool(m_vulkanCtx.device());
    m_descriptorSetLayout =
      createImGuiDescriptorSetLayout(m_fontSampler.get(), m_vulkanCtx.device());
    m_descriptorSet = createImGuiDescriptorSet(
      m_descriptorPool.get(),
      m_descriptorSetLayout.get(),
      m_vulkanCtx.device());

    Info(g_logger, "Created ImGui descriptor set");

    /* upload font texture */
    {
      auto cmd                = m_windowCtx.single_time_command();
      auto [image, imageView] = createImGuiFontTexture(
        m_windowCtx, m_vulkanCtx.physical_device(), m_vulkanCtx.device());
      m_fontImage     = std::move(image);
      m_fontImageView = std::move(imageView);

      Info(g_logger, "Uploaded ImGui font texture");
    }
    /* update descriptor set */
    {
      vk::DescriptorImageInfo info;
      info.sampler = m_fontSampler.get();
      info.imageView = m_fontImageView.get();
      info.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;

      vk::WriteDescriptorSet write;
      write.dstSet          = m_descriptorSet.get();
      write.descriptorCount = 1;
      write.descriptorType  = vk::DescriptorType::eCombinedImageSampler;
      write.pImageInfo      = &info;

      m_vulkanCtx.device().updateDescriptorSets(write, {});

      Info(g_logger, "Updated ImGui descriptor set");
    }
  }

  imgui_glfw_vulkan::~imgui_glfw_vulkan()
  {
    Info("Destroying ImGui context");
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
  }

  void imgui_glfw_vulkan::run()
  {
    /* main loop */

    while (!m_windowCtx.should_close()) {

      /* render ImGui frame */

      ImGui_ImplGlfw_NewFrame();
      ImGui::NewFrame();
      {
      }
      ImGui::Render();

      /* render Vulkan frame */

      auto bgn = std::chrono::high_resolution_clock::now();
      {
        auto recorder = m_windowCtx.new_frame();
        renderImGuiDrawData(
          ImGui::GetDrawData(), m_windowCtx, recorder.command_buffer());
      }
      auto end = std::chrono::high_resolution_clock::now();
      auto elapsed =
        std::chrono::duration_cast<std::chrono::milliseconds>(end - bgn);
      // keep it 60 fps
      if (elapsed < std::chrono::milliseconds(16))
        std::this_thread::sleep_for(std::chrono::milliseconds(16) - elapsed);
    }

    Info("Finished");
  }

} // namespace yave