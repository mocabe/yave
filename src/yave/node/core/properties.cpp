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
    uint64_t idx,
    const object_ptr<NodeArgument>& arg,
    const node_handle& n,
    structured_node_graph& ng)
  {
    assert(ng.exists(n));

    // [0]: Path of source declaratoin
    auto p0 = make_object<PropertyTreeNode>(
      "path", make_object<String>(*ng.get_path(n)));
    // [1]: Index of socket
    auto p1 = make_object<PropertyTreeNode>(
      "idx", make_object<Int>(static_cast<int64_t>(idx)));
    // [2]: Property value tree
    auto p2 = arg->property().clone();

    ng.set_property(
      ng.input_sockets(n)[idx],
      arg_property_name,
      make_object<PropertyTreeNode>(
        arg_property_name,
        object_type<NodeArgument>(),
        std::vector {p0, p1, p2}));
  }

  template <class Decl>
  concept _decl_has_default_arguments = requires(Decl d)
  {
    d.default_args();
  };

  auto get_arg(
    const socket_handle& s,
    const structured_node_graph& ng,
    const node_declaration_map& decls) -> object_ptr<NodeArgument>
  {
    if (auto p = ng.get_property(s, arg_property_name)) {
      auto path = p->children()[0]->get_value<String>();
      auto idx  = p->children()[1]->get_value<Int>();
      auto prop = p->children()[2];

      if (auto d = decls.find(*path)) {
        return d->visit([&](auto&& x) -> object_ptr<NodeArgument> {
          // find source default argument
          if constexpr (_decl_has_default_arguments<decltype(x)>) {
            for (auto&& [i, a] : x.default_args()) {
              if (i == static_cast<uint64_t>(*idx))
                return make_object<NodeArgument>(prop, a->generator());
            }
          }
          return nullptr;
        });
      }
    }
    return nullptr;
  }

  auto get_arg_property(const socket_handle& s, const structured_node_graph& ng)
    -> object_ptr<PropertyTreeNode>
  {
    if (auto p = ng.get_property(s, arg_property_name))
      return p->children()[2];
    return nullptr;
  }

  // pos

  constexpr auto pos_property_name = "pos";

  void set_pos(
    const glm::vec2& new_pos,
    const node_handle& n,
    structured_node_graph& ng)
  {
    // update current value
    if (auto p = ng.get_property(n, pos_property_name)) {
      auto x = p->children()[0];
      auto y = p->children()[1];
      x->set_value(make_object<Float>(new_pos.x));
      y->set_value(make_object<Float>(new_pos.y));
      return;
    }

    // add new property
    ng.set_property(
      n,
      pos_property_name,
      make_object<PropertyTreeNode>(
        pos_property_name,
        object_type<Vec2>(),
        std::vector {
          make_object<PropertyTreeNode>("x", make_object<Float>(new_pos.x)),
          make_object<PropertyTreeNode>("y", make_object<Float>(new_pos.y)),
        }));
  }

  auto get_pos(const node_handle& n, const structured_node_graph& ng)
    -> glm::vec2
  {
    if (auto p = ng.get_property(n, pos_property_name)) {
      auto x = p->children()[0]->get_value<Float>();
      auto y = p->children()[1]->get_value<Float>();
      return {*x, *y};
    }
    return {};
  }
}