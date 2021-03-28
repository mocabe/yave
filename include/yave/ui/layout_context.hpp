//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/ui/passkey.hpp>
#include <yave/ui/size.hpp>
#include <yave/ui/vec.hpp>
#include <yave/ui/box_constraints.hpp>
#include <yave/ui/window_geometry.hpp>
#include <yave/ui/passkey.hpp>

#include <memory>

namespace yave::ui {

  class layout_scope;
  class window_manager;
  class view_context;
  class window;
  class viewport;

  /// Per window layout data
  struct window_layout_data
  {
    /// window box geometry
    window_geometry geom;
  };

  /// layout context
  class layout_context
  {
    class impl;
    std::unique_ptr<impl> m_pimpl;

  public:
    layout_context();
    ~layout_context() noexcept;

  public:
    /// Process entire layout stage
    void do_layout(window_manager& wm, passkey<view_context>);

    /// Process layout of each window
    auto layout_window(
      const window*,
      const box_constraints& c,
      passkey<layout_scope>) -> ui::size;

  public:
    /// Setup viewport
    void init_viewport(viewport* vp, passkey<viewport>);

  public:
    /// Get window's size property
    auto get_size(const window*) const -> ui::size;
    /// Set size of window
    void set_size(const window*, ui::size, passkey<layout_scope>);

    /// Get window's offset property
    auto get_offset(const window*) const -> ui::vec;
    /// Set offset of window
    void set_offset(const window*, ui::vec, passkey<layout_scope>);
  };

} // namespace yave::ui