//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/core/properties.hpp>
#include <yave/obj/vec/vec.hpp>

namespace yave {

  // arg

  constexpr auto arg_property_name = "arg";

  void set_arg(
    structured_node_graph& ng,
    socket_handle s,
    object_ptr<NodeArgument> holder)
  {
    ng.set_property(s, arg_property_name, std::move(holder));
  }

  auto get_arg(const structured_node_graph& ng, socket_handle s)
    -> object_ptr<NodeArgument>
  {
    return ng.get_property<NodeArgument>(s, arg_property_name);
  }

  // pos

  constexpr auto pos_property_name = "pos";

  void set_pos(structured_node_graph& ng, node_handle n, glm::vec2 new_pos)
  {
    if (auto pos = ng.get_property<Vec2>(n, pos_property_name)) {
      *pos = new_pos;
      return;
    }
    ng.set_property(n, pos_property_name, make_object<Vec2>(new_pos));
  }

  auto get_pos(const structured_node_graph& ng, node_handle n) -> glm::vec2
  {
    if (auto pos = ng.get_property<Vec2>(n, pos_property_name))
      return *pos;
    return glm::vec2();
  }
}