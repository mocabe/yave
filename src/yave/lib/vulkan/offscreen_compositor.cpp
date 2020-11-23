//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/lib/vulkan/offscreen_compositor.hpp>
#include <yave/lib/vulkan/vulkan_util.hpp>
#include <yave/lib/vulkan/shader.hpp>
#include <yave/lib/vulkan/texture.hpp>
#include <yave/lib/vulkan/render_buffer.hpp>
#include <yave/lib/image/blend_operation.hpp>
#include <yave/support/log.hpp>
#include <glm/gtx/matrix_transform_2d.hpp>

namespace {

  // // file: offscreen_compositor.vert
  //
  // #version 450 core
  //
  // vec2 pos[6] = vec2[](
  //   vec2(-1, -1),
  //   vec2(-1,  1),
  //   vec2( 1,  1),
  //   vec2(-1, -1),
  //   vec2( 1,  1),
  //   vec2( 1, -1)
  // );
  //
  // vec2 uv[6] = vec2[](
  //   vec2(0, 0),
  //   vec2(0, 1),
  //   vec2(1, 1),
  //   vec2(0, 0),
  //   vec2(1, 1),
  //   vec2(1, 0)
  // );
  //
  // layout(location = 0) out vec2 outUV;
  //
  // void main()
  // {
  //   outUV       = uv[gl_VertexIndex];
  //   gl_Position = vec4(pos[gl_VertexIndex], 0, 1);
  // }
  constexpr uint32_t offscreen_compositor_vert_spv[] = {
    0x07230203, 0x00010000, 0x00080008, 0x00000032, 0x00000000, 0x00020011,
    0x00000001, 0x0006000B, 0x00000001, 0x4C534C47, 0x6474732E, 0x3035342E,
    0x00000000, 0x0003000E, 0x00000000, 0x00000001, 0x0008000F, 0x00000000,
    0x00000004, 0x6E69616D, 0x00000000, 0x0000001B, 0x0000001E, 0x00000028,
    0x00030003, 0x00000002, 0x000001C2, 0x00040005, 0x00000004, 0x6E69616D,
    0x00000000, 0x00030005, 0x0000000C, 0x00736F70, 0x00030005, 0x00000014,
    0x00007675, 0x00040005, 0x0000001B, 0x5574756F, 0x00000056, 0x00060005,
    0x0000001E, 0x565F6C67, 0x65747265, 0x646E4978, 0x00007865, 0x00060005,
    0x00000026, 0x505F6C67, 0x65567265, 0x78657472, 0x00000000, 0x00060006,
    0x00000026, 0x00000000, 0x505F6C67, 0x7469736F, 0x006E6F69, 0x00070006,
    0x00000026, 0x00000001, 0x505F6C67, 0x746E696F, 0x657A6953, 0x00000000,
    0x00070006, 0x00000026, 0x00000002, 0x435F6C67, 0x4470696C, 0x61747369,
    0x0065636E, 0x00070006, 0x00000026, 0x00000003, 0x435F6C67, 0x446C6C75,
    0x61747369, 0x0065636E, 0x00030005, 0x00000028, 0x00000000, 0x00040047,
    0x0000001B, 0x0000001E, 0x00000000, 0x00040047, 0x0000001E, 0x0000000B,
    0x0000002A, 0x00050048, 0x00000026, 0x00000000, 0x0000000B, 0x00000000,
    0x00050048, 0x00000026, 0x00000001, 0x0000000B, 0x00000001, 0x00050048,
    0x00000026, 0x00000002, 0x0000000B, 0x00000003, 0x00050048, 0x00000026,
    0x00000003, 0x0000000B, 0x00000004, 0x00030047, 0x00000026, 0x00000002,
    0x00020013, 0x00000002, 0x00030021, 0x00000003, 0x00000002, 0x00030016,
    0x00000006, 0x00000020, 0x00040017, 0x00000007, 0x00000006, 0x00000002,
    0x00040015, 0x00000008, 0x00000020, 0x00000000, 0x0004002B, 0x00000008,
    0x00000009, 0x00000006, 0x0004001C, 0x0000000A, 0x00000007, 0x00000009,
    0x00040020, 0x0000000B, 0x00000006, 0x0000000A, 0x0004003B, 0x0000000B,
    0x0000000C, 0x00000006, 0x0004002B, 0x00000006, 0x0000000D, 0xBF800000,
    0x0005002C, 0x00000007, 0x0000000E, 0x0000000D, 0x0000000D, 0x0004002B,
    0x00000006, 0x0000000F, 0x3F800000, 0x0005002C, 0x00000007, 0x00000010,
    0x0000000D, 0x0000000F, 0x0005002C, 0x00000007, 0x00000011, 0x0000000F,
    0x0000000F, 0x0005002C, 0x00000007, 0x00000012, 0x0000000F, 0x0000000D,
    0x0009002C, 0x0000000A, 0x00000013, 0x0000000E, 0x00000010, 0x00000011,
    0x0000000E, 0x00000011, 0x00000012, 0x0004003B, 0x0000000B, 0x00000014,
    0x00000006, 0x0004002B, 0x00000006, 0x00000015, 0x00000000, 0x0005002C,
    0x00000007, 0x00000016, 0x00000015, 0x00000015, 0x0005002C, 0x00000007,
    0x00000017, 0x00000015, 0x0000000F, 0x0005002C, 0x00000007, 0x00000018,
    0x0000000F, 0x00000015, 0x0009002C, 0x0000000A, 0x00000019, 0x00000016,
    0x00000017, 0x00000011, 0x00000016, 0x00000011, 0x00000018, 0x00040020,
    0x0000001A, 0x00000003, 0x00000007, 0x0004003B, 0x0000001A, 0x0000001B,
    0x00000003, 0x00040015, 0x0000001C, 0x00000020, 0x00000001, 0x00040020,
    0x0000001D, 0x00000001, 0x0000001C, 0x0004003B, 0x0000001D, 0x0000001E,
    0x00000001, 0x00040020, 0x00000020, 0x00000006, 0x00000007, 0x00040017,
    0x00000023, 0x00000006, 0x00000004, 0x0004002B, 0x00000008, 0x00000024,
    0x00000001, 0x0004001C, 0x00000025, 0x00000006, 0x00000024, 0x0006001E,
    0x00000026, 0x00000023, 0x00000006, 0x00000025, 0x00000025, 0x00040020,
    0x00000027, 0x00000003, 0x00000026, 0x0004003B, 0x00000027, 0x00000028,
    0x00000003, 0x0004002B, 0x0000001C, 0x00000029, 0x00000000, 0x00040020,
    0x00000030, 0x00000003, 0x00000023, 0x00050036, 0x00000002, 0x00000004,
    0x00000000, 0x00000003, 0x000200F8, 0x00000005, 0x0003003E, 0x0000000C,
    0x00000013, 0x0003003E, 0x00000014, 0x00000019, 0x0004003D, 0x0000001C,
    0x0000001F, 0x0000001E, 0x00050041, 0x00000020, 0x00000021, 0x00000014,
    0x0000001F, 0x0004003D, 0x00000007, 0x00000022, 0x00000021, 0x0003003E,
    0x0000001B, 0x00000022, 0x0004003D, 0x0000001C, 0x0000002A, 0x0000001E,
    0x00050041, 0x00000020, 0x0000002B, 0x0000000C, 0x0000002A, 0x0004003D,
    0x00000007, 0x0000002C, 0x0000002B, 0x00050051, 0x00000006, 0x0000002D,
    0x0000002C, 0x00000000, 0x00050051, 0x00000006, 0x0000002E, 0x0000002C,
    0x00000001, 0x00070050, 0x00000023, 0x0000002F, 0x0000002D, 0x0000002E,
    0x00000015, 0x0000000F, 0x00050041, 0x00000030, 0x00000031, 0x00000028,
    0x00000029, 0x0003003E, 0x00000031, 0x0000002F, 0x000100FD, 0x00010038,
  };

  // // file: offscreen_compositor.frag
  //
  // #version 450 core
  //
  // layout(set=0, binding=0) uniform sampler2D image;
  //
  // layout(location = 0) in vec2 uv;
  //
  // layout(location = 0) out vec4 outColor;
  //
  // void main()
  // {
  //   outColor = texture(image, uv.st);
  // }
  constexpr uint32_t offscreen_compositor_frag_spv[] = {
    0x07230203, 0x00010000, 0x00080008, 0x00000014, 0x00000000, 0x00020011,
    0x00000001, 0x0006000B, 0x00000001, 0x4C534C47, 0x6474732E, 0x3035342E,
    0x00000000, 0x0003000E, 0x00000000, 0x00000001, 0x0007000F, 0x00000004,
    0x00000004, 0x6E69616D, 0x00000000, 0x00000009, 0x00000011, 0x00030010,
    0x00000004, 0x00000007, 0x00030003, 0x00000002, 0x000001C2, 0x00040005,
    0x00000004, 0x6E69616D, 0x00000000, 0x00050005, 0x00000009, 0x4374756F,
    0x726F6C6F, 0x00000000, 0x00040005, 0x0000000D, 0x67616D69, 0x00000065,
    0x00030005, 0x00000011, 0x00007675, 0x00040047, 0x00000009, 0x0000001E,
    0x00000000, 0x00040047, 0x0000000D, 0x00000022, 0x00000000, 0x00040047,
    0x0000000D, 0x00000021, 0x00000000, 0x00040047, 0x00000011, 0x0000001E,
    0x00000000, 0x00020013, 0x00000002, 0x00030021, 0x00000003, 0x00000002,
    0x00030016, 0x00000006, 0x00000020, 0x00040017, 0x00000007, 0x00000006,
    0x00000004, 0x00040020, 0x00000008, 0x00000003, 0x00000007, 0x0004003B,
    0x00000008, 0x00000009, 0x00000003, 0x00090019, 0x0000000A, 0x00000006,
    0x00000001, 0x00000000, 0x00000000, 0x00000000, 0x00000001, 0x00000000,
    0x0003001B, 0x0000000B, 0x0000000A, 0x00040020, 0x0000000C, 0x00000000,
    0x0000000B, 0x0004003B, 0x0000000C, 0x0000000D, 0x00000000, 0x00040017,
    0x0000000F, 0x00000006, 0x00000002, 0x00040020, 0x00000010, 0x00000001,
    0x0000000F, 0x0004003B, 0x00000010, 0x00000011, 0x00000001, 0x00050036,
    0x00000002, 0x00000004, 0x00000000, 0x00000003, 0x000200F8, 0x00000005,
    0x0004003D, 0x0000000B, 0x0000000E, 0x0000000D, 0x0004003D, 0x0000000F,
    0x00000012, 0x00000011, 0x00050057, 0x00000007, 0x00000013, 0x0000000E,
    0x00000012, 0x0003003E, 0x00000009, 0x00000013, 0x000100FD, 0x00010038,
  };

  using namespace yave;
  using namespace yave::vulkan;

  auto createTextureSampler(const vk::Device& device)
  {
    // use nearest filters since we always pass textuer which has same size
    // to frame buffer.
    vk::SamplerCreateInfo info;
    info.magFilter    = vk::Filter::eNearest;
    info.minFilter    = vk::Filter::eNearest;
    info.mipmapMode   = vk::SamplerMipmapMode::eNearest;
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
  }

  auto createDescriptorPool(const vk::Device& device)
  {
    std::array poolSizes = {
      vk::DescriptorPoolSize {vk::DescriptorType::eCombinedImageSampler, 1}};

    vk::DescriptorPoolCreateInfo info;
    info.flags         = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet;
    info.maxSets       = poolSizes.size();
    info.poolSizeCount = poolSizes.size();
    info.pPoolSizes    = poolSizes.data();

    return device.createDescriptorPoolUnique(info);
  }

  auto createDescriptorSet(
    const vk::ImageView& view,
    const vk::DescriptorPool& pool,
    const vk::DescriptorSetLayout& layout,
    const vk::Device& device) -> vk::UniqueDescriptorSet
  {
    vk::DescriptorSetAllocateInfo dscInfo;
    dscInfo.descriptorPool     = pool;
    dscInfo.descriptorSetCount = 1;
    dscInfo.pSetLayouts        = &layout;

    auto set = std::move(device.allocateDescriptorSetsUnique(dscInfo).front());

    vk::DescriptorImageInfo imgInfo;
    imgInfo.imageView   = view;
    imgInfo.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;

    vk::WriteDescriptorSet write;
    write.dstSet          = set.get();
    write.descriptorCount = 1;
    write.descriptorType  = vk::DescriptorType::eCombinedImageSampler;
    write.pImageInfo      = &imgInfo;

    device.updateDescriptorSets(write, {});

    return set;
  }

  auto createPipelineCache(const vk::Device& device)
  {
    vk::PipelineCacheCreateInfo info;
    return device.createPipelineCacheUnique(info);
  }

  template <size_t N>
  auto createShaderModule(const uint32_t (&code)[N], const vk::Device& device)
  {
    vk::ShaderModuleCreateInfo info;
    info.codeSize = sizeof(uint32_t) * N;
    info.pCode    = code;

    return device.createShaderModuleUnique(info);
  }

  auto createCompositionPipelineLayout(
    const vk::DescriptorSetLayout& setLayout,
    const vk::Device& device)
  {
    vk::PipelineLayoutCreateInfo info;
    info.setLayoutCount = 1;
    info.pSetLayouts    = &setLayout;

    return device.createPipelineLayoutUnique(info);
  }

  auto createCompositionPipeline(
    const blend_operation& blend_op,
    const vk::Extent2D& imageExtent,
    const vk::RenderPass& renderPass,
    const vk::PipelineCache& pipelineCache,
    const vk::PipelineLayout& pipelineLayout,
    const vk::Device& device)
  {
    // vertex shader
    vk::UniqueShaderModule vertShaderModule =
      createShaderModule(offscreen_compositor_vert_spv, device);

    vk::PipelineShaderStageCreateInfo vertShaderStageInfo;
    vertShaderStageInfo.stage  = vk::ShaderStageFlagBits::eVertex;
    vertShaderStageInfo.module = *vertShaderModule;
    vertShaderStageInfo.pName  = "main";

    // fragment shader
    vk::UniqueShaderModule fragShaderModule =
      createShaderModule(offscreen_compositor_frag_spv, device);

    vk::PipelineShaderStageCreateInfo fragShaderStageInfo;
    fragShaderStageInfo.stage  = vk::ShaderStageFlagBits::eFragment;
    fragShaderStageInfo.module = *fragShaderModule;
    fragShaderStageInfo.pName  = "main";

    // stages
    std::array shaderStages = {vertShaderStageInfo, fragShaderStageInfo};

    /* vertex input */

    vk::PipelineVertexInputStateCreateInfo vertInputStateInfo;

    /* input assembler */

    vk::PipelineInputAssemblyStateCreateInfo inputAsmStateInfo;
    inputAsmStateInfo.topology = vk::PrimitiveTopology::eTriangleList;

    /* viewport */

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
    colAttachments[0] = convert_to_blend_state(blend_op);

    vk::PipelineColorBlendStateCreateInfo colorBlendStateInfo;
    colorBlendStateInfo.attachmentCount = colAttachments.size();
    colorBlendStateInfo.pAttachments    = colAttachments.data();

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
    info.renderPass          = renderPass;
    info.layout              = pipelineLayout;

    return device.createGraphicsPipelineUnique(pipelineCache, info).value;
  }

} // namespace

namespace yave {

  class rgba32f_offscreen_compositor::impl
  {
  public:
    offscreen_context& offscreen_ctx;
    vulkan_context& vulkan_ctx;

  public:
    rgba32f_offscreen_render_pass render_pass;

  public:
    vk::UniqueSampler texture_sampler;
    vk::UniqueDescriptorSetLayout descriptor_set_layout;
    vk::UniqueDescriptorPool descriptor_pool;

  public:
    vk::UniquePipelineCache pipeline_cache;
    vk::UniquePipelineLayout pipeline_layout;
    vk::UniquePipeline pipeline;

  public:
    staging_buffer texture_staging;

  public:
    impl(uint32_t width, uint32_t height, offscreen_context& ctx)
      : offscreen_ctx {ctx}
      , vulkan_ctx {ctx.vulkan_ctx()}
      , render_pass {width, height, offscreen_ctx}
    {
      auto physicalDevice = vulkan_ctx.physical_device();
      auto device         = offscreen_ctx.device();

      texture_sampler = createTextureSampler(device);

      descriptor_set_layout =
        createDescriptorSetLayout(texture_sampler.get(), device);

      descriptor_pool = createDescriptorPool(device);

      pipeline_cache = createPipelineCache(device);

      pipeline_layout =
        createCompositionPipelineLayout(descriptor_set_layout.get(), device);

      pipeline = createCompositionPipeline(
        blend_operation::over,
        render_pass.frame_extent(),
        render_pass.render_pass(),
        pipeline_cache.get(),
        pipeline_layout.get(),
        device);

      // create staging
      texture_staging = create_staging_buffer(1, device, physicalDevice);
    }

    ~impl() noexcept
    {
      offscreen_ctx.device().waitIdle();
    }

    void render(const vk::DescriptorSet& dsc)
    {
      auto cmd = render_pass.begin_pass();
      {
        // init pipeline
        cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline.get());

        // texture
        cmd.bindDescriptorSets(
          vk::PipelineBindPoint::eGraphics, pipeline_layout.get(), 0, dsc, {});

        // draw
        cmd.draw(6, 1, 0, 0);
      }
      render_pass.end_pass();
    }

    void compose_over(const texture_data& tex)
    {
      // bind
      auto dsc = createDescriptorSet(
        tex.view.get(),
        descriptor_pool.get(),
        descriptor_set_layout.get(),
        offscreen_ctx.device());

      // compose
      render(dsc.get());

      // wait
      render_pass.wait_draw();
    }

    void compose_source(const texture_data& tex)
    {
      // TODO: use clear command in draw command
      render_pass.clear_frame({});

      // draw
      compose_over(tex);
    }
  };

  rgba32f_offscreen_compositor::rgba32f_offscreen_compositor(
    uint32_t width,
    uint32_t height,
    offscreen_context& ctx)
    : m_pimpl {std::make_unique<impl>(width, height, ctx)}
  {
  }

  rgba32f_offscreen_compositor::~rgba32f_offscreen_compositor() noexcept =
    default;

  auto rgba32f_offscreen_compositor::render_pass() const
    -> const rgba32f_offscreen_render_pass&
  {
    return m_pimpl->render_pass;
  }

  auto rgba32f_offscreen_compositor::render_pass()
    -> rgba32f_offscreen_render_pass&
  {
    return m_pimpl->render_pass;
  }

  void rgba32f_offscreen_compositor::compose_source(const texture_data& tex)
  {
    m_pimpl->compose_source(tex);
  }

  void rgba32f_offscreen_compositor::compose_over(const texture_data& tex)
  {
    m_pimpl->compose_over(tex);
  }
} // namespace yave