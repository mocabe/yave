//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/ui/render_context.hpp>
#include <yave/ui/render_scope.hpp>
#include <yave/ui/window_manager.hpp>
#include <yave/ui/viewport.hpp>
#include <yave/ui/root.hpp>
#include <yave/ui/viewport_renderer.hpp>
#include <yave/ui/main_context.hpp>
#include <yave/ui/native_window.hpp>
#include <yave/ui/vulkan_device.hpp>
#include <yave/ui/vulkan_surface.hpp>

namespace yave::ui {

  window_render_data::window_render_data()           = default;
  window_render_data::~window_render_data() noexcept = default;

  class render_context::impl
  {
    render_context& m_self;

    ui::vulkan_device m_device;

  public:
    impl(render_context& self, main_context& mctx)
      : m_self {self}
      , m_device {mctx.vulkan_ctx(), mctx.glfw_ctx()}
    {
    }

    auto& device()
    {
      return m_device;
    }

    auto render_window(const window* w, draw_list dl)
    {
      auto out_dl = std::optional<draw_list>();
      w->render(render_scope(m_self, std::move(dl), out_dl));

      assert(out_dl.has_value());
      return std::move(out_dl.value());
    }

    void do_render_viewport(viewport* vp)
    {
      auto dl = render_window(vp, draw_list());

      // TODO: implement pipeline

      auto& surface = vp->get_native()->surface();

      static f32 r = 0.f;
      static f32 g = 0.f;
      static f32 b = 0.f;

      r = fmod(r + 0.002f, 1.f);
      g = fmod(g + 0.003f, 1.f);
      b = fmod(b + 0.007f, 1.f);

      surface.set_clear_color(r, g, b, 1.f);

      surface.begin_frame();
      {
        auto cmd = surface.begin_record();
        surface.end_record(cmd);
      }
      surface.end_frame();
    }

    bool do_render_required(const window* w)
    {
      if (w->is_invalidated())
        return true;

      for (auto&& c : w->children()) {
        if (do_render_required(c))
          return true;
      }
      return false;
    }

    void do_render(window_manager& wm)
    {
      auto root = wm.root();

      // re-render invalidated viewport
      for (auto&& vp : root->viewports())
        if (do_render_required(vp))
          do_render_viewport(vp);
    }

    void init_viewport(viewport* vp)
    {
      auto& data = *vp->get_window_render_data({});
      // init renderer
      data.renderer = std::make_unique<viewport_renderer>(vp->get_native());
    }
  };

  render_context::render_context(main_context& mctx)
    : m_pimpl {std::make_unique<impl>(*this, mctx)}
  {
  }

  render_context::~render_context() noexcept = default;

  auto render_context::vulkan_device() -> ui::vulkan_device&
  {
    return m_pimpl->device();
  }

  void render_context::do_render(window_manager& wm, passkey<view_context>)
  {
    m_pimpl->do_render(wm);
  }

  auto render_context::render_window(const window* w, draw_list dl) -> draw_list
  {
    return m_pimpl->render_window(w, std::move(dl));
  }

  void render_context::init_viewport(viewport* vp, passkey<viewport>)
  {
    m_pimpl->init_viewport(vp);
  }

} // namespace yave::ui