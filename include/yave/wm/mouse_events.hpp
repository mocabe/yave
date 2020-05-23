//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/wm/event.hpp>
#include <yave/wm/event_dispatcher.hpp>

#include <GLFW/glfw3.h>
#include <yave/lib/vec/vec.hpp>

#include <algorithm>

namespace yave::wm {

  /// Mouse button enum
  enum class mouse_button
  {
    left   = GLFW_MOUSE_BUTTON_LEFT,
    right  = GLFW_MOUSE_BUTTON_RIGHT,
    middle = GLFW_MOUSE_BUTTON_MIDDLE,
  };

  namespace events {

    class mouse_event : public event
    {
    public:
      mouse_event(std::vector<mouse_button> buttons, fvec2 pos)
        : m_buttons {std::move(buttons)}
        , m_pos {pos}
      {
      }

      bool contains(mouse_button b)
      {
        return std::find(m_buttons.begin(), m_buttons.end(), b)
               != m_buttons.end();
      }

      auto& pos() const
      {
        return m_pos;
      }

      auto& buttons() const
      {
        return m_buttons;
      }

    protected:
      std::vector<mouse_button> m_buttons;
      fvec2 m_pos;
    };

    class mouse_click final : public mouse_event
    {
    public:
      mouse_click(std::vector<mouse_button> button, fvec2 pos)
        : mouse_event {std::move(button), pos}
      {
      }
    };

    class mouse_double_click final : public mouse_event
    {
    public:
      mouse_double_click(std::vector<mouse_button> button, fvec2 pos)
        : mouse_event {std::move(button), pos}
      {
      }
    };

    class mouse_press final : public mouse_event
    {
    public:
      mouse_press(std::vector<mouse_button> button, fvec2 pos)
        : mouse_event {std::move(button), pos}
      {
      }
    };

    class mouse_release final : public mouse_event
    {
    public:
      mouse_release(std::vector<mouse_button> button, fvec2 pos)
        : mouse_event {std::move(button), pos}
      {
      }
    };

    class mouse_hover final : public mouse_event
    {
    public:
      mouse_hover(std::vector<mouse_button> button, fvec2 pos)
        : mouse_event {std::move(button), pos}
      {
      }
    };

  } // namespace events

  /// mouse event dispatcher
  struct mouse_event_dispatcher : event_dispatcher
  {
    mouse_event_dispatcher(
      std::unique_ptr<events::mouse_event>&& e,
      editor::data_context& dctx,
      editor::view_context& vctx)
      : event_dispatcher(std::move(e), dctx, vctx)
    {
    }

    bool visit(window* w) override
    {
      auto e = get_mouse_event();

      // bbox
      auto p1 = w->screen_pos();
      auto p2 = p1 + w->size();
      auto p  = e->pos();

      if (p1.x <= p.x && p.x <= p2.x)
        if (p1.y <= p.y && p.y <= p2.y)
          return event_dispatcher::visit(w);

      return false;
    }

    /// get pointer to mouse event
    auto get_mouse_event() -> events::mouse_event*
    {
      return static_cast<events::mouse_event*>(m_event.get());
    }
  };

} // namespace yave::wm