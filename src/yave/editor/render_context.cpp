//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/editor/render_context.hpp>
#include <yave/editor/view_context.hpp>

#include <yave/lib/vulkan/shader.hpp>
#include <yave/lib/vulkan/render_buffer.hpp>
#include <yave/lib/vulkan/texture.hpp>
#include <yave/lib/vulkan/staging.hpp>

namespace yave::editor {

  // rendering
  namespace {

    using namespace yave::vulkan;

    // vertex shader
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

    // fragment shader
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

    auto to_draw_tex(vk::DescriptorSet dsc)
    {
      static_assert(sizeof(dsc) == sizeof(draw_tex));
      draw_tex ret;
      std::memcpy(&ret, &dsc, sizeof(ret));
      return ret;
    }

    auto from_draw_tex(draw_tex tex)
    {
      static_assert(sizeof(tex) == sizeof(vk::DescriptorSet));
      vk::DescriptorSet ret;
      std::memcpy(&ret, &tex, sizeof(ret));
      return ret;
    }

    auto create_pipeline_layout(
      const vk::DescriptorSetLayout& setLayout,
      const vk::Device& device)
    {
      vk::PushConstantRange pcr;
      pcr.stageFlags = vk::ShaderStageFlagBits::eVertex;
      pcr.size       = sizeof(draw_pc);

      vk::PipelineLayoutCreateInfo info;
      info.setLayoutCount         = 1;
      info.pSetLayouts            = &setLayout;
      info.pushConstantRangeCount = 1;
      info.pPushConstantRanges    = &pcr;

      return device.createPipelineLayoutUnique(info);
    }

    auto create_pipeline_cache(const vk::Device& device)
    {
      vk::PipelineCacheCreateInfo info;
      return device.createPipelineCacheUnique(info);
    }

    auto create_shader_module(
      const std::vector<uint32_t>& code,
      const vk::Device& device)
    {
      vk::ShaderModuleCreateInfo info;
      info.codeSize = code.size() * sizeof(uint32_t);
      info.pCode    = code.data();

      return device.createShaderModuleUnique(info);
    }

    auto create_pipeline(
      const vk::Extent2D& swapchainExtent,
      const vk::RenderPass& renderPass,
      const vk::PipelineCache& pipelineCache,
      const vk::PipelineLayout& pipelineLayout,
      const vk::Device& device)
    {
      /* shader stages */

      // vertex shader
      vk::UniqueShaderModule vertShaderModule =
        create_shader_module(compileVertShader(vert_shader), device);

      vk::PipelineShaderStageCreateInfo vertShaderStageInfo;
      vertShaderStageInfo.stage  = vk::ShaderStageFlagBits::eVertex;
      vertShaderStageInfo.module = *vertShaderModule;
      vertShaderStageInfo.pName  = "main";

      // fragment shader
      vk::UniqueShaderModule fragShaderModule =
        create_shader_module(compileFragShader(frag_shader), device);

      vk::PipelineShaderStageCreateInfo fragShaderStageInfo;
      fragShaderStageInfo.stage  = vk::ShaderStageFlagBits::eFragment;
      fragShaderStageInfo.module = *fragShaderModule;
      fragShaderStageInfo.pName  = "main";

      // stages
      std::array shaderStages = {vertShaderStageInfo, fragShaderStageInfo};

      /* vertex input */

      std::array<vk::VertexInputBindingDescription, 1> vertBindingDesc;
      vertBindingDesc[0].stride    = sizeof(draw_vtx);
      vertBindingDesc[0].inputRate = vk::VertexInputRate::eVertex;

      std::array<vk::VertexInputAttributeDescription, 3> vertAttrDesc;
      // pos
      vertAttrDesc[0].location = 0;
      vertAttrDesc[0].binding  = vertBindingDesc[0].binding;
      vertAttrDesc[0].format   = vk::Format::eR32G32Sfloat;
      vertAttrDesc[0].offset   = offsetof(draw_vtx, pos);
      // pos
      vertAttrDesc[1].location = 1;
      vertAttrDesc[1].binding  = vertBindingDesc[0].binding;
      vertAttrDesc[1].format   = vk::Format::eR32G32Sfloat;
      vertAttrDesc[1].offset   = offsetof(draw_vtx, uv);
      // col
      vertAttrDesc[2].location = 2;
      vertAttrDesc[2].binding  = vertBindingDesc[0].binding;
      vertAttrDesc[2].format   = vk::Format::eR8G8B8A8Unorm;
      vertAttrDesc[2].offset   = offsetof(draw_vtx, col);

      vk::PipelineVertexInputStateCreateInfo vertInputStateInfo;
      vertInputStateInfo.vertexBindingDescriptionCount = vertBindingDesc.size();
      vertInputStateInfo.pVertexBindingDescriptions    = vertBindingDesc.data();
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
        vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | //
        vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;  //

      colAttachments[0].blendEnable = VK_TRUE;

      // clang-format off
      // alpha blend
      colAttachments[0].srcColorBlendFactor = vk::BlendFactor::eSrcAlpha;
      colAttachments[0].dstColorBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha;
      colAttachments[0].colorBlendOp        = vk::BlendOp::eAdd;
      colAttachments[0].srcAlphaBlendFactor = vk::BlendFactor::eSrcAlpha;
      colAttachments[0].dstAlphaBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha;
      colAttachments[0].alphaBlendOp        = vk::BlendOp::eAdd;
      // clang-format on

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

    void init_pipeline(
      vk::CommandBuffer& cmd,
      const render_buffer& vtx_buff,
      const render_buffer& idx_buff,
      const vk::Viewport& viewport,
      const vk::Pipeline& pipeline)
    {
      // init pipeline
      static_assert(sizeof(draw_idx) == sizeof(uint16_t));
      cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline);
      cmd.bindVertexBuffers(0, vtx_buff.buffer.get(), {0});
      cmd.bindIndexBuffer(idx_buff.buffer.get(), 0, vk::IndexType::eUint16);
      cmd.setViewport(0, viewport);
    }

    void send_draw_data(
      render_buffer& vtx_buff,
      render_buffer& idx_buff,
      const draw_lists& draw_data,
      const vk::Device& device)
    {
      auto* mapped_vtx_buff = map_render_buffer(vtx_buff, device);
      auto* mapped_idx_buff = map_render_buffer(idx_buff, device);

      size_t vtx_offset = 0;
      size_t idx_offset = 0;
      for (auto&& dl : draw_data.lists) {

        auto vtx_size = dl.vtx_buffer.size() * sizeof(draw_vtx);
        auto idx_size = dl.idx_buffer.size() * sizeof(draw_idx);

        std::memcpy(
          mapped_vtx_buff + vtx_offset, dl.vtx_buffer.data(), vtx_size);
        std::memcpy(
          mapped_idx_buff + idx_offset, dl.idx_buffer.data(), idx_size);

        vtx_offset += vtx_size;
        idx_offset += idx_size;
      }

      unmap_render_buffer(vtx_buff, device);
      unmap_render_buffer(idx_buff, device);
    }

    void prepare_draw_data(
      render_buffer& vtx_buff,
      render_buffer& idx_buff,
      const draw_lists& draw_data,
      const vk::Device& device,
      const vk::PhysicalDevice& physicalDevice)
    {
      auto vtx_buff_size = draw_data.total_vtx_count() * sizeof(draw_vtx);
      auto idx_buff_size = draw_data.total_idx_count() * sizeof(draw_idx);

      // reserve render buffer
      resize_render_buffer(idx_buff, idx_buff_size, device, physicalDevice);
      resize_render_buffer(vtx_buff, vtx_buff_size, device, physicalDevice);

      // write to render buffer
      send_draw_data(vtx_buff, idx_buff, draw_data, device);
    }

    void render_draw_data(
      vk::CommandBuffer& cmd,
      const draw_lists& draw_data,
      const vk::Viewport& viewport,
      const vk::DescriptorSet& defaultDsc,
      const vk::PipelineLayout& pipelineLayout)
    {
      // set push constant
      {
        // transform (0,0):(w, h) -> (-1,-1):(1,1)
        draw_pc pc;
        pc.scale     = glm::vec2(2.f / viewport.width, 2.f / viewport.height);
        pc.translate = glm::vec2(-1, -1);

        cmd.pushConstants(
          pipelineLayout,
          vk::ShaderStageFlagBits::eVertex,
          0,
          sizeof(draw_pc),
          &pc);
      }

      // indexed render
      for (auto&& dl : draw_data.lists) {

        uint32_t vtxOffset = 0;
        uint32_t idxOffset = 0;

        for (auto&& dc : dl.cmd_buffer) {

          // texture
          {
            auto dsc = from_draw_tex(dc.tex);
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

    auto create_image_sampler(const vk::Device& device)
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

    auto create_descriptor_set_layout(
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

    auto create_descriptor_pool(const vk::Device& device)
    {
      std::array poolSizes = {
        vk::DescriptorPoolSize {vk::DescriptorType::eCombinedImageSampler, 1000}};

      vk::DescriptorPoolCreateInfo info;
      info.flags         = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet;
      info.maxSets       = 1000 * poolSizes.size();
      info.poolSizeCount = poolSizes.size();
      info.pPoolSizes    = poolSizes.data();

      return device.createDescriptorPoolUnique(info);
    }
  } // namespace

  window_drawer::window_drawer(
    render_context& ctx,
    const view_context& view_ctx,
    const wm::window* win)
    : m_render_ctx {ctx}
    , m_view_ctx {view_ctx}
    , m_window {win}
    , m_window_pos {view_ctx.window_manager().screen_pos(win)}
  {
    m_render_ctx.push_clip_rect(m_window_pos, m_window_pos + win->size());
  }

  window_drawer::~window_drawer() noexcept
  {
    m_render_ctx.add_draw_list(std::move(m_draw_list));
    m_render_ctx.pop_clip_rect();
  }

  void add_rect(const fvec2& p1, const fvec2& p2, const fvec4& col)
  {
    assert(!"TODO");
  }

  void add_rect_filled(const fvec2& p1, const fvec2& p2, const fvec4& col)
  {
    assert(!"TODO");
  }

  void window_drawer::add_rect_filled(
    const fvec2& p1,
    const fvec2& p2,
    const fvec4& col)
  {
  }

  class render_context::impl
  {
  public:
    impl(
      editor::render_context& render_ctx,
      vulkan::vulkan_context& vk_ctx,
      glfw::glfw_window& glfw_win)
      : render_ctx {render_ctx}
      , vulkan_ctx {vk_ctx}
      , window_ctx {vulkan_ctx, glfw_win}
      , view_ctx {nullptr}
    {
      auto physicalDevice = vulkan_ctx.physical_device();
      auto device         = window_ctx.device();
      auto graphicsQueue  = window_ctx.graphics_queue();
      auto commandPool    = window_ctx.command_pool();
      auto renderPass     = window_ctx.render_pass();

      // clang-format off
      image_sampler         = create_image_sampler(device);
      descriptor_set_layout = create_descriptor_set_layout(image_sampler.get(), device);
      descriptor_pool       = create_descriptor_pool(device);
      pipeline_cache        = create_pipeline_cache(device);
      pipeline_layout       = create_pipeline_layout(descriptor_set_layout.get(), device);
      // clang-format on

      pipeline = create_pipeline(
        window_ctx.swapchain_extent(),
        window_ctx.render_pass(),
        pipeline_cache.get(),
        pipeline_layout.get(),
        device);

      default_texture = create_texture_data(
        1,
        1,
        vk::Format::eR8G8B8A8Unorm,
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
    ~impl() noexcept = default;

    // render
    editor::render_context& render_ctx;
    // vulkan
    vulkan::vulkan_context& vulkan_ctx;

    // window render pass
    vulkan::window_context window_ctx;

  public:
    vk::UniqueSampler image_sampler;
    vk::UniqueDescriptorSetLayout descriptor_set_layout;
    vk::UniqueDescriptorPool descriptor_pool;

  public:
    vk::UniquePipelineCache pipeline_cache;
    vk::UniquePipelineLayout pipeline_layout;
    vk::UniquePipeline pipeline;

  public:
    texture_data default_texture;
    staging_buffer texture_staging;

  public:
    render_buffer vtx_buff;
    render_buffer idx_buff;

  public:
    // (in frame) view context pointer
    editor::view_context* view_ctx;
    // (in frame) clip rect stack
    std::vector<draw_clip> clip_rect_stack;
    // (in frame) draw list
    draw_lists draw_data;

  private:
    bool in_frame()
    {
      return view_ctx;
    }

  public:
    void begin_frame(editor::view_context& vctx)
    {
      // init in frame data
      assert(!in_frame());
      view_ctx        = &vctx;
      clip_rect_stack = {};
      draw_data       = {};
    }

    void end_frame()
    {
      assert(in_frame());
      view_ctx = nullptr;
    }

    void render()
    {
      assert(!in_frame());
      window_ctx.begin_frame();
      {
        auto cmd_buff = window_ctx.begin_record();
        {
          prepare_draw_data(
            vtx_buff,
            idx_buff,
            draw_data,
            window_ctx.device(),
            vulkan_ctx.physical_device());

          auto extent   = window_ctx.swapchain_extent();
          auto viewport = vk::Viewport(0, 0, extent.width, extent.height, 0, 1);

          init_pipeline(cmd_buff, vtx_buff, idx_buff, viewport, pipeline.get());

          render_draw_data(
            cmd_buff,
            draw_data,
            viewport,
            default_texture.dsc_set.get(),
            pipeline_layout.get());
        }
        window_ctx.end_record(cmd_buff);
      }
      window_ctx.end_frame();
    }

  public:
    auto create_window_drawer(const wm::window* win)
    {
      assert(in_frame());
      return window_drawer(render_ctx, *view_ctx, win);
    }

  public:
    void add_draw_list(draw_list&& dl)
    {
      assert(in_frame());
      draw_data.lists.push_back(std::move(dl));
    }

  public:
    void push_clip_rect(const fvec2& p1, const fvec2& p2)
    {
      clip_rect_stack.push_back({p1, p2});
    }

    void pop_clip_rect()
    {
      clip_rect_stack.pop_back();
    }

    auto get_clip_rect()
    {
      assert(!clip_rect_stack.empty());
      auto min = clip_rect_stack[0];
      for (auto&& clip : clip_rect_stack) {
        min.p1 = {std::max(min.p1.x, clip.p1.x), std::max(min.p1.y, clip.p1.y)};
        min.p2 = {std::min(min.p2.x, clip.p2.x), std::min(min.p2.y, clip.p2.y)};
      }
      return min;
    }
  };

  render_context::render_context(
    vulkan::vulkan_context& vk_ctx,
    glfw::glfw_window& glfw_win)
    : m_pimpl {std::make_unique<impl>(*this, vk_ctx, glfw_win)}
  {
  }

  render_context::~render_context() noexcept = default;

  void render_context::begin_frame(editor::view_context& view_ctx)
  {
    m_pimpl->begin_frame(view_ctx);
  }

  void render_context::end_frame()
  {
    m_pimpl->end_frame();
  }

  void render_context::render()
  {
    m_pimpl->render();
  }

  auto render_context::create_window_drawer(const wm::window* win)
    -> window_drawer
  {
    return m_pimpl->create_window_drawer(win);
  }

  void render_context::push_clip_rect(const glm::vec2& p1, const glm::vec2& p2)
  {
    m_pimpl->push_clip_rect(p1, p2);
  }

  void render_context::pop_clip_rect()
  {
    m_pimpl->pop_clip_rect();
  }

  auto render_context::get_clip_rect() -> draw_clip
  {
    return m_pimpl->get_clip_rect();
  }

  void render_context::add_draw_list(draw_list&& dl)
  {
    return m_pimpl->add_draw_list(std::move(dl));
  }

  auto render_context::vulkan_context() const -> const vulkan::vulkan_context&
  {
    return m_pimpl->vulkan_ctx;
  }

  auto render_context::vulkan_context() -> vulkan::vulkan_context&
  {
    return m_pimpl->vulkan_ctx;
  }

  auto render_context::vulkan_window() const -> const vulkan::window_context&
  {
    return m_pimpl->window_ctx;
  }

  auto render_context::vulkan_window() -> vulkan::window_context&
  {
    return m_pimpl->window_ctx;
  }

} // namespace yave::editor