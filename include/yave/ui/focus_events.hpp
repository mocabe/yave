//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/ui/event.hpp>
#include <yave/support/enum_flag.hpp>

namespace yave::ui {

  /// Focus transfer reason
  enum class focus_reason
  {
    other,
    mouse,
  };

  namespace events {

    class focus_event : public event
    {
      focus_reason m_reason;

    protected:
      focus_event(const window& target, event_phase phase, focus_reason reason);

      /// get forus reason
      auto reason() const -> focus_reason;
    };

    /// Event which bubbles from window which is getting focus.
    class focusing final : public focus_event
    {
    public:
      focusing(const window& target, event_phase phase, focus_reason reason);
    };

    /// Event which bubbles from window which is losing focus.
    class blurring final : public focus_event
    {
    public:
      blurring(const window& target, event_phase phase, focus_reason reason);
    };

    /// Event sent to window which got focus.
    /// \note This event does not bubble.
    class focus final : public focus_event
    {
    public:
      focus(const window& target, focus_reason reason);
    };

    /// Event sent to window which lost focus.
    /// \note This event does not bubble.
    class blur final : public focus_event
    {
    public:
      blur(const window& target, focus_reason reason);
    };

  } // namespace events

} // namespace yave::ui