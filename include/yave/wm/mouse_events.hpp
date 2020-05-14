//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/wm/event.hpp>

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

    class mouse_event_base : public event
    {
    protected:
      mouse_event_base(std::vector<mouse_button> buttons, fvec2 pos)
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

    class mouse_click final : public mouse_event_base
    {
      mouse_click(std::vector<mouse_button> button, fvec2 pos)
        : mouse_event_base {std::move(button), pos}
      {
      }
    };

    class mouse_double_click final : public mouse_event_base
    {
      mouse_double_click(std::vector<mouse_button> button, fvec2 pos)
        : mouse_event_base {std::move(button), pos}
      {
      }
    };

    class mouse_hover final : public mouse_event_base
    {
      mouse_hover(std::vector<mouse_button> button, fvec2 pos)
        : mouse_event_base {std::move(button), pos}
      {
      }
    };

  } // namespace events

} // namespace yave::wm