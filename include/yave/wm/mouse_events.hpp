//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/wm/event.hpp>
#include <yave/wm/event_dispatcher.hpp>
#include <yave/wm/window_traverser.hpp>

#include <GLFW/glfw3.h>
#include <yave/lib/vec/vec.hpp>

#include <algorithm>

namespace yave::wm {

  /// Mouse button enum
  enum class mouse_button : int
  {
    _1     = GLFW_MOUSE_BUTTON_1,
    _2     = GLFW_MOUSE_BUTTON_2,
    _3     = GLFW_MOUSE_BUTTON_3,
    _4     = GLFW_MOUSE_BUTTON_4,
    _5     = GLFW_MOUSE_BUTTON_5,
    _6     = GLFW_MOUSE_BUTTON_6,
    _7     = GLFW_MOUSE_BUTTON_7,
    _8     = GLFW_MOUSE_BUTTON_8,
    left   = GLFW_MOUSE_BUTTON_LEFT,
    right  = GLFW_MOUSE_BUTTON_RIGHT,
    middle = GLFW_MOUSE_BUTTON_MIDDLE,
  };

  /// Mouse button state
  enum class mouse_button_state
  {
    up   = 0,
    down = 1,
  };

  namespace events {

    class mouse_event : public event
    {
    public:
      mouse_event(mouse_button button, fvec2 pos)
        : m_button {button}
        , m_pos {pos}
      {
      }

      /// position of mouse in window coordinate
      auto& pos() const
      {
        return m_pos;
      }

      /// event source button
      auto& button() const
      {
        return m_button;
      }

    protected:
      mouse_button m_button;
      fvec2 m_pos;
    };

    class mouse_click final : public mouse_event
    {
    public:
      mouse_click(mouse_button button, fvec2 pos)
        : mouse_event {button, pos}
      {
      }
    };

    class mouse_double_click final : public mouse_event
    {
    public:
      mouse_double_click(mouse_button button, fvec2 pos)
        : mouse_event {button, pos}
      {
      }
    };

    class mouse_press final : public mouse_event
    {
    public:
      mouse_press(mouse_button button, fvec2 pos)
        : mouse_event {button, pos}
      {
      }
    };

    class mouse_release final : public mouse_event
    {
    public:
      mouse_release(mouse_button button, fvec2 pos)
        : mouse_event {button, pos}
      {
      }
    };

    class mouse_repeat final : public mouse_event
    {
    public:
      mouse_repeat(mouse_button button, fvec2 pos)
        : mouse_event {button, pos}
      {
      }
    };

    class mouse_hover final : public mouse_event
    {
    public:
      mouse_hover(fvec2 pos)
        : mouse_event {{}, pos}
      {
      }
    };

  } // namespace events

  /// mouse event dispatcher base
  struct mouse_event_visitor : event_visitor
  {
  protected:
    /// last window accepted mouse event
    window* m_accepted = nullptr;

  public:
    mouse_event_visitor(
      std::unique_ptr<events::mouse_event>&& e,
      const editor::data_context& dctx,
      const editor::view_context& vctx)
      : event_visitor(std::move(e), dctx, vctx)
    {
    }

    /// dispatch mouse event based on hit detection
    bool visit(window* w) override;

    /// get pointer to mouse event
    auto get_mouse_event() -> events::mouse_event*
    {
      return static_cast<events::mouse_event*>(m_event.get());
    }

    /// is the event accepted?
    bool accepted() const
    {
      return m_accepted;
    }

    /// get accepted window
    auto reciever() const -> window*
    {
      assert(accepted());
      return m_accepted;
    }
  };

  /// tagged for specific mouse event
  template <class MouseEvent>
  struct mouse_event_visitor_wrapper : mouse_event_visitor
  {
    mouse_event_visitor_wrapper(
      std::unique_ptr<MouseEvent>&& e,
      const editor::data_context& dctx,
      const editor::view_context& vctx)
      : mouse_event_visitor(std::move(e), dctx, vctx)
    {
    }
  };

  using mouse_press_dispatcher = composed_event_dispatcher<
    mouse_event_visitor_wrapper<events::mouse_press>,
    dfs_traverser_reverse_pre>;

  using mouse_release_dispatcher = composed_event_dispatcher<
    mouse_event_visitor_wrapper<events::mouse_release>,
    dfs_traverser_reverse_pre>;

  using mouse_repeat_dispatcher = composed_event_dispatcher<
    mouse_event_visitor_wrapper<events::mouse_repeat>,
    dfs_traverser_reverse_pre>;

  using mouse_hover_dispatcher = composed_event_dispatcher<
    mouse_event_visitor_wrapper<events::mouse_hover>,
    dfs_traverser_reverse_pre>;

  using mouse_click_dispatcher = composed_event_dispatcher<
    mouse_event_visitor_wrapper<events::mouse_click>,
    dfs_traverser_reverse_pre>;

  using mouse_double_click_dispatcher = composed_event_dispatcher<
    mouse_event_visitor_wrapper<events::mouse_double_click>,
    dfs_traverser_reverse_pre>;

} // namespace yave::wm