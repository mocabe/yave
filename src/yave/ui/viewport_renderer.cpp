//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <optional>
#include <chrono>

#include <yave/ui/viewport_renderer.hpp>
#include <yave/ui/render_context.hpp>
#include <yave/ui/render_buffer.hpp>
#include <yave/ui/viewport.hpp>
#include <yave/ui/native_window.hpp>
#include <yave/ui/vulkan_shaders.hpp>
#include <yave/ui/draw_list.hpp>
#include <yave/ui/render_layer.hpp>
#include <yave/ui/vulkan_pipelines.hpp>
#include <yave/ui/window_manager.hpp>
#include <yave/ui/view_context.hpp>

namespace {

  using namespace yave::ui;

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
    const vk::CommandBuffer& cmd,
    const draw_lists& draw_data,
    const draw_tex& defaultTex,
    const vk::Viewport& viewport,
    const render_buffer& vtx_buff,
    const render_buffer& idx_buff,
    vulkan_pipeline_draw& pipeline_draw)
  {
    // bind drawing pipeline
    pipeline_draw.bind(cmd, vtx_buff, idx_buff, viewport);

    uint32_t idxOffset = 0;
    uint32_t vtxOffset = 0;

    // render commands
    for (auto&& dl : draw_data.lists()) {

      for (auto&& dc : dl.cmd_buffer)
        pipeline_draw.draw(dc, defaultTex, idxOffset, vtxOffset);

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
    m_surface.set_clear_color(0.2, 0.2, 0.2);
    m_pipeline_draw = std::make_unique<vulkan_pipeline_draw>(rctx, m_surface);
  }

  viewport_renderer::~viewport_renderer() noexcept
  {
    terminate_fence_thread();
    m_rctx.vulkan_device().device().waitIdle();
  }

  auto viewport_renderer::lock_surface() -> std::unique_lock<std::mutex>
  {
    return std::unique_lock(m_fence_mtx);
  }

  void viewport_renderer::init_fence_thread(view_context& vctx)
  {
    assert(!m_fence_thread.joinable());
    m_fence_thread = std::thread([&] {
      while (true) {
        auto lck = lock_surface();
        m_fence_cond.wait(lck, [&] { return m_fence_notify || m_fence_exit; });

        if (m_fence_exit)
          break;

        // wait next frame
        m_surface.wait_next_frame();
        // store can_render flag
        m_can_render.store(true, std::memory_order_release);
        // wake view context
        vctx.wake();
        // reset flag
        m_fence_notify = false;
      }
    });
  }

  void viewport_renderer::terminate_fence_thread()
  {
    {
      auto lck     = lock_surface();
      m_fence_exit = true;
      m_fence_cond.notify_one();
    }
    m_fence_thread.join();
  }

  bool viewport_renderer::can_render()
  {
    return m_can_render.load(std::memory_order_acquire);
  }

  void viewport_renderer::render(render_layer&& rl, view_context& vctx)
  {
    const auto& lists = rl.draw_lists({});

    auto lck = lock_surface();

    if (!m_fence_thread.joinable())
      init_fence_thread(vctx);

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

        auto&& extent   = m_surface.swapchain_extent();
        auto&& viewport = vk::Viewport(0, 0, extent.width, extent.height, 0, 1);

        auto&& default_tex = m_rctx.default_texture();
        auto&& pipeline    = *m_pipeline_draw;

        renderDrawData(
          cmd, lists, default_tex, viewport, vtx_buff, idx_buff, pipeline);

        m_surface.end_record();
      }
      m_surface.end_frame();
    }

    // wait next frame on fence thread
    {
      m_can_render.store(false, std::memory_order_release);
      m_fence_notify = true;
      m_fence_cond.notify_one();
    }
  }

} // namespace yave::ui