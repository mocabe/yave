//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/config/config.hpp>

namespace yave::app {

  // clang-format off

  // Editor state machine:
  //
  // States:
  //   Neutral, Background, Node, Socket
  //
  // Events:
  //   ClickOnBackground, ClickOnNode, ClickOnSocket, MouseDown, MouseUp
  //
  // Machine:
  //   Neutral    <ClickOnBackground> Background { ClearSelected(); BeginDrag() }
  //   Background <MouseDown>         Background { UpdateDragPos() }
  //   Background <MouseUp>           Neutral    { RangeSelectOnRelease(); EndDrag() }
  //   Neutral    <ClickOnNode>       Node       { SelectNode(); BeginDrag() }
  //   Node       <MouseDown>         Node       { UpdateDragPos() }
  //   Node       <MouseUp>           Neutral    { ApplyNewNodePos(); EndDrag }
  //   Neutral    <ClickOnSocket>     Socket     { SelectSocket(); BeginDrag() }
  //   Socket     <MouseDown>         Socket     { UpdateDragPos() }
  //   Socket     <MouseUp>           Nautral    { ConnectSocketsOnRelease(); EndDrag() }

  // clang-format on

  /// Editor state enum.
  enum class editor_state
  {
    neutral,    ///< Neutral state
    background, ///< Dragging background
    node,       ///< Dragging node
    socket,     ///< Dragging socket
  };

  inline auto to_string(const editor_state& state) -> std::string
  {
    switch (state) {
      case editor_state::neutral:
        return "editor_state::neutral";
      case editor_state::background:
        return "editor_state::background";
      case editor_state::node:
        return "editor_state::node_drag";
      case editor_state::socket:
        return "editor_state::socket_drag";
      default:
        unreachable();
    }
  }

} // namespace yave::app