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

    class render_thread
    {
    public:
      render_thread(render_context& rctx, native_window& nw);
      bool is_running() const;
      void init(view_context& vtx);
      void terminate() noexcept;
      bool can_render() const;
      void render(const render_layer& rl);

    private:
      // context
      render_context& m_rctx;
      // rendering thread
      std::thread m_thread;
      // render mutex
      std::mutex m_mtx;
      // render condition variable
      std::condition_variable m_cond;
      // render thread notify flag
      bool m_notify = false;
      // renedr thread exit flag
      bool m_exit = false;
      // can render next frame?
      std::atomic<bool> m_can_render = true;
      // surface to render
      vulkan_surface m_surface;
      // buffers (for each in-flight frame)
      std::vector<std::unique_ptr<render_buffer>> m_vtx_buffers;
      std::vector<std::unique_ptr<render_buffer>> m_idx_buffers;
      // rendering pipelines
      std::unique_ptr<vulkan_pipeline_draw> m_pipeline_draw;

    } m_render_thread;

  public:
    viewport_renderer(viewport& vp, render_context& rctx);
    ~viewport_renderer() noexcept;

  public:
    /// can render next frame without blocking?
    bool can_render();
    // render draw commands
    void render(render_layer&& rl, view_context& vctx);
  };

} // namespace yave::ui