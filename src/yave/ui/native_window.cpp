//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/ui/native_window.hpp>
#include <yave/ui/window_events.hpp>
#include <yave/ui/view_context.hpp>
#include <yave/ui/main_context.hpp>
#include <yave/ui/glfw_context.hpp>
#include <yave/ui/glfw_window.hpp>
#include <yave/ui/vulkan_surface.hpp>
#include <yave/ui/render_context.hpp>

#include <yave/support/log.hpp>

YAVE_DECL_LOCAL_LOGGER(ui::native_window);

namespace yave::ui {

  class native_window::impl
  {
    native_window& m_self;
    view_context& m_vctx;
    ui::viewport* m_vp;
    glfw_window m_win;
    vulkan_surface m_surface;

  public:
    impl(
      native_window& self,
      view_context& vctx,
      std::u8string name,
      ui::size size)
      : m_self {self}
      , m_vctx {vctx}
      , m_win {vctx.main_ctx().glfw_ctx(), std::move(name), size}
      , m_surface {vctx.render_ctx().vulkan_device(), m_win}
    {
    }

    auto& surface()
    {
      return m_surface;
    }

    auto handle() const
    {
      return m_win.handle();
    }

    auto& name() const
    {
      return m_win.name();
    }

    auto& size() const
    {
      return m_win.size();
    }

    auto pos() const
    {
      return m_win.pos();
    }

    void set_name(std::u8string name)
    {
      m_win.set_name(std::move(name));
    }

    void set_size(ui::size size)
    {
      m_win.set_size(size);
    }

    void set_pos(ui::vec pos)
    {
      m_win.set_pos(pos);
    }

    void update_pos(u32 x, u32 y)
    {
      m_win.update_pos(x, y);
    }

    void update_size(u32 w, u32 h)
    {
      m_win.update_size(w, h);
    }

    void update_fb_size(u32 w, u32 h)
    {
      m_win.update_fb_size(w, h);
    }

    auto get_viewport() -> ui::viewport*
    {
      return m_vp;
    }

    void set_viewport(ui::viewport* vp)
    {
      m_vp = vp;
    }

    void show()
    {
      m_win.show();
    }

    void hide()
    {
      m_win.hide();
    }
  };

  native_window::native_window(
    view_context& vctx,
    std::u8string name,
    ui::size size,
    passkey<ui::viewport>)
    : m_pimpl {std::make_unique<impl>(*this, vctx, name, size)}
  {
  }

  native_window::~native_window() noexcept = default;

  void native_window::set_viewport(ui::viewport* vp, passkey<ui::viewport>)
  {
    return m_pimpl->set_viewport(vp);
  }

  auto native_window::surface() -> vulkan_surface&
  {
    return m_pimpl->surface();
  }

  auto native_window::handle() const -> GLFWwindow*
  {
    return m_pimpl->handle();
  }

  auto native_window::name() const -> const std::u8string&
  {
    return m_pimpl->name();
  }

  auto native_window::size() const -> ui::size
  {
    return m_pimpl->size();
  }

  auto native_window::pos() const -> ui::vec
  {
    return m_pimpl->pos();
  }

  void native_window::set_name(std::u8string name)
  {
    return m_pimpl->set_name(std::move(name));
  }

  void native_window::set_size(ui::size size)
  {
    return m_pimpl->set_size(size);
  }

  void native_window::set_pos(ui::vec pos)
  {
    return m_pimpl->set_pos(pos);
  }

  void native_window::update_pos(u32 x, u32 y, passkey<window_event_dispatcher>)
  {
    m_pimpl->update_pos(x, y);
  }

  void native_window::update_size(
    u32 w,
    u32 h,
    passkey<window_event_dispatcher>)
  {
    m_pimpl->update_size(w, h);
  }

  void native_window::update_fb_size(
    u32 w,
    u32 h,
    passkey<window_event_dispatcher>)
  {
    m_pimpl->update_fb_size(w, h);
  }

  auto native_window::viewport() -> ui::viewport*
  {
    return m_pimpl->get_viewport();
  }

  auto native_window::viewport() const -> const ui::viewport*
  {
    return m_pimpl->get_viewport();
  }

  void native_window::show()
  {
    m_pimpl->show();
  }

  void native_window::hide()
  {
    m_pimpl->hide();
  }

} // namespace yave::ui