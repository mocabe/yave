//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/ui/event.hpp>
#include <yave/ui/controller.hpp>
#include <yave/ui/vec.hpp>
#include <yave/ui/size.hpp>
#include <yave/ui/layout_scope.hpp>
#include <yave/ui/render_scope.hpp>
#include <yave/ui/signal.hpp>

#include <range/v3/view/subrange.hpp>
#include <range/v3/view/transform.hpp>

#include <vector>
#include <memory>
#include <typeinfo>
#include <span>

namespace yave::ui {

  class view_context;
  class data_context;
  class window_manager;
  class layout_context;
  class render_context;
  struct window_layout_data;
  struct window_render_data;

  /// window ID
  struct wid
  {
    u64 data = 0;

    auto operator<=>(const wid&) const = default;

    /// random gen
    static auto random() -> wid;
  };

  /// Base class of GUI window and widgets
  class window : public trackable
  {
    /// window manager ref
    ui::window_manager* m_wm = nullptr;
    /// unique id for this window
    ui::wid m_id = ui::wid::random();
    /// non-owning pointer to parent
    ui::window* m_parent = nullptr;
    /// list of owning children pointers.
    std::list<ui::unique<ui::window>> m_children;
    /// list of owning event controllers.
    std::list<ui::unique<ui::controller>> m_controllers;
    /// per window layout data
    std::unique_ptr<ui::window_layout_data> m_ldata;
    /// per window render data
    std::unique_ptr<ui::window_render_data> m_rdata;
    /// registered?
    /// \note will be applied recursively
    bool m_registered : 1 = false;
    /// invalidated?
    /// \note will not be applied recursively
    bool m_invalidated : 1 = false;
    /// visible?
    /// \note will not be applied recursively
    bool m_visible : 1 = false;

  public:
    /// ctor
    window();
    /// dtor
    virtual ~window() noexcept;

  public:
    void set_parent(window*, passkey<ui::window_manager>);
    void set_registered(bool, ui::window_manager&, passkey<ui::window_manager>);
    void set_invalidated(bool, passkey<ui::window_manager>);
    void set_visible(bool, passkey<ui::window_manager>);

  public:
    auto window_layout_data(passkey<layout_context>) const
      -> ui::window_layout_data&;
    auto window_render_data(passkey<render_context>) const
      -> ui::window_render_data&;

  public:
    /// Registered to window manager?
    /// e.g. window is currently attached to root window tree?
    bool is_registered() const;

  public:
    /// Mark this window as invalidated.
    /// Invalidated windows will be queued into next layout stage.
    void invalidate();
    /// Invalidated?
    bool is_invalidated() const;

  public:
    /// Show this window
    void show();
    /// Hide this window
    void hide();
    /// Visible?
    bool is_visible() const;

  public:
    /// get window manager
    /// \requres is_registered()
    auto window_manager() -> ui::window_manager&;
    /// get window manager
    /// \requres is_registered()
    auto window_manager() const -> const ui::window_manager&;

  protected:
    /// utility function to add new window
    /// \param idx position of insertion. will be clamped to [0, size].
    /// \param win window to insert
    void add_child(size_t idx, ui::unique<window> win);
    /// utility function to detach child window
    auto detach_child(const window* w) -> ui::unique<window>;
    /// utility function to remove child window
    void remove_child(const window* w);
    /// utility function to move child window
    void move_child_front(const window* w);
    /// utility function to move child window
    void move_child_back(const window* w);

  protected:
    /// add event controller
    void add_controller(ui::unique<controller> l);
    /// detach event controller
    auto detach_controller(const controller* l) -> ui::unique<controller>;
    /// remove event controller
    void remove_controller(const controller* l);

  public:
    /// Process layout calculation.
    ///
    /// This function calculates and sets window's new size.
    /// Typical layout computation is done by first computing all sizes of child
    /// windows, then rearrange their offsets based on new sizes.
    ///
    /// \param ctx scoped context for this window
    virtual void layout(layout_scope ctx) const = 0;

    /// Process drawing.
    ///
    /// This function computes draw commands for the window.
    /// Created draw commands will be stored into layer(s) then composed
    /// by parent window later if needed.
    ///
    /// \param ctx scoped context for this window
    virtual void render(render_scope ctx) const = 0;

  private:
    template <class T>
    auto make_raw_pointer_view(const T& ps) const
    {
      using namespace ranges;
      return subrange(ps) | views::transform([](auto&& p) { return p.get(); });
    }

    template <class T>
    auto make_raw_pointer_view(T& ps)
    {
      using namespace ranges;
      return subrange(ps) | views::transform([](auto&& p) { return p.get(); });
    }

  public:
    /// Get range view of child windows
    auto children() const
    {
      return make_raw_pointer_view(m_children);
    }

    /// Get range view of child windows
    auto children()
    {
      return make_raw_pointer_view(m_children);
    }

    /// Get range view of controllers
    auto controllers() const
    {
      return make_raw_pointer_view(m_controllers);
    }

    /// Get range view of controllers
    auto controllers()
    {
      return make_raw_pointer_view(m_controllers);
    }

  public:
    /// Get ID of this window
    auto id() const -> wid
    {
      return m_id;
    }

    /// Get parent window
    auto parent() const -> const ui::window*
    {
      return m_parent;
    }

    /// Get parent window
    auto parent() -> ui::window*
    {
      return m_parent;
    }
  };

  template <class Derived>
  using generic_window = generic_trackable<Derived, window>;

} // namespace yave::ui