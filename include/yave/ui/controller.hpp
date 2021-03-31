//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/ui/event.hpp>
#include <yave/ui/passkey.hpp>
#include <yave/ui/signal.hpp>

#include <optional>

namespace yave::ui {

  class window;
  class data_context;
  class view_context;

  /// Base class of event controllers
  class controller : trackable
  {
    /// owner
    ui::window* m_window = nullptr;
    /// target phase
    event_phase m_phase;

  public:
    controller(event_phase phase);
    virtual ~controller() noexcept = default;

  public:
    /// handle event
    /// \param event Event object to process.
    /// \param vctx view context ref
    /// \param dctx data context ref
    /// \returns `true` to skip rest of event listeners registered to the
    /// window. This value will be ignored when event was accepted.
    virtual bool event(ui::event& e, view_context& vctx) = 0;

  public:
    // for window
    void set_window(ui::window*, passkey<ui::window>);

  protected:
    /// reset target phase
    void set_phase(event_phase p);

  public:
    /// get target phase
    auto phase() -> event_phase;
    /// get attached window
    auto window() -> ui::window*;
    /// get attached window
    auto window() const -> const ui::window*;
  };
} // namespace yave::ui