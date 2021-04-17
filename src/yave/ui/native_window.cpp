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
#include <yave/ui/viewport.hpp>

namespace yave::ui {

  class native_window::impl
  {
    ui::viewport* m_vp;
    glfw_window m_win;

  public:
    impl(view_context& vctx, std::u8string name, ui::size size)
      : m_win {vctx.main_ctx().glfw_ctx(), std::move(name), size}
    {
    }

    auto& glfw_win()
    {
      return m_win;
    }

    auto handle() const -> GLFWwindow*
    {
      return m_win.handle();
    }

    auto name() const -> const std::u8string&
    {
      return m_win.name();
    }

    auto size() const -> ui::size
    {
      return m_win.size();
    }

    auto pos() const -> ui::vec
    {
      return m_win.pos();
    }

    auto fb_size() const -> ui::size
    {
      return m_win.fb_size();
    }

    auto content_scale() const -> ui::vec
    {
      return m_win.content_scale();
    }

    auto fb_scale() const -> ui::vec
    {
      return fb_size().vec() / size().vec();
    }

    bool focused() const
    {
      return m_win.focused();
    }

    bool maximized() const
    {
      return m_win.maximized();
    }

    bool minimized() const
    {
      return m_win.minimized();
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
      m_win.update_pos(x, y, {});
    }

    void update_size(u32 w, u32 h)
    {
      m_win.update_size(w, h, {});
    }

    void update_fb_size(u32 w, u32 h)
    {
      m_win.update_fb_size(w, h, {});
    }

    void update_content_scale(f32 xs, f32 ys)
    {
      m_win.update_content_scale(xs, ys, {});
    }

    void update_focus(bool focused)
    {
      m_win.update_focus(focused, {});
    }

    void update_maximize(bool maximized)
    {
      m_win.update_maximize(maximized, {});
    }

    void update_minimize(bool minimized)
    {
      m_win.update_minimize(minimized, {});
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
      m_vp->show();
    }

    void hide()
    {
      m_vp->hide();
      m_win.hide();
    }
  };

  native_window::native_window(
    view_context& vctx,
    std::u8string name,
    ui::size size,
    passkey<ui::viewport>&&)
    : m_pimpl {std::make_unique<impl>(vctx, name, size)}
  {
  }

  native_window::~native_window() noexcept = default;

  void native_window::set_viewport(ui::viewport* vp, passkey<ui::viewport>)
  {
    return m_pimpl->set_viewport(vp);
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

  auto native_window::fb_size() const -> ui::size
  {
    return m_pimpl->fb_size();
  }

  auto native_window::content_scale() const -> ui::vec
  {
    return m_pimpl->content_scale();
  }

  auto native_window::fb_scale() const -> ui::vec
  {
    return m_pimpl->fb_scale();
  }

  bool native_window::focused() const
  {
    return m_pimpl->focused();
  }

  bool native_window::maximized() const
  {
    return m_pimpl->maximized();
  }

  bool native_window::minimized() const
  {
    return m_pimpl->minimized();
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

  void native_window::update_content_scale(
    f32 xs,
    f32 ys,
    passkey<window_event_dispatcher>)
  {
    m_pimpl->update_content_scale(xs, ys);
  }

  void native_window::update_focus(
    bool focused,
    passkey<window_event_dispatcher>)
  {
    m_pimpl->update_focus(focused);
  }

  void native_window::update_maximize(
    bool maximized,
    passkey<window_event_dispatcher>)
  {
    m_pimpl->update_maximize(maximized);
  }

  void native_window::update_minimize(
    bool minimized,
    passkey<window_event_dispatcher>)
  {
    m_pimpl->update_minimize(minimized);
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