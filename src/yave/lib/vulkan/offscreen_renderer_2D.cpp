//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/lib/vulkan/offscreen_renderer_2D.hpp>
#include <yave/lib/vulkan/vulkan_util.hpp>
#include <yave/lib/vulkan/shader.hpp>
#include <yave/lib/vulkan/texture.hpp>
#include <yave/lib/vulkan/render_buffer.hpp>
#include <yave/lib/image/blend_operation.hpp>
#include <yave/support/log.hpp>
#include <glm/gtx/matrix_transform_2d.hpp>

YAVE_DECL_G_LOGGER(composition_pipeline_2D)

namespace {

  constexpr auto vert_shader = R"(
    #version 450 core

    layout(location = 0) in vec2 vertPos;
    layout(location = 1) in vec2 vertUV;
    layout(location = 2) in vec4 vertColor;
    
    layout(push_constant) uniform PushConstant {
      vec2 translate;
      vec2 scale;
    } pc;
    
    out gl_PerVertex {
      vec4 gl_Position;
    };
    
    layout(location = 0) out struct {
      vec4 color;
      vec2 uv;
    } fragIn;

    void main()
    {
      fragIn.color = vertColor;
      fragIn.uv    = vertUV;
      gl_Position  = vec4(vertPos * pc.scale + pc.translate, 0, 1);
    }
  )";

  constexpr auto frag_shader = R"(
    #version 450 core
    
    layout(set=0, binding=0) uniform sampler2D image;
    
    layout(location = 0) in struct {
      vec4 color;
      vec2 uv;
    } vertOut;
    
    layout(location = 0) out vec4 outColor;
    
    void main()
    {
      outColor = vertOut.color * texture(image, vertOut.uv.st);
    }
  )";

  using namespace yave;
  using namespace yave::vulkan;

  auto createTextureSampler(const vk::Device& device)
  {
    vk::SamplerCreateInfo info;
    info.magFilter    = vk::Filter::eLinear;
    info.minFilter    = vk::Filter::eLinear;
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
  }

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
    info.codeSize = code.size() * sizeof(uint32_t);
    info.pCode    = code.data();

    return device.createShaderModuleUnique(info);
  }

  auto createColorBlendState(blend_operation op)
  {
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
        dstFactor = vk::BlendFactor::eZero;
        break;
      case blend_operation::dst:
        srcFactor = vk::BlendFactor::eZero;
        dstFactor = vk::BlendFactor::eOne;
        break;
      case blend_operation::over:
        srcFactor = vk::BlendFactor::eSrcAlpha;
        dstFactor = vk::BlendFactor::eOneMinusSrcAlpha;
        break;
      case blend_operation::in:
        srcFactor = vk::BlendFactor::eDstAlpha;
        dstFactor = vk::BlendFactor::eZero;
        break;
      case blend_operation::out:
        srcFactor = vk::BlendFactor::eOneMinusDstAlpha;
        dstFactor = vk::BlendFactor::eZero;
        break;
      case blend_operation::add:
        srcFactor = vk::BlendFactor::eOne;
        dstFactor = vk::BlendFactor::eOne;
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
    vk::PushConstantRange pcr;
    pcr.stageFlags = vk::ShaderStageFlagBits::eVertex;
    pcr.size       = sizeof(draw2d_pc);

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
    // vertex shader
    vk::UniqueShaderModule vertShaderModule =
      createShaderModule(compileVertShader(vert_shader), device);

    vk::PipelineShaderStageCreateInfo vertShaderStageInfo;
    vertShaderStageInfo.stage  = vk::ShaderStageFlagBits::eVertex;
    vertShaderStageInfo.module = *vertShaderModule;
    vertShaderStageInfo.pName  = "main";

    // fragment shader
    vk::UniqueShaderModule fragShaderModule =
      createShaderModule(compileFragShader(frag_shader), device);

    vk::PipelineShaderStageCreateInfo fragShaderStageInfo;
    fragShaderStageInfo.stage  = vk::ShaderStageFlagBits::eFragment;
    fragShaderStageInfo.module = *fragShaderModule;
    fragShaderStageInfo.pName  = "main";

    // stages
    std::array shaderStages = {vertShaderStageInfo, fragShaderStageInfo};

    /* vertex input stage */

    std::array<vk::VertexInputBindingDescription, 1> vertBindingDesc;
    vertBindingDesc[0].stride    = sizeof(draw2d_vtx);
    vertBindingDesc[0].inputRate = vk::VertexInputRate::eVertex;

    std::array<vk::VertexInputAttributeDescription, 3> vertAttrDesc;
    // pos
    vertAttrDesc[0].location = 0;
    vertAttrDesc[0].binding  = vertBindingDesc[0].binding;
    vertAttrDesc[0].format   = vk::Format::eR32G32Sfloat;
    vertAttrDesc[0].offset   = offsetof(draw2d_vtx, pos);
    // uv
    vertAttrDesc[1].location = 1;
    vertAttrDesc[1].binding  = vertBindingDesc[0].binding;
    vertAttrDesc[1].format   = vk::Format::eR32G32Sfloat;
    vertAttrDesc[1].offset   = offsetof(draw2d_vtx, uv);
    // col
    vertAttrDesc[2].location = 2;
    vertAttrDesc[2].binding  = vertBindingDesc[0].binding;
    vertAttrDesc[2].format   = vk::Format::eR32G32B32A32Sfloat;
    vertAttrDesc[2].offset   = offsetof(draw2d_vtx, col);

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
    colAttachments[0] = createColorBlendState(blend_operation::over);

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

  void storeRenderDataToBuffer(
    render_buffer& vtx_buff,
    render_buffer& idx_buff,
    const draw2d_data& draw_data,
    const vk::Device& device)
  {
    auto* mapped_vtx_buff = map_render_buffer(vtx_buff, device);
    auto* mapped_idx_buff = map_render_buffer(idx_buff, device);

    size_t vtx_offset = 0;
    size_t idx_offset = 0;
    for (auto&& dl : draw_data.draw_lists) {

      auto vtx_size = dl.vtx_buffer.size() * sizeof(draw2d_vtx);
      auto idx_size = dl.idx_buffer.size() * sizeof(draw2d_idx);

      std::memcpy(mapped_vtx_buff + vtx_offset, dl.vtx_buffer.data(), vtx_size);
      std::memcpy(mapped_idx_buff + idx_offset, dl.idx_buffer.data(), idx_size);

      vtx_offset += vtx_size;
      idx_offset += idx_size;
    }

    unmap_render_buffer(vtx_buff, device);
    unmap_render_buffer(idx_buff, device);
  }

  void prepareRenderData(
    render_buffer& vtx_buff,
    render_buffer& idx_buff,
    const draw2d_data& draw_data,
    const vk::Device& device,
    const vk::PhysicalDevice& physicalDevice)
  {
    auto vtx_buff_size = draw_data.total_vtx_count() * sizeof(draw2d_vtx);
    auto idx_buff_size = draw_data.total_idx_count() * sizeof(draw2d_idx);

    // reserve render buffer
    resize_render_buffer(idx_buff, idx_buff_size, device, physicalDevice);
    resize_render_buffer(vtx_buff, vtx_buff_size, device, physicalDevice);

    // write to render buffer
    storeRenderDataToBuffer(vtx_buff, idx_buff, draw_data, device);
  }

  void initRenderPipeline(
    vk::CommandBuffer& cmd,
    const render_buffer& vtx_buff,
    const render_buffer& idx_buff,
    const vk::Viewport& viewport,
    const vk::Pipeline& pipeline)
  {
    // init pipeline
    static_assert(sizeof(draw2d_idx) == sizeof(uint16_t));
    cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline);
    cmd.bindVertexBuffers(0, vtx_buff.buffer.get(), {0});
    cmd.bindIndexBuffer(idx_buff.buffer.get(), 0, vk::IndexType::eUint16);
    cmd.setViewport(0, viewport);
  }

  void renderDrawData(
    vk::CommandBuffer& cmd,
    const draw2d_data& draw_data,
    const vk::Viewport& viewport,
    const vk::DescriptorSet& defaultDsc,
    const vk::PipelineLayout& pipelineLayout)
  {
    // set push constant
    {
      // transform (0,0):(w, h) -> (-1,-1):(1,1)
      draw2d_pc pc;
      pc.scale     = glm::vec2(2.f / viewport.width, 2.f / viewport.height);
      pc.translate = glm::vec2(-1, -1);

      cmd.pushConstants(
        pipelineLayout,
        vk::ShaderStageFlagBits::eVertex,
        0,
        sizeof(draw2d_pc),
        &pc);
    }

    // indexed render
    for (auto&& dl : draw_data.draw_lists) {

      uint32_t vtxOffset = 0;
      uint32_t idxOffset = 0;

      for (auto&& dc : dl.cmd_buffer) {

        // texture
        {
          auto dsc = dc.tex.descriptor_set;
          // default when empty
          if (!dsc)
            dsc = defaultDsc;
          // bind
          cmd.bindDescriptorSets(
            vk::PipelineBindPoint::eGraphics, pipelineLayout, 0, dsc, {});
        }

        {
          // scissor rect (frame buffer coord)
          vk::Rect2D scissor;
          scissor.offset.x      = dc.clip.p1.x;
          scissor.offset.y      = dc.clip.p1.y;
          scissor.extent.width  = dc.clip.p2.x;
          scissor.extent.height = dc.clip.p2.y;
          cmd.setScissor(0, scissor);
        }

        // draw
        cmd.drawIndexed(
          dc.idx_count,
          1,
          idxOffset + dc.idx_offset,
          vtxOffset + dc.vtx_offset,
          0);
      }
      idxOffset += dl.idx_buffer.size();
      vtxOffset += dl.vtx_buffer.size();
    }
  }
} // namespace

namespace yave::vulkan {

  class rgba32f_offscreen_renderer_2D::impl
  {
  public:
    //  ref
    vulkan_context& vulkan_ctx;
    // owning
    offscreen_context offscreen_ctx;
    // owning
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
    texture_data default_texture;
    std::vector<texture_data> textures;

  public:
    render_buffer vtx_buff;
    render_buffer idx_buff;

  public:
    impl(uint32_t width, uint32_t height, vulkan_context& ctx)
      : vulkan_ctx {ctx}
      , offscreen_ctx {ctx}
      , render_pass {width, height, offscreen_ctx}
    {
      init_logger();

      auto physicalDevice = vulkan_ctx.physical_device();
      auto device         = offscreen_ctx.device();
      auto graphicsQueue  = offscreen_ctx.graphics_queue();
      auto commandPool    = render_pass.command_pool();

      // clang-format off
      texture_sampler       = createTextureSampler(device);
      descriptor_set_layout = createDescriptorSetLayout(texture_sampler.get(), device);
      descriptor_pool       = createDescriptorPool(device);
      pipeline_cache        = createPipelineCache(device);
      pipeline_layout       = createPipelineLayout(descriptor_set_layout.get(), device);
      pipeline              = createPipeline(render_pass.frame_extent(), render_pass.render_pass(), pipeline_cache.get(), pipeline_layout.get(), device);
      // clang-format on

      default_texture = create_texture_data(
        1,
        1,
        vk::Format::eR32G32B32A32Sfloat,
        graphicsQueue,
        commandPool,
        descriptor_pool.get(),
        descriptor_set_layout.get(),
        vk::DescriptorType::eCombinedImageSampler,
        device,
        physicalDevice);

      clear_texture_data(
        default_texture,
        vk::ClearColorValue(std::array {1.f, 1.f, 1.f, 1.f}),
        graphicsQueue,
        commandPool,
        device,
        physicalDevice);

      texture_staging = create_staging_buffer(1, device, physicalDevice);

      vtx_buff = create_render_buffer(
        vk::BufferUsageFlagBits::eVertexBuffer, 0, device, physicalDevice);

      idx_buff = create_render_buffer(
        vk::BufferUsageFlagBits::eIndexBuffer, 0, device, physicalDevice);
    }

    ~impl() noexcept
    {
    }

    void render(const draw2d_data& draw_data)
    {
      auto cmd = render_pass.begin_pass();
      {
        prepareRenderData(
          vtx_buff,
          idx_buff,
          draw_data,
          offscreen_ctx.device(),
          vulkan_ctx.physical_device());

        // always render full size of frame
        auto viewport =
          vk::Viewport(0, 0, render_pass.width(), render_pass.height(), 0, 1);

        initRenderPipeline(cmd, vtx_buff, idx_buff, viewport, pipeline.get());

        renderDrawData(
          cmd,
          draw_data,
          viewport,
          default_texture.dsc_set.get(),
          pipeline_layout.get());
      }
      render_pass.end_pass();
    }

    auto get_default_texture()
    {
      return draw2d_tex {default_texture.dsc_set.get()};
    }

    auto add_texture(const boost::gil::rgba32fc_view_t& view) -> draw2d_tex
    {
      auto physicalDevice = vulkan_ctx.physical_device();
      auto commandPool    = render_pass.command_pool();
      auto device         = offscreen_ctx.device();
      auto graphicsQueue  = offscreen_ctx.graphics_queue();

      auto tex = create_texture_data(
        view.width(),
        view.height(),
        vk::Format::eR32G32B32A32Sfloat,
        graphicsQueue,
        commandPool,
        descriptor_pool.get(),
        descriptor_set_layout.get(),
        vk::DescriptorType::eCombinedImageSampler,
        device,
        physicalDevice);

      store_texture_data(
        texture_staging,
        tex,
        (const std::byte*)view.row_begin(0),
        view.size() * sizeof(boost::gil::rgba32f_pixel_t),
        graphicsQueue,
        commandPool,
        device,
        physicalDevice);

      Info(g_logger, "Added texture: {},{}", view.width(), view.height());

      draw2d_tex ret {tex.dsc_set.get()};
      textures.push_back(std::move(tex));
      return ret;
    }

    void remove_texture(const draw2d_tex& tex)
    {
      auto end =
        std::remove_if(textures.begin(), textures.end(), [&](auto& td) {
          return td.dsc_set.get() == tex.descriptor_set;
        });
      textures.erase(end, textures.end());
    }
  };

  rgba32f_offscreen_renderer_2D::rgba32f_offscreen_renderer_2D(
    uint32_t width,
    uint32_t height,
    vulkan_context& ctx)
    : m_pimpl {std::make_unique<impl>(width, height, ctx)}
  {
  }

  rgba32f_offscreen_renderer_2D::~rgba32f_offscreen_renderer_2D() noexcept
  {
  }

  auto rgba32f_offscreen_renderer_2D::width() const noexcept -> uint32_t
  {
    return m_pimpl->render_pass.width();
  }

  auto rgba32f_offscreen_renderer_2D::height() const noexcept -> uint32_t
  {
    return m_pimpl->render_pass.height();
  }

  auto rgba32f_offscreen_renderer_2D::format() const noexcept -> image_format
  {
    return m_pimpl->render_pass.format();
  }

  void rgba32f_offscreen_renderer_2D::store_frame(
    const boost::gil::rgba32fc_view_t& view)
  {
    return m_pimpl->render_pass.store_frame(view);
  }

  void rgba32f_offscreen_renderer_2D::load_frame(
    const boost::gil::rgba32f_view_t& view) const
  {
    return m_pimpl->render_pass.load_frame(view);
  }

  void rgba32f_offscreen_renderer_2D::render(const draw2d_data& draw_data)
  {
    m_pimpl->render(draw_data);
  }

  auto rgba32f_offscreen_renderer_2D::default_texture() const -> draw2d_tex
  {
    return m_pimpl->get_default_texture();
  }

  auto rgba32f_offscreen_renderer_2D::add_texture(
    const boost::gil::rgba32fc_view_t& view) -> draw2d_tex
  {
    return m_pimpl->add_texture(view);
  }

  void rgba32f_offscreen_renderer_2D::remove_texture(const draw2d_tex& tex)
  {
    m_pimpl->remove_texture(tex);
  }
} // namespace yave::vulkan