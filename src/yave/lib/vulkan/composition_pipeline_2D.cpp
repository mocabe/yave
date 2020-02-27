//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/lib/vulkan/composition_pipeline_2D.hpp>
#include <yave/lib/image/blend_operation.hpp> 
#include <yave/support/log.hpp>

YAVE_DECL_G_LOGGER(composition_pipeline_2D)

namespace {

  // types
  using vert = yave::vulkan::rgba32f_composition_pipeline_2D::vert;
  using pc   = yave::vulkan::rgba32f_composition_pipeline_2D::pc;

  auto createTextureSampler(const vk::Device& device)
  {
    vk::SamplerCreateInfo info;
    info.magFilter    = vk::Filter::eCubicEXT;
    info.minFilter    = vk::Filter::eCubicEXT;
    info.mipmapMode   = vk::SamplerMipmapMode::eLinear;
    info.addressModeU = vk::SamplerAddressMode::eRepeat;
    info.addressModeV = vk::SamplerAddressMode::eRepeat;
    info.addressModeW = vk::SamplerAddressMode::eRepeat;

    return device.createSamplerUnique(info);
  }

  auto createDescriptorSetLayout(
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
  };

  auto createDescriptorPool(const vk::Device& device)
  {
    // FIXME: Pool sizes are totally heuristic. Need better design to handle.
    std::array poolSizes = {
      vk::DescriptorPoolSize {vk::DescriptorType::eCombinedImageSampler, 100}};

    vk::DescriptorPoolCreateInfo info;
    info.flags         = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet;
    info.maxSets       = 100 * poolSizes.size();
    info.poolSizeCount = poolSizes.size();
    info.pPoolSizes    = poolSizes.data();

    return device.createDescriptorPoolUnique(info);
  }

  auto createPipelineCache(const vk::Device& device)
  {
    vk::PipelineCacheCreateInfo info;
    return device.createPipelineCacheUnique(info);
  }

  auto createShaderModule(
    const std::vector<uint32_t>& code,
    const vk::Device& device)
  {
    vk::ShaderModuleCreateInfo info;
    info.codeSize = code.size();
    info.pCode    = code.data();

    return device.createShaderModuleUnique(info);
  }

  auto createColorBlendState(yave::blend_operation op)
  {
    using namespace yave;

    // blending: expect premultiplied alpha
    vk::PipelineColorBlendAttachmentState state;

    state.blendEnable = true;
    state.colorWriteMask =
      vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG
      | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;

    vk::BlendFactor srcFactor = vk::BlendFactor::eSrcAlpha;
    vk::BlendFactor dstFactor = vk::BlendFactor::eOneMinusSrcAlpha;

    switch (op) {
      case blend_operation::src:
        srcFactor = vk::BlendFactor::eOne;
        srcFactor = vk::BlendFactor::eZero;
        break;
      case blend_operation::dst:
        srcFactor = vk::BlendFactor::eZero;
        srcFactor = vk::BlendFactor::eOne;
        break;
      case blend_operation::over:
        srcFactor = vk::BlendFactor::eSrcAlpha;
        srcFactor = vk::BlendFactor::eOneMinusSrcAlpha;
        break;
      case blend_operation::in:
        srcFactor = vk::BlendFactor::eDstAlpha;
        srcFactor = vk::BlendFactor::eZero;
        break;
      case blend_operation::out:
        srcFactor = vk::BlendFactor::eOneMinusDstAlpha;
        srcFactor = vk::BlendFactor::eZero;
        break;
      case blend_operation::add:
        srcFactor = vk::BlendFactor::eOne;
        srcFactor = vk::BlendFactor::eOne;
        break;
    }

    // color components
    state.srcColorBlendFactor = srcFactor;
    state.dstColorBlendFactor = dstFactor;
    state.colorBlendOp        = vk::BlendOp::eAdd;

    // alpha component
    state.srcAlphaBlendFactor = srcFactor;
    state.dstAlphaBlendFactor = dstFactor;
    state.alphaBlendOp        = vk::BlendOp::eAdd;

    return state;
  }

  auto createPipelineLayout(
    const vk::DescriptorSetLayout& setLayout,
    const vk::Device& device)
  {
    // Push constants:
    vk::PushConstantRange pcr;
    pcr.stageFlags = vk::ShaderStageFlagBits::eVertex;
    pcr.size       = sizeof(pc);

    vk::PipelineLayoutCreateInfo info;
    info.setLayoutCount         = 1;
    info.pSetLayouts            = &setLayout;
    info.pushConstantRangeCount = 1;
    info.pPushConstantRanges    = &pcr;

    return device.createPipelineLayoutUnique(info);
  }

  auto createPipeline(
    const vk::Extent2D& imageExtent,
    const vk::RenderPass& renderPass,
    const vk::PipelineCache& pipelineCache,
    const vk::PipelineLayout& pipelineLayout,
    const vk::Device& device)
  {
    /* shader stage */

    // vertex shader
    vk::UniqueShaderModule vertShaderModule =
      createShaderModule({/*TODO*/}, device);

    vk::PipelineShaderStageCreateInfo vertShaderStageInfo;
    vertShaderStageInfo.stage  = vk::ShaderStageFlagBits::eVertex;
    vertShaderStageInfo.module = *vertShaderModule;
    vertShaderStageInfo.pName  = "main";

    // fragment shader
    vk::UniqueShaderModule fragShaderModule =
      createShaderModule({/*TODO*/}, device);

    vk::PipelineShaderStageCreateInfo fragShaderStageInfo;
    fragShaderStageInfo.stage  = vk::ShaderStageFlagBits::eFragment;
    fragShaderStageInfo.module = *fragShaderModule;
    fragShaderStageInfo.pName  = "main";

    // stages
    std::array shaderStages = {vertShaderStageInfo, fragShaderStageInfo};

    /* vertex input stage */

    std::array<vk::VertexInputBindingDescription, 1> vertBindingDesc;
    vertBindingDesc[0].stride    = sizeof(vert);
    vertBindingDesc[0].inputRate = vk::VertexInputRate::eVertex;

    std::array<vk::VertexInputAttributeDescription, 3> vertAttrDesc;
    // pos
    vertAttrDesc[0].location = 0;
    vertAttrDesc[0].binding  = vertBindingDesc[0].binding;
    vertAttrDesc[0].format   = vk::Format::eR32G32Sfloat;
    vertAttrDesc[0].offset   = offsetof(vert, pos);
    // uv
    vertAttrDesc[1].location = 1;
    vertAttrDesc[1].binding  = vertBindingDesc[0].binding;
    vertAttrDesc[1].format   = vk::Format::eR32G32Sfloat;
    vertAttrDesc[1].offset   = offsetof(vert, uv);
    // col
    vertAttrDesc[2].location = 2;
    vertAttrDesc[2].binding  = vertBindingDesc[0].binding;
    vertAttrDesc[2].format   = vk::Format::eR32G32B32A32Sfloat;
    vertAttrDesc[2].offset   = offsetof(vert, col);

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
    viewports[0].width    = imageExtent.width;
    viewports[0].height   = imageExtent.height;
    viewports[0].maxDepth = 1.f;

    std::array<vk::Rect2D, 1> scissors;
    scissors[0].extent = imageExtent;

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
    // defualt: alpha blending
    colAttachments[0] = createColorBlendState(yave::blend_operation::over);

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

} // namespace

namespace yave::vulkan {

  class rgba32f_composition_pipeline_2D::impl
  {
  public:
    vulkan_context& context;
    rgba32f_composition_pass pass;

  public:
    vk::UniqueSampler texture_sampler;
    vk::UniqueDescriptorSetLayout descriptor_set_layout;
    vk::UniqueDescriptorPool descriptor_pool;

  public:
    vk::UniquePipelineCache pipeline_cache;
    vk::UniquePipelineLayout pipeline_layout;
    vk::UniquePipeline pipeline;

  public:
    impl(uint32_t width, uint32_t height, vulkan_context& ctx)
      : context {ctx}
      , pass {width, height, ctx}
    {
      init_logger();

      // clang-format off
      texture_sampler       = createTextureSampler(ctx.device());
      descriptor_set_layout = createDescriptorSetLayout(texture_sampler.get(), ctx.device());
      descriptor_pool       = createDescriptorPool(ctx.device());
      pipeline_cache        = createPipelineCache(ctx.device());
      pipeline_layout       = createPipelineLayout(descriptor_set_layout.get(), ctx.device());
      pipeline              = createPipeline(pass.frame_extent(), pass.render_pass(), pipeline_cache.get(), pipeline_layout.get(), ctx.device());
      // clang-format on
    }
  };

  rgba32f_composition_pipeline_2D::rgba32f_composition_pipeline_2D(
    uint32_t width,
    uint32_t height,
    vulkan_context& ctx)
    : m_pimpl {std::make_unique<impl>(width, height, ctx)}
  {
  }

  rgba32f_composition_pipeline_2D::~rgba32f_composition_pipeline_2D() noexcept
  {
  }
} // namespace yave::vulkan