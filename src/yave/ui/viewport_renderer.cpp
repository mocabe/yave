//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/ui/viewport_renderer.hpp>
#include <yave/ui/render_context.hpp>
#include <yave/ui/render_buffer.hpp>
#include <yave/ui/viewport.hpp>
#include <yave/ui/native_window.hpp>
#include <yave/ui/vulkan_shaders.hpp>
#include <yave/ui/draw_list.hpp>
#include <yave/ui/render_layer.hpp>

namespace {

  using namespace yave::ui;

  auto createShaderModule(
    const std::span<const u32>& code,
    const vk::Device& device)
  {
    auto info = vk::ShaderModuleCreateInfo()
                  .setCodeSize(code.size() * sizeof(u32))
                  .setPCode(code.data());

    return device.createShaderModuleUnique(info);
  }

  auto createPipeline(
    const vk::RenderPass& renderPass,
    const vk::PipelineCache& pipelineCache,
    const vk::PipelineLayout& pipelineLayout,
    const vk::Device& device)
  {
    /* shader stages */

    // vertex shader
    auto vertShaderModule = createShaderModule(shader_vert_spv, device);

    auto vertShaderStageInfo //
      = vk::PipelineShaderStageCreateInfo()
          .setStage(vk::ShaderStageFlagBits::eVertex)
          .setModule(*vertShaderModule)
          .setPName("main");

    // fragment shader
    auto fragShaderModule = createShaderModule(shader_frag_spv, device);

    auto fragShaderStageInfo //
      = vk::PipelineShaderStageCreateInfo()
          .setStage(vk::ShaderStageFlagBits::eFragment)
          .setModule(*fragShaderModule)
          .setPName("main");

    // stages
    std::array shaderStages = {vertShaderStageInfo, fragShaderStageInfo};

    /* vertex input */

    std::array vertBindingDesc = {
      vk::VertexInputBindingDescription()
        .setStride(sizeof(draw_vtx))
        .setInputRate(vk::VertexInputRate::eVertex)};

    std::array vertAttrDesc = {
      vk::VertexInputAttributeDescription()
        .setLocation(0)
        .setBinding(vertBindingDesc[0].binding)
        .setFormat(vk::Format::eR32G32Sfloat)
        .setOffset(offsetof(draw_vtx, pos)),
      vk::VertexInputAttributeDescription()
        .setLocation(1)
        .setBinding(vertBindingDesc[0].binding)
        .setFormat(vk::Format::eR32G32Sfloat)
        .setOffset(offsetof(draw_vtx, uv)),
      vk::VertexInputAttributeDescription()
        .setLocation(2)
        .setBinding(vertBindingDesc[0].binding)
        .setFormat(vk::Format::eR8G8B8A8Unorm)
        .setOffset(offsetof(draw_vtx, col))};

    auto vertInputStateInfo = vk::PipelineVertexInputStateCreateInfo()
                                .setVertexBindingDescriptions(vertBindingDesc)
                                .setVertexAttributeDescriptions(vertAttrDesc);

    /* input assembler */

    auto inputAsmStateInfo =
      vk::PipelineInputAssemblyStateCreateInfo() //
        .setTopology(vk::PrimitiveTopology::eTriangleList);

    /* viewport */

    std::array viewports = {vk::Viewport(0.f, 0.f, 1.f, 1.f, 0.f, 1.f)};
    std::array scissors  = {vk::Rect2D({0, 0}, {1, 1})};

    static_assert(viewports.size() == scissors.size());

    auto viewportStateInfo = vk::PipelineViewportStateCreateInfo() //
                               .setViewports(viewports)
                               .setScissors(scissors);

    /* rasterization */

    auto rasterStateInfo = vk::PipelineRasterizationStateCreateInfo()
                             .setPolygonMode(vk::PolygonMode::eFill)
                             .setFrontFace(vk::FrontFace::eCounterClockwise)
                             .setLineWidth(1.f);

    /* sample */

    auto multisampleStateInfo =
      vk::PipelineMultisampleStateCreateInfo()
        .setRasterizationSamples(vk::SampleCountFlagBits::e1)
        .setMinSampleShading(1.f);

    /* color blend */

    std::array colAttachments = {
      vk::PipelineColorBlendAttachmentState()
        .setColorWriteMask(
          vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | //
          vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA)
        .setBlendEnable(VK_TRUE)
        .setSrcColorBlendFactor(vk::BlendFactor::eSrcAlpha)
        .setDstColorBlendFactor(vk::BlendFactor::eOneMinusSrcAlpha)
        .setColorBlendOp(vk::BlendOp::eAdd)
        .setSrcAlphaBlendFactor(vk::BlendFactor::eSrcAlpha)
        .setDstAlphaBlendFactor(vk::BlendFactor::eOneMinusSrcAlpha)
        .setAlphaBlendOp(vk::BlendOp::eAdd)};

    auto colorBlendStateInfo =
      vk::PipelineColorBlendStateCreateInfo().setAttachments(colAttachments);

    auto depthStencilStateInfo = vk::PipelineDepthStencilStateCreateInfo()
                                   .setDepthCompareOp(vk::CompareOp::eLess)
                                   .setMinDepthBounds(0.f)
                                   .setMaxDepthBounds(1.f);

    /* dynamic state */

    std::array dynamicStates = {
      vk::DynamicState::eViewport, vk::DynamicState::eScissor};

    auto dynamicStateInfo =
      vk::PipelineDynamicStateCreateInfo().setDynamicStates(dynamicStates);

    /* pipeline */

    auto info = vk::GraphicsPipelineCreateInfo()
                  .setStages(shaderStages)
                  .setPVertexInputState(&vertInputStateInfo)
                  .setPInputAssemblyState(&inputAsmStateInfo)
                  .setPViewportState(&viewportStateInfo)
                  .setPRasterizationState(&rasterStateInfo)
                  .setPMultisampleState(&multisampleStateInfo)
                  .setPColorBlendState(&colorBlendStateInfo)
                  .setPDepthStencilState(&depthStencilStateInfo)
                  .setPDynamicState(&dynamicStateInfo)
                  .setRenderPass(renderPass)
                  .setLayout(pipelineLayout);

    // NOTE: ignoring vk::Result::ePipelineCompileRequiredEXT
    return device.createGraphicsPipelineUnique(pipelineCache, info).value;
  }

  void initPipeline(
    vk::CommandBuffer& cmd,
    const render_buffer& vtx_buff,
    const render_buffer& idx_buff,
    const vk::Viewport& viewport,
    const vk::Pipeline& pipeline)
  {
    // init pipeline
    static_assert(sizeof(draw_idx) == sizeof(uint16_t));
    cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline);
    cmd.bindVertexBuffers(0, vtx_buff.buffer(), {0});
    cmd.bindIndexBuffer(idx_buff.buffer(), 0, vk::IndexType::eUint16);
    cmd.setViewport(0, viewport);
  }

  void resizeRenderBuffers(
    std::vector<std::unique_ptr<render_buffer>>& buffers,
    size_t size,
    vk::BufferUsageFlags usage,
    vulkan_allocator& alloc)
  {
    auto old_size = buffers.size();
    buffers.resize(size);

    for (auto i = old_size; i < size; ++i) {
      buffers[i] = std::make_unique<render_buffer>(usage, alloc);
    }
  }

  void writeRenderBuffer(
    render_buffer& vtx_buff,
    render_buffer& idx_buff,
    const draw_lists& draw_data)
  {
    auto* mapped_vtx_buff = vtx_buff.begin_write();
    auto* mapped_idx_buff = idx_buff.begin_write();

    size_t vtx_offset = 0;
    size_t idx_offset = 0;

    for (auto&& dl : draw_data.lists()) {

      auto vtx_size = dl.vtx_buffer.size() * sizeof(draw_vtx);
      auto idx_size = dl.idx_buffer.size() * sizeof(draw_idx);

      std::memcpy(mapped_vtx_buff + vtx_offset, dl.vtx_buffer.data(), vtx_size);
      std::memcpy(mapped_idx_buff + idx_offset, dl.idx_buffer.data(), idx_size);

      vtx_offset += vtx_size;
      idx_offset += idx_size;
    }

    vtx_buff.end_write();
    idx_buff.end_write();
  }

  void prepareDrawData(
    render_buffer& vtx_buff,
    render_buffer& idx_buff,
    const draw_lists& draw_data)
  {
    auto vtx_buff_size = draw_data.total_vtx_count() * sizeof(draw_vtx);
    auto idx_buff_size = draw_data.total_idx_count() * sizeof(draw_idx);

    // resize render buffer
    idx_buff.resize(idx_buff_size);
    vtx_buff.resize(vtx_buff_size);

    // write to render buffer
    writeRenderBuffer(vtx_buff, idx_buff, draw_data);
  }

  void renderDrawData(
    vk::CommandBuffer& cmd,
    const draw_lists& draw_data,
    const draw_tex& defaultTex,
    const vk::Viewport& viewport,
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

    uint32_t vtxOffset = 0;
    uint32_t idxOffset = 0;

    // indexed render
    for (auto&& dl : draw_data.lists()) {

      for (auto&& dc : dl.cmd_buffer) {

        // bind texture
        {
          auto dsc = render_context::draw_tex_to_descriptor_set(
            dc.tex ? dc.tex : defaultTex);
          cmd.bindDescriptorSets(
            vk::PipelineBindPoint::eGraphics, pipelineLayout, 0, dsc, {});
        }

        {
          // scissor rect (frame buffer coord)
          vk::Rect2D scissor;
          scissor.offset.x      = dc.scissor.offset.x;
          scissor.offset.y      = dc.scissor.offset.y;
          scissor.extent.width  = dc.scissor.extent.x;
          scissor.extent.height = dc.scissor.extent.y;
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

namespace yave::ui {

  viewport_renderer::viewport_renderer(viewport& vp, render_context& rctx)
    : m_vp {vp}
    , m_rctx {rctx}
    , m_surface {rctx, vp.native_window()}
  {
    auto& device = rctx.vulkan_device();

    m_pipeline = createPipeline(
      m_surface.render_pass(),
      rctx.pipeline_cache(),
      rctx.pipeline_layout(),
      device.device());

    m_surface.set_clear_color(0.2, 0.2, 0.2, 1.f);
  }

  viewport_renderer::~viewport_renderer() noexcept
  {
    m_rctx.vulkan_device().device().waitIdle();
  }

  void viewport_renderer::render(render_layer&& rl)
  {
    const auto& lists = rl.draw_lists({});

    if (m_surface.begin_frame()) {

      if (auto cmd = m_surface.begin_record()) {

        resizeRenderBuffers(
          m_vtx_buffers,
          m_surface.frame_index_count(),
          vk::BufferUsageFlagBits::eVertexBuffer,
          m_rctx.vulkan_allocator());

        resizeRenderBuffers(
          m_idx_buffers,
          m_surface.frame_index_count(),
          vk::BufferUsageFlagBits::eIndexBuffer,
          m_rctx.vulkan_allocator());

        auto& vtx_buff = *m_vtx_buffers[m_surface.frame_index()];
        auto& idx_buff = *m_idx_buffers[m_surface.frame_index()];

        prepareDrawData(vtx_buff, idx_buff, lists);

        auto extent   = m_surface.swapchain_extent();
        auto viewport = vk::Viewport(0, 0, extent.width, extent.height, 0, 1);
        initPipeline(cmd, vtx_buff, idx_buff, viewport, m_pipeline.get());

        renderDrawData(
          cmd,
          lists,
          m_rctx.default_texture(),
          viewport,
          m_rctx.pipeline_layout());

        m_surface.end_record();
      }
      m_surface.end_frame();
    }
  }

} // namespace yave::ui