//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/config/config.hpp>

namespace yave::editor::imgui {

  /// Editor input mode
  enum class input_mode
  {
    neutral,      ///< Neutral
    socket_drag,  ///< Dragging connection
    range_select, ///< Range select
  };

  /// Editor input state.
  class input_state
  {
  public:
    /// ctor
    input_state();

    /// Set new input mode
    void set_input_mode(input_mode mode);
    /// Get current input mode
    auto get_input_mode() const -> input_mode;

  private:
    input_mode m_mode;
  };
}