//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/ui/vulkan_surface.hpp>
#include <yave/ui/draw_list.hpp>
#include <yave/ui/render_buffer.hpp>

#include <memory>

namespace yave::ui {

  class viewport;
  class render_context;

  // TODO: Implement this
  class viewport_renderer
  {
    viewport& m_vp;
    render_context& m_rctx;

    // surface resources
    vulkan_surface m_surface;
    // rendering pipeline
    vk::UniquePipeline m_pipeline;

    // buffers (for each in-flight frame)
    std::vector<std::unique_ptr<render_buffer>> m_vtx_buffers;
    std::vector<std::unique_ptr<render_buffer>> m_idx_buffers;

  public:
    viewport_renderer(viewport& vp, render_context& rctx);
    ~viewport_renderer() noexcept;

    auto& surface()
    {
      return m_surface;
    }

    auto pipeline() const
    {
      return m_pipeline.get();
    }

  public:
    // render draw commands
    void render(draw_list&& dl);
  };

} // namespace yave::ui