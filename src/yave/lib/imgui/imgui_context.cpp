//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/config/config.hpp>
#include <yave/lib/imgui/imgui_context.hpp>
#include <yave/lib/vulkan/texture.hpp>
#include <yave/lib/filesystem/filesystem.hpp>
#include <yave/support/log.hpp>

#include <imgui.h>
#include <imgui_impl_glfw.h>

#include <chrono>
#include <thread>
#include <fstream>
#include <map>

#if !defined(YAVE_IMGUI_VERT_SHADER) || !defined(YAVE_IMGUI_FRAG_SHADER)
#  pragma message("Shader paths are not privided by build script.")
#  define YAVE_IMGUI_VERT_SHADER vert.spv
#  define YAVE_IMGUI_FRAG_SHADER frag.spv
#endif

#if !defined(YAVE_IMGUI_FONT_ROBOTO)
#  pragma message("Font file paths are not provided by build script.")
#endif

#define YAVE_TOSTR_(id) #id
#define YAVE_TOSTR(id) YAVE_TOSTR_(id)

YAVE_DECL_G_LOGGER(imgui)

namespace {

  auto createImGuiPipelineLayout(
    const vk::DescriptorSetLayout& setLayout,
    const vk::Device& device) -> vk::UniquePipelineLayout
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

  auto createPipelineCache(const vk::Device& device) -> vk::UniquePipelineCache
  {
    vk::PipelineCacheCreateInfo info;
    return device.createPipelineCacheUnique(info);
  }

  auto createShaderModule(
    const std::vector<char>& code,
    const vk::Device& device) -> vk::UniqueShaderModule
  {
    vk::ShaderModuleCreateInfo info;
    info.flags    = vk::ShaderModuleCreateFlags();
    info.codeSize = code.size();
    info.pCode    = reinterpret_cast<const uint32_t*>(code.data());

    return device.createShaderModuleUnique(info);
  }

  auto loadShaderFile(const std::string& str_path) -> std::vector<char>
  {
    auto path = std::filesystem::u8path(str_path);

    // open file and seek to end
    std::ifstream ifs(path.string(), std::ios::binary | std::ios::ate);

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

  auto createImGuiPipeline(
    const vk::Extent2D& swapchainExtent,
    const vk::RenderPass& renderPass,
    const vk::PipelineCache& pipelineCache,
    const vk::PipelineLayout& pipelineLayout,
    const vk::Device& device) -> vk::UniquePipeline
  {
    /* shader stages */

    // vertex shader
    vk::UniqueShaderModule vertShaderModule = createShaderModule(
      loadShaderFile(YAVE_TOSTR(YAVE_IMGUI_VERT_SHADER)), device);

    vk::PipelineShaderStageCreateInfo vertShaderStageInfo;
    vertShaderStageInfo.stage  = vk::ShaderStageFlagBits::eVertex;
    vertShaderStageInfo.module = *vertShaderModule;
    vertShaderStageInfo.pName  = "main";

    // fragment shader
    vk::UniqueShaderModule fragShaderModule = createShaderModule(
      loadShaderFile(YAVE_TOSTR(YAVE_IMGUI_FRAG_SHADER)), device);

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
    colAttachments[0].srcAlphaBlendFactor = vk::BlendFactor::eSrcAlpha;
    colAttachments[0].dstAlphaBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha;
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

  auto createImGuiDescriptorPool(const vk::Device& device)
    -> vk::UniqueDescriptorPool
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

  auto findMemoryType(
    uint32_t typeFilter,
    const vk::MemoryPropertyFlags& properties,
    const vk::PhysicalDevice& physicalDevice) -> uint32_t
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

  auto createImGuiFontTexture(
    const yave::vulkan::window_context& windowCtx,
    const vk::PhysicalDevice& physicalDevice,
    const vk::Device& device)
    -> std::tuple<vk::UniqueDeviceMemory, vk::UniqueImage, vk::UniqueImageView>
  {
    ImGuiIO& io = ImGui::GetIO();

    /* get texture data */
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

  auto createImGuiFontSampler(const vk::Device& device) -> vk::UniqueSampler
  {
    vk::SamplerCreateInfo info;
    info.magFilter     = vk::Filter::eNearest;
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

  auto createImGuiDescriptorSetLayout(
    const vk::Sampler& sampler,
    const vk::Device& device) -> vk::UniqueDescriptorSetLayout
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

  auto createImGuiDescriptorSet(
    const vk::DescriptorPool& pool,
    const vk::DescriptorSetLayout& layout,
    const vk::Device& device) -> vk::UniqueDescriptorSet
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

  auto toImTextureId(const vk::DescriptorSet* dsc) -> ImTextureID
  {
    // we don't assume anything about what ImTextureID actually is, so use
    // memcpy() here.
    static_assert(sizeof(ImTextureID) == sizeof(vk::DescriptorSet*));
    ImTextureID ret;
    std::memcpy(&ret, &dsc, sizeof(ImTextureID));
    return ret;
  }

  auto fromImTextureId(const ImTextureID& tex) -> const vk::DescriptorSet*
  {
    const vk::DescriptorSet* ret;
    std::memcpy(&ret, &tex, sizeof(vk::DescriptorSet*));
    return ret;
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

        assert(dscToBind);

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

namespace yave::imgui {

  /// for vertex/index buffers
  class ImGuiRenderBuffer
  {
  public:
    ImGuiRenderBuffer(const vk::BufferUsageFlags& usage)
      : m_usage {usage}
    {
      // Leave all resources uninitialized. Use reisze() before using.
    }

    auto size() const -> vk::DeviceSize
    {
      return m_size;
    }

    auto capacity() const -> vk::DeviceSize
    {
      return m_capacity;
    }

    auto buffer() const -> vk::Buffer
    {
      return m_buffer.get();
    }

    auto memory() const -> vk::DeviceMemory
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
    if (newSize <= m_capacity && newSize > m_capacity / 16) {
      // Bind existing memory to new buffer.
      device.bindBufferMemory(buff.get(), m_memory.get(), 0);
      // Update
      m_buffer = std::move(buff);
      m_size   = newSize;
      return;
    }

    // Find new buffer capacity
    auto newCapacity = std::max(m_capacity, vk::DeviceSize(1));

    while (memReq.size > newCapacity)
      newCapacity *= 2;

    while (memReq.size < newCapacity / 2)
      newCapacity /= 2;

    assert(newCapacity >= memReq.size);

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

  // fwd
  class ImGuiTextureDataHolder;

  class imgui_context::impl
  {
  public: /* ref */
    vulkan::vulkan_context& vulkanCtx;

  public: /* managed by impl */
    glfw::glfw_context glfwCtx;
    glfw::glfw_window glfwWindow;
    vulkan::window_context windowCtx;
    ImGuiContext* imCtx;

  public:
    vk::UniqueSampler fontSampler;
    vk::UniqueDescriptorPool descriptorPool;
    vk::UniqueDescriptorSetLayout descriptorSetLayout;
    vk::UniqueDescriptorSet descriptorSet;
    vk::UniquePipelineCache pipelineCache;
    vk::UniquePipelineLayout pipelineLayout;
    vk::UniquePipeline pipeline;
    vk::UniqueDeviceMemory fontImageMemory;
    vk::UniqueImage fontImage;
    vk::UniqueImageView fontImageView;

  public:
    std::vector<class ImGuiRenderBuffer> vertexBuffers;
    std::vector<class ImGuiRenderBuffer> indexBuffers;

    std::chrono::high_resolution_clock::time_point lastTime;

  public:
    vulkan::staging_buffer texture_staging;
    std::map<std::string, std::unique_ptr<ImGuiTextureDataHolder>> textures;

  public:
    uint32_t fps;
    std::array<float, 4> clearColor;

  public:
    impl(
      vulkan::vulkan_context& vulkan_ctx,
      glfw::glfw_context::init_flags glfw_flags,
      uint32_t width,
      uint32_t height,
      const char* windowName);
    ~impl() noexcept;

  public:
    void set_current();
    void begin_frame();
    void end_frame();

  public:
    void render();

  public:
    auto add_texture(
      const std::string& name,
      const vk::Extent2D& extent,
      const vk::DeviceSize& byte_size,
      const vk::Format& format,
      const uint8_t* data) -> ImTextureID;
    auto find_texture(const std::string& name) const -> ImTextureID;
    void update_texture(
      const std::string& name,
      const uint8_t* srcData,
      const vk::DeviceSize& srcSize);
    void remove_texture(const std::string& name);
  };

  imgui_context::impl::impl(
    vulkan::vulkan_context& vulkan_ctx,
    glfw::glfw_context::init_flags glfw_flags,
    uint32_t widt,
    uint32_t height,
    const char* windowName)
    : vulkanCtx {vulkan_ctx}
    , glfwCtx {glfw_flags}
    , glfwWindow {glfwCtx.create_window(widt, height, windowName)}
    , windowCtx {vulkanCtx, glfwWindow}
    , imCtx {ImGui::CreateContext()}
  {
    /* init ImGui */
    {
      IMGUI_CHECKVERSION();
      ImGui::StyleColorsDark();

      ImGuiIO& io = ImGui::GetIO();

      /* Enable docking */
      io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

      /* Enable viewport */
      io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

      /* setup vulkan binding */
      io.BackendRendererName = "yave::imgui_context";

      Info(g_logger, "Initialized ImGui context");
    }

    /* setup GLFW input binding */
    {
      ImGui_ImplGlfw_InitForVulkan(glfwWindow.get(), true);
    }

    /* register fonts */
    {
      // Add default font
      ImGuiIO& io = ImGui::GetIO();
      io.Fonts->AddFontDefault();
      io.Fonts->AddFontFromFileTTF(YAVE_TOSTR(YAVE_IMGUI_FONT_ROBOTO), 13);
    }

    /* build fonts with FreeType */
    {
      ImGuiIO& io       = ImGui::GetIO();
      auto raster_flags = ImGuiFreeType::RasterizerFlags::NoHinting;
      ImGuiFreeType::BuildFontAtlas(io.Fonts, raster_flags);
    }

    /* prepare uploading font texture */
    {
      fontSampler = createImGuiFontSampler(windowCtx.device());
      Info(g_logger, "Created ImGui font sampler");
    }

    {
      descriptorPool = createImGuiDescriptorPool(windowCtx.device());
      descriptorSetLayout =
        createImGuiDescriptorSetLayout(fontSampler.get(), windowCtx.device());
      Info(g_logger, "Created ImGui descriptor set");
    }

    {
      pipelineCache  = createPipelineCache(windowCtx.device());
      pipelineLayout = createImGuiPipelineLayout(
        descriptorSetLayout.get(), windowCtx.device());
      pipeline = createImGuiPipeline(
        windowCtx.swapchain_extent(),
        windowCtx.render_pass(),
        pipelineCache.get(),
        pipelineLayout.get(),
        windowCtx.device());

      Info(g_logger, "Created ImGui pipeline");
    }

    /* upload font texture */
    {
      auto cmd                             = windowCtx.single_time_command();
      auto [imageMemory, image, imageView] = createImGuiFontTexture(
        windowCtx, vulkanCtx.physical_device(), windowCtx.device());
      fontImageMemory = std::move(imageMemory);
      fontImage       = std::move(image);
      fontImageView   = std::move(imageView);

      Info(g_logger, "Uploaded ImGui font texture");
    }

    /* create default descriptor set (font texture) */
    {
      descriptorSet = createImGuiDescriptorSet(
        descriptorPool.get(), descriptorSetLayout.get(), windowCtx.device());

      vk::DescriptorImageInfo info;
      info.sampler     = fontSampler.get();
      info.imageView   = fontImageView.get();
      info.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;

      vk::WriteDescriptorSet write;
      write.dstSet          = descriptorSet.get();
      write.descriptorCount = 1;
      write.descriptorType  = vk::DescriptorType::eCombinedImageSampler;
      write.pImageInfo      = &info;

      windowCtx.device().updateDescriptorSets(write, {});

      // set font texture ID
      ImGui::GetIO().Fonts->TexID = toImTextureId(&descriptorSet.get());

      Info(g_logger, "Updated ImGui descriptor set");
    }

    // texture staging buffer
    texture_staging = vulkan::create_staging_buffer(
      1, windowCtx.device(), vulkanCtx.physical_device());

    // for rendering loop
    lastTime = std::chrono::high_resolution_clock::now();

    // default clear color
    clearColor = {0.45f, 0.55f, 0.60f, 1.f};

    // default fps
    fps = glfwWindow.refresh_rate();
  }

  imgui_context::impl::~impl() noexcept
  {
    Info(g_logger, "Destroying ImGui context");
    // wait idle
    windowCtx.device().waitIdle();
    // unbind GLFW
    ImGui_ImplGlfw_Shutdown();
    // destroy ImGui
    ImGui::DestroyContext(imCtx);
  }

  void imgui_context::impl::set_current()
  {
    ImGui::SetCurrentContext(imCtx);
  }

  void imgui_context::impl::begin_frame()
  {
    /* start ImGui frame */
    {
      glfwPollEvents();
      ImGui_ImplGlfw_NewFrame();
      ImGui::NewFrame();
    }
  }

  void imgui_context::impl::end_frame()
  {
    /* end ImGui frame */
    {
      ImGui::EndFrame();
      ImGui::Render();
    }
  }

  void imgui_context::impl::render()
  {
    /* render Vulkan frame */

    // ImGui draw data
    const auto* drawData = ImGui::GetDrawData();

    assert(drawData->Valid);

    windowCtx.set_clear_color(
      clearColor[0], clearColor[1], clearColor[2], clearColor[3]);

    /* Render with Vulkan */
    {
      // begin new frame
      windowCtx.begin_frame();
      auto commandBuffer = windowCtx.begin_record();

      // device
      auto physicalDevice = vulkanCtx.physical_device();
      auto device         = windowCtx.device();

      /* Resize buffers vector to match current in-flight frames */
      {
        auto frameCount = windowCtx.frame_index_count();
        // vertec buffer
        if (vertexBuffers.size() > frameCount) {
          for (size_t i = 0; i < vertexBuffers.size() - frameCount; ++i)
            vertexBuffers.pop_back();
        } else {
          for (size_t i = 0; i < frameCount - vertexBuffers.size(); ++i)
            vertexBuffers.emplace_back(vk::BufferUsageFlagBits::eVertexBuffer);
        }
        // index buffer
        if (indexBuffers.size() > frameCount) {
          for (size_t i = 0; i < indexBuffers.size() - frameCount; ++i)
            indexBuffers.pop_back();
        } else {
          for (size_t i = 0; i < frameCount - indexBuffers.size(); ++i)
            indexBuffers.emplace_back(vk::BufferUsageFlagBits::eIndexBuffer);
        }
      }

      auto& vertexBuffer = vertexBuffers[windowCtx.frame_index()];
      auto& indexBuffer  = indexBuffers[windowCtx.frame_index()];

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
        pipelineLayout.get(),
        pipeline.get(),
        vertexBuffer.buffer(),
        indexBuffer.buffer(),
        windowCtx.swapchain_extent(),
        commandBuffer);

      /* Record draw commands */
      renderImGuiDrawData(
        drawData,
        descriptorSet.get(),
        pipelineLayout.get(),
        pipeline.get(),
        vertexBuffer.buffer(),
        indexBuffer.buffer(),
        windowCtx.swapchain_extent(),
        commandBuffer);

      windowCtx.end_record(commandBuffer);
      windowCtx.end_frame();
    }

    /* frame rate limiter */

    using namespace std::chrono_literals;
    auto endTime         = std::chrono::high_resolution_clock::now();
    auto frameTime       = (endTime - lastTime);
    auto frameTimeWindow = std::chrono::nanoseconds(1s) / fps;
    auto sleepTime       = frameTimeWindow - frameTime;

    if (sleepTime.count() > 0) {
      lastTime = endTime + sleepTime;
      std::this_thread::sleep_for(sleepTime);
    } else {
      lastTime = endTime;
    }
  }

  class ImGuiTextureDataHolder
  {
  public:
    ImGuiTextureDataHolder(vulkan::texture_data&& tex)
      : tex_data {std::move(tex)} {};

    vulkan::texture_data tex_data;

    ImTextureID get_texture()
    {
      return toImTextureId(&tex_data.dsc_set.get());
    }
  };

  auto imgui_context::impl::add_texture(
    const std::string& name,
    const vk::Extent2D& extent,
    const vk::DeviceSize& byte_size,
    const vk::Format& format,
    const uint8_t* data) -> ImTextureID
  {
    assert(data);

    if (textures.find(name) != textures.end()) {
      Error(g_logger, "Failed to add texture data: Texute name already used");
      return ImTextureID();
    }

    auto device         = windowCtx.device();
    auto physicalDevice = vulkanCtx.physical_device();
    auto commandPool    = windowCtx.command_pool();
    auto queue          = windowCtx.graphics_queue();
    auto descPool       = descriptorPool.get();
    auto descLayout     = descriptorSetLayout.get();

    auto& staging = texture_staging;

    auto texture = vulkan::create_texture_data(
      extent.width,
      extent.height,
      format,
      queue,
      commandPool,
      descPool,
      descLayout,
      vk::DescriptorType::eCombinedImageSampler,
      device,
      physicalDevice);

    vulkan::store_texture_data(
      staging,
      texture,
      (const std::byte*)data,
      byte_size,
      queue,
      commandPool,
      device,
      physicalDevice);

    auto texData = std::make_unique<ImGuiTextureDataHolder>(std::move(texture));

    auto [iter, succ] = textures.emplace(name, std::move(texData));

    assert(succ);

    Info(
      g_logger,
      "Added new texture \"{}\": width={}, height={}",
      name,
      extent.width,
      extent.height);

    return iter->second->get_texture();
  }

  auto imgui_context::impl::find_texture(const std::string& name) const
    -> ImTextureID
  {
    auto iter = textures.find(name);

    if (iter == textures.end())
      return ImTextureID();

    return iter->second->get_texture();
  }

  void imgui_context::impl::update_texture(
    const std::string& name,
    const uint8_t* srcData,
    const vk::DeviceSize& srcSize)
  {
    auto iter = textures.find(name);

    if (iter == textures.end()) {
      Error(g_logger, "Failed to update texture: no such texture");
      return;
    }

    auto device         = windowCtx.device();
    auto physicalDevice = vulkanCtx.physical_device();
    auto commandPool    = windowCtx.command_pool();
    auto queue          = windowCtx.graphics_queue();

    auto& staging = texture_staging;

    vulkan::store_texture_data(
      staging,
      iter->second->tex_data,
      (const std::byte*)srcData,
      srcSize,
      queue,
      commandPool,
      device,
      physicalDevice);
  }

  void imgui_context::impl::remove_texture(const std::string& name)
  {
    auto iter = textures.find(name);

    if (iter == textures.end())
      return;

    textures.erase(iter);

    Info(g_logger, "Destroyed texture \"{}\"", name);
  }

  auto imgui_context::_init_flags() noexcept -> init_flags
  {
    return init_flags::enable_logging;
  }

  imgui_context::imgui_context(
    vulkan::vulkan_context& vulkan_ctx,
    init_flags flags)
  {
    using namespace yave;
    init_logger();

    glfw::glfw_context::init_flags glfw_flags {0};

    if (!(flags & init_flags::enable_logging)) {
      g_logger->set_level(spdlog::level::off);
    }

    if (!!(flags & init_flags::enable_logging)) {
      glfw_flags |= glfw::glfw_context::init_flags::enable_logging;
    }

    // create context
    m_pimpl = std::make_unique<impl>(
      vulkan_ctx, glfw_flags, 1280, 720, "imgui_context");
  }

  imgui_context::~imgui_context() noexcept = default;

  void imgui_context::set_current()
  {
    m_pimpl->set_current();
  }

  void imgui_context::begin_frame()
  {
    m_pimpl->begin_frame();
  }

  void imgui_context::end_frame()
  {
    m_pimpl->end_frame();
  }

  void imgui_context::render()
  {
    m_pimpl->render();
  }

  auto imgui_context::get_texture_id(const vk::DescriptorSet& tex) const
    -> ImTextureID
  {
    return toImTextureId(&tex);
  }

  auto imgui_context::glfw_context() const -> const glfw::glfw_context&
  {
    return m_pimpl->glfwCtx;
  }

  auto imgui_context::glfw_context() -> glfw::glfw_context&
  {
    return m_pimpl->glfwCtx;
  }

  auto imgui_context::vulkan_context() const -> const vulkan::vulkan_context&
  {
    return m_pimpl->vulkanCtx;
  }

  auto imgui_context::vulkan_context() -> vulkan::vulkan_context&
  {
    return m_pimpl->vulkanCtx;
  }

  auto imgui_context::window_context() const -> const vulkan::window_context&
  {
    return m_pimpl->windowCtx;
  }

  auto imgui_context::window_context() -> vulkan::window_context&
  {
    return m_pimpl->windowCtx;
  }

  auto imgui_context::font_sampler() const -> vk::Sampler
  {
    return m_pimpl->fontSampler.get();
  }

  auto imgui_context::descriptor_pool() const -> vk::DescriptorPool
  {
    return m_pimpl->descriptorPool.get();
  }

  auto imgui_context::descriptor_set_layout() const -> vk::DescriptorSetLayout
  {
    return m_pimpl->descriptorSetLayout.get();
  }

  auto imgui_context::descriptor_set() const -> vk::DescriptorSet
  {
    return m_pimpl->descriptorSet.get();
  }

  auto imgui_context::pipeline_cache() const -> vk::PipelineCache
  {
    return m_pimpl->pipelineCache.get();
  }

  auto imgui_context::pipeline_layout() const -> vk::PipelineLayout
  {
    return m_pimpl->pipelineLayout.get();
  }

  auto imgui_context::pipeline() const -> vk::Pipeline
  {
    return m_pimpl->pipeline.get();
  }

  auto imgui_context::font_image_memory() const -> vk::DeviceMemory
  {
    return m_pimpl->fontImageMemory.get();
  }

  auto imgui_context::font_image() const -> vk::Image
  {
    return m_pimpl->fontImage.get();
  }

  auto imgui_context::font_image_view() const -> vk::ImageView
  {
    return m_pimpl->fontImageView.get();
  }

  auto imgui_context::add_texture(
    const std::string& name,
    const vk::Extent2D& extent,
    const vk::DeviceSize& byte_size,
    const vk::Format& format,
    const uint8_t* data) -> ImTextureID
  {
    return m_pimpl->add_texture(name, extent, byte_size, format, data);
  }

  auto imgui_context::find_texture(const std::string& name) const -> ImTextureID
  {
    return m_pimpl->find_texture(name);
  }

  void imgui_context::update_texture(
    const std::string& name,
    const uint8_t* srcData,
    const vk::DeviceSize& srcSize)
  {
    m_pimpl->update_texture(name, srcData, srcSize);
  }

  void imgui_context::remove_texture(const std::string& name)
  {
    m_pimpl->remove_texture(name);
  }

  auto imgui::imgui_context::get_clear_color() const -> std::array<float, 4>
  {
    return m_pimpl->clearColor;
  }

  void imgui::imgui_context::set_clear_color(float r, float g, float b, float a)
  {
    m_pimpl->clearColor = {r, g, b, a};
  }

  auto imgui::imgui_context::get_fps() const -> uint32_t
  {
    return m_pimpl->fps;
  }

  void imgui::imgui_context::set_fps(uint32_t fps)
  {
    m_pimpl->fps = fps;
  }
} // namespace yave::imgui