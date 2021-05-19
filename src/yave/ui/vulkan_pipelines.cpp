//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/ui/vulkan_pipelines.hpp>
#include <yave/ui/vulkan_surface.hpp>
#include <yave/ui/vulkan_shaders.hpp>
#include <yave/ui/render_context.hpp>
#include <yave/ui/draw_list.hpp>
#include <yave/ui/render_buffer.hpp>

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
}

namespace yave::ui {

  auto vulkan_pipeline_draw::create_pipeline_layout(
    const vk::DescriptorSetLayout& layout,
    const vk::Device& device) const -> vk::UniquePipelineLayout
  {
    std::array pcr = {vk::PushConstantRange()
                        .setStageFlags(vk::ShaderStageFlagBits::eVertex)
                        .setSize(sizeof(yave::ui::draw_pc))};

    std::array layouts = {layout};

    auto info = vk::PipelineLayoutCreateInfo()
                  .setSetLayouts(layouts)
                  .setPushConstantRanges(pcr);

    return device.createPipelineLayoutUnique(info);
  }

  auto vulkan_pipeline_draw::create_pipeline(
    const vk::PipelineLayout& layout,
    const vk::RenderPass& pass,
    const vk::PipelineCache& cache,
    const vk::Device& device) const -> vk::UniquePipeline
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
                  .setRenderPass(pass)
                  .setLayout(layout);

    // NOTE: ignoring vk::Result::ePipelineCompileRequiredEXT
    return device.createGraphicsPipelineUnique(cache, info).value;
  }

  vulkan_pipeline_draw::vulkan_pipeline_draw(
    render_context& rctx,
    vulkan_surface& surface)
    : m_rctx {rctx}
    , m_surface {surface}
  {
    m_pipeline_layout = create_pipeline_layout(
      rctx.descriptor_set_layout(), rctx.vulkan_device().device());

    m_pipeline = create_pipeline(
      m_pipeline_layout.get(),
      surface.render_pass(),
      rctx.pipeline_cache(),
      rctx.vulkan_device().device());
  }

  void vulkan_pipeline_draw::bind(
    const vk::CommandBuffer& cmd,
    const render_buffer& vtx_buff,
    const render_buffer& idx_buff,
    const vk::Viewport& viewport) 
  {
    // set bind data
    m_bind_data.command_buffer = cmd;
    m_bind_data.vertex_buffer  = vtx_buff.buffer();
    m_bind_data.index_buffer   = idx_buff.buffer();
    m_bind_data.viewport       = viewport;

    // bind pipeline
    static_assert(sizeof(draw_idx) == sizeof(uint16_t));
    cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, m_pipeline.get());
    cmd.bindVertexBuffers(0, vtx_buff.buffer(), {0});
    cmd.bindIndexBuffer(idx_buff.buffer(), 0, vk::IndexType::eUint16);
    cmd.setViewport(0, viewport);

    // init push constants
    {
      // transform (0,0):(w, h) -> (-1,-1):(1,1)
      draw_pc pc;
      pc.scale     = glm::vec2(2.f / viewport.width, 2.f / viewport.height);
      pc.translate = glm::vec2(-1, -1);

      cmd.pushConstants(
        m_pipeline_layout.get(),
        vk::ShaderStageFlagBits::eVertex,
        0,
        sizeof(draw_pc),
        &pc);
    }
  }

  void vulkan_pipeline_draw::draw(
    const draw_cmd& dc,
    const draw_tex& default_tex,
    const u32& idx_base_offset,
    const u32& vtx_base_offset)
  {
    auto cmd    = m_bind_data.command_buffer;
    auto layout = m_pipeline_layout.get();

    {
      auto dsc = render_context::draw_tex_to_descriptor_set(
        dc.tex ? dc.tex : default_tex);

      // bind texture
      cmd.bindDescriptorSets(
        vk::PipelineBindPoint::eGraphics, layout, 0, dsc, {});
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
      idx_base_offset + dc.idx_offset,
      vtx_base_offset + dc.vtx_offset,
      0);
  }

} // namespace yave::ui