//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/wm/event.hpp>
#include <yave/wm/event_dispatcher.hpp>
#include <yave/wm/window_traverser.hpp>

// fwd
namespace yave::editor {
  class data_context_access;
  class view_context_access;
} // namespace yave::editor

namespace yave::wm {

  namespace events {

    /// resize event
    ///
    /// this event will be sent when size of window changed
    class resize final : public event
    {
      // new size
      glm::vec2 m_size;

      using event::accept;
      using event::accepted;

    public:
      resize(const glm::vec2& size)
        : event::event()
        , m_size {size}
      {
      }

      auto& size() const
      {
        return m_size;
      }
    };

    /// move event
    ///
    /// this event will be sent when position of window changed
    class move final : public event
    {
      // new relative pos
      glm::vec2 m_pos;

      using event::accept;
      using event::accepted;

    public:
      move(const glm::vec2& pos)
        : event::event()
        , m_pos {pos}
      {
      }

      auto& pos() const
      {
        return m_pos;
      }
    };

  } // namespace events

  struct resize_event_visitor : event_visitor
  {
    using event_visitor::event_visitor;
  };

  /// dispatcher for events::resize
  using resize_event_dispatcher =
    composed_event_dispatcher<resize_event_visitor, single_window_traverser>;

  struct move_event_visitor : event_visitor
  {
    using event_visitor::event_visitor;
  };

  /// dispatcher for events::move
  using move_event_dispatcher =
    composed_event_dispatcher<move_event_visitor, single_window_traverser>;

} // namespace yave::wm