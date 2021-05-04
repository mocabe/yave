//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/ui/vulkan_surface.hpp>
#include <yave/ui/draw_list.hpp>

#include <memory>

namespace yave::ui {

  class viewport;
  class render_context;
  class render_buffer;
  class render_layer;
  class vulkan_pipeline_draw;

  // TODO: Implement this
  class viewport_renderer
  {
    viewport& m_vp;
    render_context& m_rctx;

    // surface resources
    vulkan_surface m_surface;

    // rendering pipelines
    std::unique_ptr<vulkan_pipeline_draw> m_pipeline_draw;

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

  public:
    // render draw commands
    void render(render_layer&& rl);
  };

} // namespace yave::ui