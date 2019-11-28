//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave-imgui/editor/input_state.hpp>
#include <yave/support/log.hpp>

YAVE_DECL_G_LOGGER(input_state);

namespace yave::editor::imgui {

  input_state::input_state()
  {
    init_logger();
    m_mode = input_mode::neutral;
  }

  void input_state::set_input_mode(input_mode mode)
  {
    Info(g_logger, "input mode: {}", mode);
    m_mode = mode;
  }

  auto input_state::get_input_mode() const -> input_mode
  {
    return m_mode;
  }
}