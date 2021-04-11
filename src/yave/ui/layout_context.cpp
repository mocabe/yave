//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/ui/layout_context.hpp>
#include <yave/ui/layout_scope.hpp>
#include <yave/ui/window.hpp>
#include <yave/ui/window_manager.hpp>
#include <yave/ui/root.hpp>

namespace yave::ui {

  class layout_context::impl
  {
    layout_context& self;

  public:
    impl(layout_context& self)
      : self {self}
    {
    }

    ~impl() noexcept = default;

  private:
    void do_layout_impl(const window* w)
    {
      // TODO: avoid always using parent when layout of children will not affect
      // updated layout. (how can we handle rendering stage then?)
      auto p = (w->parent() == w->window_manager().root()) ? w : w->parent();
      // run layout recursively
      p->layout(layout_scope(self, p, box_constraints()));
    }

    // TODO: Use more fancy algorithm for this
    void do_layout_search(const window* w)
    {
      if (w->is_invalidated()) {
        do_layout_impl(w);
        return;
      }

      for (auto&& c : w->children())
        do_layout_search(c);
    }

  public:
    auto get_size(const window* w) const
    {
      return w->window_layout_data({}).geom.size();
    }

    auto get_offset(const window* w) const
    {
      return w->window_layout_data({}).geom.offset();
    }

    void set_size(const window* w, ui::size s)
    {
      w->window_layout_data({}).geom.set_size(s);
    }

    void set_offset(const window* w, ui::vec o)
    {
      w->window_layout_data({}).geom.set_offset(o);
    }

    void do_layout(window_manager& wm)
    {
      auto root = static_cast<const window*>(wm.root());

      // init root
      if (root->is_invalidated()) {
        auto& data = root->window_layout_data({});
        auto inf   = std::numeric_limits<f32>::infinity();
        data.geom.set_offset({0, 0});
        data.geom.set_size({inf, inf});
      }

      // layout viewports
      for (auto&& vp : root->children())
        do_layout_search(vp);
    }

    auto layout_window(const window* w, const box_constraints& c)
    {
      w->layout(layout_scope(self, w, c));
      return get_size(w);
    }
  };

  layout_context::layout_context()
    : m_pimpl {std::make_unique<impl>(*this)}
  {
  }

  layout_context::~layout_context() noexcept = default;

  void layout_context::do_layout(window_manager& wm, passkey<view_context>)
  {
    m_pimpl->do_layout(wm);
  }

  auto layout_context::layout_window(
    const window* w,
    const box_constraints& c,
    passkey<layout_scope>) -> ui::size
  {
    return m_pimpl->layout_window(w, c);
  }

  void layout_context::init_viewport(viewport* /*vp*/, passkey<viewport>)
  {
    // TODO?
  }

  auto layout_context::get_size(const window* w) const -> ui::size
  {
    return m_pimpl->get_size(w);
  }

  auto layout_context::get_offset(const window* w) const -> ui::vec
  {
    return m_pimpl->get_offset(w);
  }

  void layout_context::set_size(
    const window* w,
    ui::size s,
    passkey<layout_scope>)
  {
    m_pimpl->set_size(w, s);
  }

  void layout_context::set_offset(
    const window* w,
    ui::vec o,
    passkey<layout_scope>)
  {
    m_pimpl->set_offset(w, o);
  }

} // namespace yave::ui