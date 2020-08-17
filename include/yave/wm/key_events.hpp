//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/wm/event.hpp>
#include <yave/wm/event_dispatcher.hpp>
#include <yave/wm/window_traverser.hpp>
#include <yave/lib/glfw/glfw_context.hpp>
#include <yave/support/enum_flag.hpp>

#include <vector>
#include <algorithm>

namespace yave::wm {

  /// key input
  enum class key : int
  {
    unknown       = GLFW_KEY_UNKNOWN,
    space         = GLFW_KEY_SPACE,
    apostrophe    = GLFW_KEY_APOSTROPHE,
    comma         = GLFW_KEY_COMMA,
    minus         = GLFW_KEY_MINUS,
    period        = GLFW_KEY_PERIOD,
    slash         = GLFW_KEY_SLASH,
    _0            = GLFW_KEY_0,
    _1            = GLFW_KEY_1,
    _2            = GLFW_KEY_2,
    _3            = GLFW_KEY_3,
    _4            = GLFW_KEY_4,
    _5            = GLFW_KEY_5,
    _6            = GLFW_KEY_6,
    _7            = GLFW_KEY_7,
    _8            = GLFW_KEY_8,
    _9            = GLFW_KEY_9,
    semicolon     = GLFW_KEY_SEMICOLON,
    equal         = GLFW_KEY_EQUAL,
    a             = GLFW_KEY_A,
    b             = GLFW_KEY_B,
    c             = GLFW_KEY_C,
    d             = GLFW_KEY_D,
    e             = GLFW_KEY_E,
    f             = GLFW_KEY_F,
    g             = GLFW_KEY_G,
    h             = GLFW_KEY_H,
    i             = GLFW_KEY_I,
    j             = GLFW_KEY_J,
    k             = GLFW_KEY_K,
    l             = GLFW_KEY_L,
    m             = GLFW_KEY_M,
    n             = GLFW_KEY_N,
    o             = GLFW_KEY_O,
    p             = GLFW_KEY_P,
    q             = GLFW_KEY_Q,
    r             = GLFW_KEY_R,
    s             = GLFW_KEY_S,
    t             = GLFW_KEY_T,
    u             = GLFW_KEY_U,
    v             = GLFW_KEY_V,
    w             = GLFW_KEY_W,
    x             = GLFW_KEY_X,
    y             = GLFW_KEY_Y,
    z             = GLFW_KEY_Z,
    left_bracket  = GLFW_KEY_LEFT_BRACKET,
    backslash     = GLFW_KEY_BACKSLASH,
    right_bracket = GLFW_KEY_RIGHT_BRACKET,
    grave_accent  = GLFW_KEY_GRAVE_ACCENT,
    world_1       = GLFW_KEY_WORLD_1,
    world_2       = GLFW_KEY_WORLD_2,
    escape        = GLFW_KEY_ESCAPE,
    enter         = GLFW_KEY_ENTER,
    tab           = GLFW_KEY_TAB,
    backspace     = GLFW_KEY_BACKSPACE,
    insert        = GLFW_KEY_INSERT,
    delete_       = GLFW_KEY_DELETE,
    right         = GLFW_KEY_RIGHT,
    left          = GLFW_KEY_LEFT,
    down          = GLFW_KEY_DOWN,
    up            = GLFW_KEY_UP,
    page_up       = GLFW_KEY_PAGE_UP,
    page_down     = GLFW_KEY_PAGE_DOWN,
    home          = GLFW_KEY_HOME,
    end           = GLFW_KEY_END,
    caps_lock     = GLFW_KEY_CAPS_LOCK,
    scroll_lock   = GLFW_KEY_SCROLL_LOCK,
    num_lock      = GLFW_KEY_NUM_LOCK,
    print_screen  = GLFW_KEY_PRINT_SCREEN,
    pause         = GLFW_KEY_PAUSE,
    f1            = GLFW_KEY_F1,
    f2            = GLFW_KEY_F2,
    f3            = GLFW_KEY_F3,
    f4            = GLFW_KEY_F4,
    f5            = GLFW_KEY_F5,
    f6            = GLFW_KEY_F6,
    f7            = GLFW_KEY_F7,
    f8            = GLFW_KEY_F8,
    f9            = GLFW_KEY_F9,
    f10           = GLFW_KEY_F10,
    f11           = GLFW_KEY_F11,
    f12           = GLFW_KEY_F12,
    f13           = GLFW_KEY_F13,
    f14           = GLFW_KEY_F14,
    f15           = GLFW_KEY_F15,
    f16           = GLFW_KEY_F16,
    f17           = GLFW_KEY_F17,
    f18           = GLFW_KEY_F18,
    f19           = GLFW_KEY_F19,
    f20           = GLFW_KEY_F20,
    f21           = GLFW_KEY_F21,
    f22           = GLFW_KEY_F22,
    f23           = GLFW_KEY_F23,
    f24           = GLFW_KEY_F24,
    f25           = GLFW_KEY_F25,
    kp_0          = GLFW_KEY_KP_0,
    kp_1          = GLFW_KEY_KP_1,
    kp_2          = GLFW_KEY_KP_2,
    kp_3          = GLFW_KEY_KP_3,
    kp_4          = GLFW_KEY_KP_4,
    kp_5          = GLFW_KEY_KP_5,
    kp_6          = GLFW_KEY_KP_6,
    kp_7          = GLFW_KEY_KP_7,
    kp_8          = GLFW_KEY_KP_8,
    kp_9          = GLFW_KEY_KP_9,
    kp_decimal    = GLFW_KEY_KP_DECIMAL,
    kp_divide     = GLFW_KEY_KP_DIVIDE,
    kp_multiply   = GLFW_KEY_KP_MULTIPLY,
    kp_subtract   = GLFW_KEY_KP_SUBTRACT,
    kp_add        = GLFW_KEY_KP_ADD,
    kp_enter      = GLFW_KEY_KP_ENTER,
    kp_equal      = GLFW_KEY_KP_EQUAL,
    left_shift    = GLFW_KEY_LEFT_SHIFT,
    left_control  = GLFW_KEY_LEFT_CONTROL,
    left_alt      = GLFW_KEY_LEFT_ALT,
    left_super    = GLFW_KEY_LEFT_SUPER,
    right_shift   = GLFW_KEY_RIGHT_SHIFT,
    right_control = GLFW_KEY_RIGHT_CONTROL,
    right_alt     = GLFW_KEY_RIGHT_ALT,
    right_super   = GLFW_KEY_RIGHT_SUPER,
    menu          = GLFW_KEY_MENU,
  };

  /// key modifier
  enum class key_modifier_flags : int
  {
    none      = 0,
    shift     = GLFW_MOD_SHIFT,
    control   = GLFW_MOD_CONTROL,
    alt       = GLFW_MOD_ALT,
    super     = GLFW_MOD_SUPER,
    caps_lock = GLFW_MOD_CAPS_LOCK,
    num_lock  = GLFW_MOD_NUM_LOCK,
  };

  /// key state
  enum class key_state
  {
    up   = GLFW_RELEASE,
    down = GLFW_PRESS,
  };

  /// raw key event
  enum class key_event
  {
    release = GLFW_RELEASE,
    press   = GLFW_PRESS,
    repeat  = GLFW_REPEAT,
  };

  /// get key name
  [[nodiscard]] auto get_key_name(const glfw::glfw_context& glfw, wm::key key)
    -> std::u8string;

  namespace events {

    class key_event : public event
    {
    protected:
      key_event(wm::key key)
        : m_key {key}
      {
      }

    protected:
      wm::key m_key;
    };

    class key_press final : public key_event
    {
      wm::key_event m_event;
      wm::key_modifier_flags m_mods;

    public:
      key_press(wm::key key, wm::key_event event, wm::key_modifier_flags mods)
        : key_event(key)
        , m_event {event}
        , m_mods {mods}
      {
      }

      auto& key() const
      {
        return m_key;
      }

      auto& modifiers() const
      {
        return m_mods;
      }

      /// auto repeat
      [[nodiscard]] bool is_repeat() const
      {
        return m_event == wm::key_event::repeat;
      }

      /// test modifiers
      [[nodiscard]] bool test_modifiers(wm::key_modifier_flags mods) const;
      [[nodiscard]] bool shift() const;
      [[nodiscard]] bool control() const;
      [[nodiscard]] bool alt() const;
      [[nodiscard]] bool super() const;
      [[nodiscard]] bool caps_lock() const;
      [[nodiscard]] bool num_lock() const;
    };

    class key_release final : public key_event
    {
    public:
      key_release(wm::key key)
        : key_event(key)
      {
      }

      auto& key() const
      {
        return m_key;
      }
    };

    class key_char final : public key_event
    {
      std::u8string m_str;

    public:
      key_char(std::u8string str)
        : key_event(key::unknown)
        , m_str {str}
      {
      }

      auto& chars() const
      {
        return m_str;
      }
    };

  } // namespace events

  /// key event visitor
  template <class KeyEvent>
  struct key_event_visitor : event_visitor
  {
    key_event_visitor(
      std::unique_ptr<KeyEvent>&& e,
      const editor::data_context& dctx,
      const editor::view_context& vctx)
      : event_visitor(std::move(e), dctx, vctx)
    {
    }
  };

  using key_press_dispatcher = composed_event_dispatcher<
    key_event_visitor<events::key_press>,
    dfs_traverser_reverse_pre>;

  using key_release_dispatcher = composed_event_dispatcher<
    key_event_visitor<events::key_release>,
    dfs_traverser_reverse_pre>;

  using key_char_dispatcher = composed_event_dispatcher<
    key_event_visitor<events::key_char>,
    dfs_traverser_reverse_pre>;

} // namespace yave::wm

YAVE_DECL_ENUM_FLAG(yave::wm::key_modifier_flags)