//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/module/std/transform/translate.hpp>
#include <yave/module/std/primitive/primitive.hpp>
#include <yave/obj/frame_demand/frame_demand.hpp>
#include <yave/obj/primitive/primitive.hpp>
#include <yave/obj/mat/mat.hpp>
#include <yave/obj/vec/vec.hpp>

namespace yave {

  auto node_declaration_traits<node::Translate>::get_node_declaration()
    -> node_declaration
  {
    return node_declaration(
      "Translate", "/std/transform", "Move object", {"target", "vec"}, {"out"});
  }
} // namespace yave