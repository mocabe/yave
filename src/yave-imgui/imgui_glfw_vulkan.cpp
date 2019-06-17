//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave-imgui/imgui_glfw_vulkan.hpp>
#include <yave/support/log.hpp>

#include <imgui.h>
#include <imgui_impl_glfw.h>

#include <chrono>
#include <thread>
#include <fstream>

#include <boost/dll/runtime_symbol_info.hpp>

namespace {

  std::shared_ptr<spdlog::logger> g_logger;

  void init_logger()
  {
    [[maybe_unused]] static auto init_logger = [] {
      g_logger = yave::add_logger("imgui_glfw_vulkan");
      return 1;
    }();
  }

  vk::UniquePipelineLayout createImGuiPipelineLayout(
    const vk::DescriptorSetLayout& setLayout,
    const vk::Device& device)
  {
    // Push constants:
    // vec2 scale;
    // vec2 translate;
    vk::PushConstantRange pcr;
    pcr.stageFlags = vk::ShaderStageFlagBits::eVertex;
    pcr.size       = sizeof(float) * 4;

    vk::PipelineLayoutCreateInfo info;
    info.setLayoutCount         = 1;
    info.pSetLayouts            = &setLayout;
    info.pushConstantRangeCount = 1;
    info.pPushConstantRanges    = &pcr;

    return device.createPipelineLayoutUnique(info);
  }

  vk::UniquePipelineCache createPipelineCache(const vk::Device& device)
  {
    vk::PipelineCacheCreateInfo info;
    return device.createPipelineCacheUnique(info);
  }

  vk::UniqueShaderModule
    createShaderModule(const std::vector<char>& code, const vk::Device& device)
  {
    vk::ShaderModuleCreateInfo info;
    info.flags    = vk::ShaderModuleCreateFlags();
    info.codeSize = code.size();
    info.pCode    = reinterpret_cast<const uint32_t*>(code.data());

    return device.createShaderModuleUnique(info);
  }

  std::vector<char> loadShaderFile(const std::string& filename)
  {
    // get relative path (this really should be in std::filesystem...)
    auto path = boost::dll::program_location().remove_filename() / filename;

    // open file and seek to end
    std::ifstream ifs(path.native(), std::ios::binary | std::ios::ate);

    if (!ifs.is_open())
      throw std::runtime_error("Failed to open shader file " + path.string());

    // get file size
    auto size = ifs.tellg();
    std::vector<char> ret(size);

    // read whole file to buffer
    ifs.seekg(0);
    ifs.read(ret.data(), size);

    return ret;
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
      createShaderModule(loadShaderFile("vert.spv"), device);

    vk::PipelineShaderStageCreateInfo vertShaderStageInfo;
    vertShaderStageInfo.stage  = vk::ShaderStageFlagBits::eVertex;
    vertShaderStageInfo.module = *vertShaderModule;
    vertShaderStageInfo.pName  = "main";

    // fragment shader
    vk::UniqueShaderModule fragShaderModule =
      createShaderModule(loadShaderFile("frag.spv"), device);

    vk::PipelineShaderStageCreateInfo fragShaderStageInfo;
    fragShaderStageInfo.stage  = vk::ShaderStageFlagBits::eFragment;
    fragShaderStageInfo.module = *fragShaderModule;
    fragShaderStageInfo.pName  = "main";

    // stages
    std::array shaderStages = {vertShaderStageInfo, fragShaderStageInfo};

    /* vertex input */

    // ImDrawVert
    std::array<vk::VertexInputBindingDescription, 1> vertBindingDesc;
    vertBindingDesc[0].stride    = sizeof(ImDrawVert);
    vertBindingDesc[0].inputRate = vk::VertexInputRate::eVertex;

    // vertex input:
    // vec2 pos
    // vec2 uv
    // vec2 color
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
    vertInputStateInfo.vertexBindingDescriptionCount   = vertBindingDesc.size();
    vertInputStateInfo.pVertexBindingDescriptions      = vertBindingDesc.data();
    vertInputStateInfo.vertexAttributeDescriptionCount = vertAttrDesc.size();
    vertInputStateInfo.pVertexAttributeDescriptions    = vertAttrDesc.data();

    /* input assembler */

    vk::PipelineInputAssemblyStateCreateInfo inputAsmStateInfo;
    inputAsmStateInfo.topology = vk::PrimitiveTopology::eTriangleList;

    /* viewport */

    // these values are ignored when dynamic viewport is enabled.

    std::array<vk::Viewport, 1> viewports;
    viewports[0].width    = swapchainExtent.width;
    viewports[0].height   = swapchainExtent.height;
    viewports[0].maxDepth = 1.f;

    std::array<vk::Rect2D, 1> scissors;
    scissors[0].extent = swapchainExtent;

    static_assert(viewports.size() == scissors.size());

    vk::PipelineViewportStateCreateInfo viewportStateInfo;
    viewportStateInfo.viewportCount = viewports.size();
    viewportStateInfo.pViewports    = viewports.data();
    viewportStateInfo.scissorCount  = scissors.size();
    viewportStateInfo.pScissors     = scissors.data();

    /* rasterization */

    vk::PipelineRasterizationStateCreateInfo rasterStateInfo;
    rasterStateInfo.polygonMode = vk::PolygonMode::eFill;
    rasterStateInfo.frontFace   = vk::FrontFace::eCounterClockwise;
    rasterStateInfo.lineWidth   = 1.f;

    /* sample */

    vk::PipelineMultisampleStateCreateInfo multisampleStateInfo;
    multisampleStateInfo.rasterizationSamples = vk::SampleCountFlagBits::e1;
    multisampleStateInfo.minSampleShading     = 1.f;

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

    vk::PipelineColorBlendStateCreateInfo colorBlendStateInfo;
    colorBlendStateInfo.attachmentCount = colAttachments.size();
    colorBlendStateInfo.pAttachments    = colAttachments.data();

    vk::PipelineDepthStencilStateCreateInfo depthStencilStateInfo;
    depthStencilStateInfo.depthCompareOp = vk::CompareOp::eLess;
    depthStencilStateInfo.minDepthBounds = 0.f;
    depthStencilStateInfo.maxDepthBounds = 1.f;

    /* dynamic state */

    std::array dynamicStates = {vk::DynamicState::eViewport,
                                vk::DynamicState::eScissor};

    vk::PipelineDynamicStateCreateInfo dynamicStateInfo;
    dynamicStateInfo.dynamicStateCount = dynamicStates.size();
    dynamicStateInfo.pDynamicStates    = dynamicStates.data();

    /* pipeline */

    vk::GraphicsPipelineCreateInfo info;
    info.stageCount          = shaderStages.size();
    info.pStages             = shaderStages.data();
    info.pVertexInputState   = &vertInputStateInfo;
    info.pInputAssemblyState = &inputAsmStateInfo;
    info.pViewportState      = &viewportStateInfo;
    info.pRasterizationState = &rasterStateInfo;
    info.pMultisampleState   = &multisampleStateInfo;
    info.pColorBlendState    = &colorBlendStateInfo;
    info.pDepthStencilState  = &depthStencilStateInfo;
    info.pDynamicState       = &dynamicStateInfo;
    info.renderPass          = renderPass;
    info.layout              = pipelineLayout;

    return device.createGraphicsPipelineUnique(pipelineCache, info);
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

    return device.createDescriptorPoolUnique(info);
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

  std::tuple<vk::UniqueDeviceMemory, vk::UniqueImage, vk::UniqueImageView>
    createImGuiFontTexture(
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
      info.imageType   = vk::ImageType::e2D;
      info.format      = vk::Format::eR8G8B8A8Unorm;
      info.extent      = imageExtent;
      info.mipLevels   = 1;
      info.arrayLayers = 1;
      info.samples     = vk::SampleCountFlagBits::e1;
      info.tiling      = vk::ImageTiling::eOptimal;
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
      barrier.oldLayout        = vk::ImageLayout::eTransferDstOptimal;
      barrier.newLayout        = vk::ImageLayout::eShaderReadOnlyOptimal;
      barrier.image            = image.get();
      barrier.subresourceRange = subresourceRange;
      barrier.srcAccessMask    = vk::AccessFlagBits::eTransferWrite;
      barrier.dstAccessMask    = vk::AccessFlagBits::eShaderRead;

      vk::PipelineStageFlags srcStage = vk::PipelineStageFlagBits::eTransfer;
      vk::PipelineStageFlags dstStage =
        vk::PipelineStageFlagBits::eFragmentShader;

      cmd.pipelineBarrier(
        srcStage, dstStage, vk::DependencyFlags(), {}, {}, barrier);
    }

    return {std::move(imageMemory), std::move(image), std::move(imageView)};
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

  template <class BufferT>
  void uploadImGuiDrawData(
    const BufferT& vertexBuffer,
    const BufferT& indexBuffer,
    const ImDrawData* drawData,
    const vk::Device& device)
  {
    assert(vertexBuffer.size() == drawData->TotalVtxCount * sizeof(ImDrawVert));
    assert(indexBuffer.size() == drawData->TotalIdxCount * sizeof(ImDrawIdx));

    if (vertexBuffer.size() != 0) {
      // map to host memory
      std::byte* vertexPtr = (std::byte*)device.mapMemory(
        vertexBuffer.memory(), 0, vertexBuffer.size());
      // copy data
      for (int i = 0; i < drawData->CmdListsCount; ++i) {
        const ImDrawList* cmdList = drawData->CmdLists[i];
        auto vtxSize              = cmdList->VtxBuffer.size_in_bytes();
        std::memcpy(vertexPtr, cmdList->VtxBuffer.Data, vtxSize);
        vertexPtr += vtxSize;
      }
      // unmap memory
      {
        vk::MappedMemoryRange range;
        range.memory = vertexBuffer.memory();
        device.flushMappedMemoryRanges(range);
        device.unmapMemory(vertexBuffer.memory());
      }
    }

    if (indexBuffer.size() != 0) {
      // map to host memory
      std::byte* indexPtr = (std::byte*)device.mapMemory(
        indexBuffer.memory(), 0, indexBuffer.size());
      // copy data
      for (int i = 0; i < drawData->CmdListsCount; ++i) {
        const ImDrawList* cmdList = drawData->CmdLists[i];
        auto idxSize              = cmdList->IdxBuffer.size_in_bytes();
        std::memcpy(indexPtr, cmdList->IdxBuffer.Data, idxSize);
        indexPtr += idxSize;
      }
      // unmap memory
      {
        vk::MappedMemoryRange range;
        range.memory = indexBuffer.memory();
        device.flushMappedMemoryRanges(range);
        device.unmapMemory(indexBuffer.memory());
      }
    }
  }

  ImTextureID toImTextureId(vk::DescriptorSet& dsc)
  {
    return (ImTextureID)&dsc;
  }

  const vk::DescriptorSet* fromImTextureId(const ImTextureID& tex)
  {
    return (const vk::DescriptorSet*)tex;
  }

  void initImGuiPipeline(
    const ImDrawData* drawData,
    const vk::PipelineLayout& pipelineLayout,
    const vk::Pipeline& pipeline,
    const vk::Buffer& vertexBuffer,
    const vk::Buffer& indexBuffer,
    const vk::Extent2D swapchainExtent,
    const vk::CommandBuffer& commandBuffer)
  {
    /* Bind pipeline */
    {
      commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline);
    }
    /* Bind buffers */
    {
      commandBuffer.bindVertexBuffers(0, vertexBuffer, {0});

      static_assert(
        std::is_same_v<ImDrawIdx, uint16_t> ||
        std::is_same_v<ImDrawIdx, uint32_t>);

      vk::IndexType indexType = sizeof(ImDrawIdx) == sizeof(uint16_t)
                                  ? vk::IndexType::eUint16
                                  : vk::IndexType::eUint32;

      commandBuffer.bindIndexBuffer(indexBuffer, 0, indexType);
    }
    /* Viewport */
    {
      vk::Viewport viewport;
      viewport.width    = swapchainExtent.width;
      viewport.height   = swapchainExtent.height;
      viewport.maxDepth = 1.f;
      commandBuffer.setViewport(0, viewport);
    }
    /* Push constants */
    {
      float scale[2];
      scale[0] = 2.f / drawData->DisplaySize.x;
      scale[1] = 2.f / drawData->DisplaySize.y;

      commandBuffer.pushConstants(
        pipelineLayout,
        vk::ShaderStageFlagBits::eVertex,
        sizeof(float) * 0,
        sizeof(float) * 2,
        scale);

      float translate[2];
      translate[0] = -1.f - drawData->DisplayPos.x * scale[0];
      translate[1] = -1.f - drawData->DisplayPos.y * scale[1];

      commandBuffer.pushConstants(
        pipelineLayout,
        vk::ShaderStageFlagBits::eVertex,
        sizeof(float) * 2,
        sizeof(float) * 2,
        translate);
    }
  }

  void renderImGuiDrawData(
    const ImDrawData* drawData,
    const vk::DescriptorSet& descriptorSet,
    const vk::PipelineLayout& pipelineLayout,
    const vk::Pipeline& pipeline,
    const vk::Buffer& vertexBuffer,
    const vk::Buffer& indexBuffer,
    const vk::Extent2D swapchainExtent,
    const vk::CommandBuffer& commandBuffer)
  {
    using namespace yave;

    ImVec2 viewportPos = drawData->DisplayPos;
    ImVec2 pixScale    = drawData->FramebufferScale;

    int idxOffset = 0;
    int vtxOffset = 0;

    for (int listIdx = 0; listIdx < drawData->CmdListsCount; ++listIdx) {

      const ImDrawList* cmdList = drawData->CmdLists[listIdx];

      for (int cmdIdx = 0; cmdIdx < cmdList->CmdBuffer.Size; ++cmdIdx) {

        const ImDrawCmd& cmd = cmdList->CmdBuffer[cmdIdx];

        // User callback
        if (cmd.UserCallback) {
          // Handle special callback to reset render state.
          if (cmd.UserCallback == ImDrawCallback_ResetRenderState)
            initImGuiPipeline(
              drawData,
              pipelineLayout,
              pipeline,
              vertexBuffer,
              indexBuffer,
              swapchainExtent,
              commandBuffer);
          else
            cmd.UserCallback(cmdList, &cmd);
          continue;
        }

        if (
          cmd.ClipRect.z < cmd.ClipRect.x && cmd.ClipRect.w < cmd.ClipRect.y) {
          Error(
            g_logger,
            "Detected invalid ClipRect in draw data. This ca be caused by bug "
            "in ImGui. ClipRect: {},{},{},{}",
            cmd.ClipRect.x,
            cmd.ClipRect.y,
            cmd.ClipRect.z,
            cmd.ClipRect.w);
          continue;
        }

        /* Texture */

        auto dscToBind =
          cmd.TextureId ? *fromImTextureId(cmd.TextureId) : descriptorSet;

        commandBuffer.bindDescriptorSets(
          vk::PipelineBindPoint::eGraphics, pipelineLayout, 0, dscToBind, {});

        /* Render */

        // scissor
        vk::Rect2D scissor;
        scissor.offset.x      = (cmd.ClipRect.x - viewportPos.x) * pixScale.x;
        scissor.offset.y      = (cmd.ClipRect.y - viewportPos.y) * pixScale.y;
        scissor.extent.width  = (cmd.ClipRect.z - cmd.ClipRect.x) * pixScale.x;
        scissor.extent.height = (cmd.ClipRect.w - cmd.ClipRect.y) * pixScale.y;

        // Vulkan specification says offset must be positive.
        // Scissor rect can be larger than frame buffer.
        scissor.offset.x = std::max(0, scissor.offset.x);
        scissor.offset.y = std::max(0, scissor.offset.y);

        // set clipping rectangle
        commandBuffer.setScissor(0, scissor);

        // draw
        commandBuffer.drawIndexed(
          cmd.ElemCount,
          1,
          (uint32_t)idxOffset + cmd.IdxOffset,
          (uint32_t)vtxOffset + cmd.VtxOffset,
          0);
      }

      idxOffset += cmdList->IdxBuffer.Size;
      vtxOffset += cmdList->VtxBuffer.Size;
    }
  }
} // namespace

namespace yave {

  /// for vertex/index buffers
  class ImGuiRenderBuffer
  {
  public:
    ImGuiRenderBuffer(const vk::BufferUsageFlags& usage)
      : m_usage {usage}
    {
      // Leave all resources uninitialized. Use reisze() before using.
    }

    vk::DeviceSize size() const
    {
      return m_size;
    }

    vk::DeviceSize capacity() const
    {
      return m_capacity;
    }

    vk::Buffer buffer() const
    {
      return m_buffer.get();
    }

    vk::DeviceMemory memory() const
    {
      return m_memory.get();
    }

    void resize(
      const vk::DeviceSize& size,
      const vk::PhysicalDevice& physicalDevice,
      const vk::Device& device);

  private:
    vk::BufferUsageFlags m_usage; // init by ctor
    vk::DeviceSize m_size           = 0;
    vk::DeviceSize m_capacity       = 0;
    vk::UniqueBuffer m_buffer       = vk::UniqueBuffer();
    vk::UniqueDeviceMemory m_memory = vk::UniqueDeviceMemory();
  };

  void ImGuiRenderBuffer::resize(
    const vk::DeviceSize& newSize,
    const vk::PhysicalDevice& physicalDevice,
    const vk::Device& device)
  {
    // create new buffer
    vk::BufferCreateInfo info;
    info.size        = newSize;
    info.usage       = m_usage;
    info.sharingMode = vk::SharingMode::eExclusive;

    // Vulkan does not allow zero sized buffer.
    // Just use dymmy size here.
    if (info.size == 0)
      info.size = 1;

    // create new buffer
    auto buff   = device.createBufferUnique(info);
    auto memReq = device.getBufferMemoryRequirements(buff.get());

    // Avoid allocation when new size is smaller than current capacity unless
    // required size is very small.
    if (newSize <= m_capacity && newSize > m_capacity / 8) {
      // Bind existing memory to new buffer.
      device.bindBufferMemory(buff.get(), m_memory.get(), 0);
      // Update
      m_buffer = std::move(buff);
      m_size   = newSize;
      return;
    }

    // Double buffer capacity (or half when we can shrink).
    auto newCapacity = (memReq.size < m_capacity)
                         ? std::max(memReq.size, m_capacity / 2)
                         : std::max(memReq.size, m_capacity * 2);

    vk::MemoryAllocateInfo allocInfo;
    allocInfo.allocationSize  = newCapacity;
    allocInfo.memoryTypeIndex = findMemoryType(
      memReq.memoryTypeBits,
      vk::MemoryPropertyFlagBits::eHostVisible,
      physicalDevice);

    // allocate new device memory.
    auto mem = device.allocateMemoryUnique(allocInfo);

    // Bind new memory to new buffer.
    device.bindBufferMemory(buff.get(), mem.get(), 0);

    // Update
    m_size     = newSize;
    m_capacity = newCapacity;
    m_buffer   = std::move(buff);
    m_memory   = std::move(mem);
  }

  imgui_glfw_vulkan::imgui_glfw_vulkan(bool enableValidation)
    : m_glfwCtx {}
    , m_vulkanCtx {m_glfwCtx, enableValidation}
    , m_window {m_glfwCtx.create_window(1280, 720, "imgui_glfw_vulkan")}
    , m_windowCtx {m_vulkanCtx.create_window_context(m_window)}
  {
    using namespace yave;
    init_logger();

    /* init ImGui */
    {
      IMGUI_CHECKVERSION();
      ImGui::CreateContext();
      ImGui::StyleColorsDark();

      /* setup GLFW input binding */
      ImGui_ImplGlfw_InitForVulkan(m_window.get(), true);

      /* setup vulkan binding */
      ImGuiIO& io            = ImGui::GetIO();
      io.BackendRendererName = "yave::imgui_glfw_vulkan";

      Info(g_logger, "Initialized ImGui context");
    }

    {
      m_fontSampler = createImGuiFontSampler(m_vulkanCtx.device());
      Info(g_logger, "Created ImGui font sampler");
    }

    {
      m_descriptorPool      = createImGuiDescriptorPool(m_vulkanCtx.device());
      m_descriptorSetLayout = createImGuiDescriptorSetLayout(
        m_fontSampler.get(), m_vulkanCtx.device());
      Info(g_logger, "Created ImGui descriptor set");
    }

    {
      m_pipelineCache  = createPipelineCache(m_vulkanCtx.device());
      m_pipelineLayout = createImGuiPipelineLayout(
        m_descriptorSetLayout.get(), m_vulkanCtx.device());
      m_pipeline = createImGuiPipeline(
        m_windowCtx.swapchain_extent(),
        m_windowCtx.render_pass(),
        m_pipelineCache.get(),
        m_pipelineLayout.get(),
        m_vulkanCtx.device());

      Info(g_logger, "Created ImGui pipeline");
    }

    /* upload font texture */
    {
      auto cmd                             = m_windowCtx.single_time_command();
      auto [imageMemory, image, imageView] = createImGuiFontTexture(
        m_windowCtx, m_vulkanCtx.physical_device(), m_vulkanCtx.device());
      m_fontImageMemory = std::move(imageMemory);
      m_fontImage       = std::move(image);
      m_fontImageView   = std::move(imageView);

      Info(g_logger, "Uploaded ImGui font texture");
    }

    /* create default descriptor set (font texture) */
    {
      m_descriptorSet = createImGuiDescriptorSet(
        m_descriptorPool.get(),
        m_descriptorSetLayout.get(),
        m_vulkanCtx.device());

      vk::DescriptorImageInfo info;
      info.sampler     = m_fontSampler.get();
      info.imageView   = m_fontImageView.get();
      info.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;

      vk::WriteDescriptorSet write;
      write.dstSet          = m_descriptorSet.get();
      write.descriptorCount = 1;
      write.descriptorType  = vk::DescriptorType::eCombinedImageSampler;
      write.pImageInfo      = &info;

      m_vulkanCtx.device().updateDescriptorSets(write, {});

      // set font texture ID
      ImGui::GetIO().Fonts->TexID = toImTextureId(m_descriptorSet.get());

      Info(g_logger, "Updated ImGui descriptor set");
    }

    // for rendering loop
    m_lastTime = std::chrono::high_resolution_clock::now();
  }

  imgui_glfw_vulkan::~imgui_glfw_vulkan()
  {
    Info(g_logger, "Destroying ImGui context");
    // wait idle
    m_vulkanCtx.device().waitIdle();
    // unbind GLFW
    ImGui_ImplGlfw_Shutdown();
    // destroy ImGui
    ImGui::DestroyContext();
  }

  void imgui_glfw_vulkan::begin()
  {
    /* start ImGui frame */
    {
      glfwPollEvents();
      ImGui_ImplGlfw_NewFrame();
      ImGui::NewFrame();
    }
  }

  void imgui_glfw_vulkan::end()
  {
    /* end ImGui frame */
    {
      ImGui::Render();
    }
  }

  void imgui_glfw_vulkan::render()
  {
    /* render Vulkan frame */

    // ImGui draw data
    const auto* drawData = ImGui::GetDrawData();

    assert(drawData->Valid);

    m_windowCtx.set_clear_color(0.45f, 0.55f, 0.60f, 1.f);

    /* Render with Vulkan */
    {
      // begin new frame (use RAII recorder)
      auto recorder      = m_windowCtx.new_recorder();
      auto commandBuffer = recorder.command_buffer();

      // device
      auto physicalDevice = m_vulkanCtx.physical_device();
      auto device         = m_vulkanCtx.device();

      /* Resize buffers vector to match current in-flight frames */
      {
        auto frameCount = m_windowCtx.frame_index_count();
        // vertec buffer
        if (m_vertexBuffers.size() > frameCount) {
          for (size_t i = 0; i < m_vertexBuffers.size() - frameCount; ++i)
            m_vertexBuffers.pop_back();
        } else {
          for (size_t i = 0; i < frameCount - m_vertexBuffers.size(); ++i)
            m_vertexBuffers.emplace_back(
              vk::BufferUsageFlagBits::eVertexBuffer);
        }
        // index buffer
        if (m_indexBuffers.size() > frameCount) {
          for (size_t i = 0; i < m_indexBuffers.size() - frameCount; ++i)
            m_indexBuffers.pop_back();
        } else {
          for (size_t i = 0; i < frameCount - m_indexBuffers.size(); ++i)
            m_indexBuffers.emplace_back(vk::BufferUsageFlagBits::eIndexBuffer);
        }
      }

      auto& vertexBuffer = m_vertexBuffers[m_windowCtx.frame_index()];
      auto& indexBuffer  = m_indexBuffers[m_windowCtx.frame_index()];

      /* Resize vertex/index buffers */
      {
        {
          auto newSize = drawData->TotalVtxCount * sizeof(ImDrawVert);
          vertexBuffer.resize(newSize, physicalDevice, device);
          assert(vertexBuffer.size() == newSize);
        }
        {
          auto newSize = drawData->TotalIdxCount * sizeof(ImDrawIdx);
          indexBuffer.resize(newSize, physicalDevice, device);
          assert(indexBuffer.size() == newSize);
        }
      }

      /* Upload vertex/index data to GPU */
      uploadImGuiDrawData(vertexBuffer, indexBuffer, drawData, device);

      /* Initialize pipeline */
      initImGuiPipeline(
        drawData,
        m_pipelineLayout.get(),
        m_pipeline.get(),
        vertexBuffer.buffer(),
        indexBuffer.buffer(),
        m_windowCtx.swapchain_extent(),
        commandBuffer);

      /* Record draw commands */
      renderImGuiDrawData(
        drawData,
        m_descriptorSet.get(),
        m_pipelineLayout.get(),
        m_pipeline.get(),
        vertexBuffer.buffer(),
        indexBuffer.buffer(),
        m_windowCtx.swapchain_extent(),
        commandBuffer);
    }

    /* frame rate limiter */

    auto endTime         = std::chrono::high_resolution_clock::now();
    auto frameTime       = (endTime - m_lastTime);
    auto frameTimeWindow = std::chrono::microseconds(16666);
    auto sleepTime       = frameTimeWindow - frameTime;

    if (sleepTime.count() > 0) {
      m_lastTime = endTime + sleepTime;
      std::this_thread::sleep_for(sleepTime);
    } else {
      m_lastTime = endTime;
    }
  }

  ImTextureID imgui_glfw_vulkan::get_texture_id(vk::DescriptorSet& tex) const
  {
    return toImTextureId(tex);
  }

  const glfw_context& imgui_glfw_vulkan::glfw_context() const
  {
    return m_glfwCtx;
  }

  const vulkan_context& imgui_glfw_vulkan::vulkan_context() const
  {
    return m_vulkanCtx;
  }

  const vulkan_context::window_context&
    imgui_glfw_vulkan::window_context() const
  {
    return m_windowCtx;
  }

  vk::Sampler imgui_glfw_vulkan::font_sampler() const
  {
    return m_fontSampler.get();
  }

  vk::DescriptorPool imgui_glfw_vulkan::descriptor_pool() const
  {
    return m_descriptorPool.get();
  }

  vk::DescriptorSetLayout imgui_glfw_vulkan::descriptor_set_layout() const
  {
    return m_descriptorSetLayout.get();
  }

  vk::DescriptorSet imgui_glfw_vulkan::descriptor_set() const
  {
    return m_descriptorSet.get();
  }

  vk::PipelineCache imgui_glfw_vulkan::pipeline_cache() const
  {
    return m_pipelineCache.get();
  }

  vk::PipelineLayout imgui_glfw_vulkan::pipeline_layout() const
  {
    return m_pipelineLayout.get();
  }

  vk::Pipeline imgui_glfw_vulkan::pipeline() const
  {
    return m_pipeline.get();
  }

  vk::DeviceMemory imgui_glfw_vulkan::font_image_memory() const
  {
    return m_fontImageMemory.get();
  }

  vk::Image imgui_glfw_vulkan::font_image() const
  {
    return m_fontImage.get();
  }

  vk::ImageView imgui_glfw_vulkan::font_image_view() const
  {
    return m_fontImageView.get();
  }
} // namespace yave