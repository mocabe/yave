//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/editor/render_context.hpp>
#include <yave/editor/view_context.hpp>

namespace yave::editor {

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
    }
    ~impl() noexcept = default;

    // render
    editor::render_context& render_ctx;
    // vulkan
    vulkan::vulkan_context& vulkan_ctx;

    // window render pass
    vulkan::window_context window_ctx;

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
          // TODO submit render commands
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