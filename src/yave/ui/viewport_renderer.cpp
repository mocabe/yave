//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <vulkan/vulkan.hpp>
#include <yave/ui/viewport_renderer.hpp>
#include <yave/ui/render_context.hpp>
#include <yave/ui/viewport.hpp>
#include <yave/ui/native_window.hpp>
#include <yave/ui/vulkan_shaders.hpp>
#include <yave/ui/draw_list.hpp>

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

} // namespace

namespace yave::ui {

  viewport_renderer::viewport_renderer(viewport& vp, render_context& rctx)
    : m_vp {vp}
    , m_rctx {rctx}
    , m_surface {rctx, *vp.get_native()}
  {
    auto& device = rctx.vulkan_device();

    m_pipeline = createPipeline(
      m_surface.render_pass(),
      rctx.pipeline_cache(),
      rctx.pipeline_layout(),
      device.device());
  }

  void viewport_renderer::render(draw_list&& dl)
  {
    // TODO: render dl
    (void)dl;

    static f32 r = 0.f;
    static f32 g = 0.f;
    static f32 b = 0.f;

    r = fmod(r + 0.002f, 1.f);
    g = fmod(g + 0.003f, 1.f);
    b = fmod(b + 0.007f, 1.f);

    m_surface.set_clear_color(r, g, b, 1.f);

    m_surface.begin_frame();
    {
      auto cmd = m_surface.begin_record();
      m_surface.end_record(cmd);
    }
    m_surface.end_frame();
  }

} // namespace yave::ui