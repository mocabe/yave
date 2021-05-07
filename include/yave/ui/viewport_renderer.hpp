//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/ui/vulkan_surface.hpp>
#include <yave/ui/draw_list.hpp>

#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>

namespace yave::ui {

  class view_context;
  class viewport;
  class render_context;
  class render_buffer;
  class render_layer;
  class vulkan_pipeline_draw;

  /// Viewport renderer
  class viewport_renderer
  {
    viewport& m_vp;
    render_context& m_rctx;

    // surface resources
    vulkan_surface m_surface;

    // surface fence thread
    std::thread m_fence_thread;
    // surface fence mutex
    std::mutex m_fence_mtx;
    // surfece fence condition variable
    std::condition_variable m_fence_cond;
    // surface fence thread notify flag
    bool m_fence_notify = false;
    // surface fence thread exit flag
    bool m_fence_exit = false;

    // can render next frame?
    std::atomic<bool> m_can_render = true;

    // rendering pipelines
    std::unique_ptr<vulkan_pipeline_draw> m_pipeline_draw;

    // buffers (for each in-flight frame)
    std::vector<std::unique_ptr<render_buffer>> m_vtx_buffers;
    std::vector<std::unique_ptr<render_buffer>> m_idx_buffers;

    auto lock_surface() -> std::unique_lock<std::mutex>;
    void init_fence_thread(view_context& vctx);
    void terminate_fence_thread();

  public:
    viewport_renderer(viewport& vp, render_context& rctx);
    ~viewport_renderer() noexcept;

    auto& surface()
    {
      return m_surface;
    }

  public:
    /// can render next frame without blocking?
    bool can_render();
    // render draw commands
    void render(render_layer&& rl, view_context& vctx);
  };

} // namespace yave::ui