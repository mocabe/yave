//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/ui/typedefs.hpp>
#include <yave/ui/vulkan_context.hpp>

#include <span>

namespace yave::ui {

  struct draw_cmd;
  struct draw_tex;
  class render_buffer;
  class vulkan_surface;
  class render_context;

  /// Draw pipeline
  class vulkan_pipeline_draw
  {
    render_context& m_rctx;
    vulkan_surface& m_surface;

    // pipeline
    vk::UniquePipelineLayout m_pipeline_layout;
    vk::UniquePipeline m_pipeline;

    // pipeline bind data
    struct bind_data
    {
      vk::CommandBuffer command_buffer;
      vk::Viewport viewport;
      vk::Buffer vertex_buffer;
      vk::Buffer index_buffer;
    } m_bind_data;

    auto create_pipeline_layout(
      const vk::DescriptorSetLayout& layout,
      const vk::Device& device) const -> vk::UniquePipelineLayout;

    auto create_pipeline(
      const vk::PipelineLayout& layout,
      const vk::RenderPass& pass,
      const vk::PipelineCache& cache,
      const vk::Device& device) const -> vk::UniquePipeline;

  public:
    vulkan_pipeline_draw(render_context& rctx, vulkan_surface& surface);

    /// Bind pipeline
    void bind(
      const vk::CommandBuffer& cmd,
      const render_buffer& vtx_buff,
      const render_buffer& idx_buff,
      const vk::Viewport& viewport);

    /// Record draw command
    void draw(
      const draw_cmd& dc,
      const draw_tex& default_tex,
      const u32& idx_base_offset,
      const u32& vtx_base_offset);
  };

} // namespace yave::ui
