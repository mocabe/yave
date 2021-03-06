//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave-imgui/node_window_drawables.hpp>
#include <yave-imgui/basic_node_drawer.hpp>
#include <yave-imgui/basic_socket_drawer.hpp>
#include <yave-imgui/basic_connection_drawer.hpp>
#include <yave-imgui/node_window.hpp>
#include <yave-imgui/data_type_socket.hpp>

#include <yave/node/core/properties.hpp>
#include <yave/editor/editor_data.hpp>
#include <yave/module/std/primitive/primitive.hpp>
#include <yave/module/std/color/color.hpp>
#include <yave/module/std/vec/vec.hpp>
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

  /// Generate matching socket drawer from data type
  template <class T, class... Ts>
  auto create_data_type_socket(
    meta_tuple<T, Ts...>,
    const object_ptr<PropertyTreeNode>& arg,
    const socket_handle& s,
    const structured_node_graph& g,
    const node_window& nw) -> std::unique_ptr<socket_drawable>
  {
    if (same_type(arg->type(), object_type<T>()))
      return std::make_unique<data_type_socket<T>>(arg, s, g, nw);

    if constexpr (sizeof...(Ts) > 0)
      return create_data_type_socket<Ts...>(tuple_c<Ts...>, arg, s, g, nw);
    else
      return nullptr;
  }

  auto create_socket_drawable(
    const socket_handle& s,
    const structured_node_graph& g,
    const node_window& nw) -> std::unique_ptr<socket_drawable>
  {
    // list of supported socket data types
    constexpr auto data_types = tuple_c<Int, Float, Bool, String, Color, Vec2>;

    // check data types
    if (auto arg = get_arg_property(s, g))
      if (auto ds = create_data_type_socket(data_types, arg, s, g, nw))
        return ds;

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