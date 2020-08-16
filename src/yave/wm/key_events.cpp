//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/wm/key_events.hpp>

namespace yave::wm {

  bool events::key_press::test_modifiers(wm::key_modifier_flags mods)
  {
    return !!(m_mods & mods);
  }

  auto get_key_name(const glfw::glfw_context&, wm::key key) -> std::u8string
  {
    if (auto name = (const char8_t*)glfwGetKeyName(static_cast<int>(key), 0))
      return name;
    else
      return u8"";
  }
} // namespace yave::wm