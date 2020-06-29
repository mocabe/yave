//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave-imgui/node_window_drawables.hpp>
#include <yave-imgui/basic_node_drawer.hpp>
#include <yave-imgui/basic_socket_drawer.hpp>
#include <yave-imgui/basic_connection_drawer.hpp>
#include <yave-imgui/node_window.hpp>

#include <yave/editor/editor_data.hpp>
#include <yave/module/std/primitive/primitive.hpp>
#include <imgui_stdlib.h>
#include <iostream>

namespace yave::editor::imgui {

  using namespace yave::imgui;

  auto create_node_drawable(
    const node_handle& n,
    const structured_node_graph& g,
    const node_window& nw) -> std::unique_ptr<node_drawable>
  {
    return std::make_unique<basic_node_drawer>(n, g, nw);
  }

  auto create_socket_drawable(
    const socket_handle& s,
    const structured_node_graph& g,
    const node_window& nw) -> std::unique_ptr<socket_drawable>
  {
    return std::make_unique<basic_socket_drawer>(s, g, nw);
  }

  auto create_connection_drawable(
    const connection_handle& c,
    const structured_node_graph& g,
    const node_window& nw) -> std::unique_ptr<connection_drawable>
  {
    return std::make_unique<basic_connection_drawer>(c, g, nw);
  }

} // namespace yave::editor::imgui