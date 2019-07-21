//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/scene/layered_node_graph.hpp>
#include <yave/node/core/primitive.hpp>
#include <catch2/catch.hpp>

using namespace yave;

TEST_CASE("layered_node_graph")
{
  SECTION("init")
  {
    layered_node_graph graph;
  }

  SECTION("root")
  {
    layered_node_graph graph;
    auto handle = graph.root();
    REQUIRE(handle);
    REQUIRE(graph.get_owning_resources(handle).empty());
    REQUIRE(graph.get_inherited_resources(handle).empty());
    REQUIRE(graph.get_sublayers(handle).empty());
    REQUIRE(!graph.get_parent(handle));
    graph.remove_layer(handle);
    REQUIRE(handle == graph.root());
  }

  SECTION("add")
  {
    layered_node_graph graph; // 1
    REQUIRE(!graph.add_layer(nullptr));
    auto handle = graph.add_layer(graph.root()); // 2
    REQUIRE(handle);
    REQUIRE(graph.exists(handle));
    REQUIRE(graph.get_parent(handle) == graph.root());
    REQUIRE(graph.add_layer(graph.root())); // 3
    REQUIRE(graph.add_layer(handle));       // 4
    REQUIRE(graph.layers().size() == 4);
  }

  SECTION("remove")
  {
    layered_node_graph graph;
    auto handle = graph.add_layer(graph.root());
    graph.remove_layer(handle);
    REQUIRE(!graph.exists(handle));
    REQUIRE(graph.layers().size() == 1);

    auto l1 = graph.add_layer(graph.root());
    auto l2 = graph.add_layer(l1);
    auto l3 = graph.add_layer(l1);
    graph.remove_layer(l1);
    REQUIRE(!graph.exists(l1));
    REQUIRE(!graph.exists(l2));
    REQUIRE(!graph.exists(l3));
    REQUIRE(graph.layers().size() == 1);
  }

  SECTION("layer name")
  {
    layered_node_graph graph;
    auto handle = graph.add_layer(graph.root());
    graph.set_layer_name(handle, "abc");
    REQUIRE(graph.get_info(handle)->name() == "abc");
    REQUIRE(graph.get_layer_name(handle) == "abc");
  }

  SECTION("move_below")
  {
    layered_node_graph graph;
    auto root = graph.root();
    REQUIRE(!graph.movable_below(nullptr, nullptr));
    REQUIRE(!graph.movable_below(root, nullptr));
    REQUIRE(!graph.movable_below(nullptr, root));
    REQUIRE(!graph.movable_below(root, root));

    auto l1 = graph.add_layer(root);
    REQUIRE(!graph.movable_below(l1, l1));
    REQUIRE(!graph.movable_below(root, l1));
    REQUIRE(!graph.movable_below(l1, root));

    auto l2 = graph.add_layer(root);
    REQUIRE(graph.movable_below(l2, l1));
    REQUIRE(graph.movable_below(l1, l2));
    graph.move_below(l2, l1);
    REQUIRE(graph.exists(l1));
    REQUIRE(graph.exists(l2));
    REQUIRE(graph.get_sublayers(root)[1] == l1);
    REQUIRE(graph.get_sublayers(root)[0] == l2);

    graph.move_below(l1, l2);
    REQUIRE(graph.get_sublayers(root)[1] == l2);
    REQUIRE(graph.get_sublayers(root)[0] == l1);

    graph.move_below(l2, l1);

    /*
      root
        - l1
          - l4
          - l3
        - l2
          - l5
     */
    auto l3 = graph.add_layer(l1);
    auto l4 = graph.add_layer(l1);
    auto l5 = graph.add_layer(l2);

    REQUIRE(!graph.movable_below(l1, l3));
    REQUIRE(graph.movable_below(l3, l1));
    REQUIRE(graph.movable_below(l2, l3));

    /*
      root
        - l1
          - l4
          - l2
            - l5
          - l3
     */
    REQUIRE(graph.movable_below(l2, l4));
    graph.move_below(l2, l4);

    REQUIRE(graph.get_parent(l2) == l1);
    REQUIRE(graph.get_sublayers(root)[0] == l1);
    REQUIRE(graph.get_sublayers(l1)[2] == l4);
    REQUIRE(graph.get_sublayers(l1)[1] == l2);
    REQUIRE(graph.get_sublayers(l1)[0] == l3);
    REQUIRE(graph.get_sublayers(l2)[0] == l5);
  }

  SECTION("move_above")
  {
    layered_node_graph graph;
    REQUIRE(!graph.movable_above(graph.root(), graph.root()));
    REQUIRE(!graph.movable_above(nullptr, graph.root()));
    REQUIRE(!graph.movable_above(graph.root(), nullptr));

    auto l1 = graph.add_layer(graph.root());

    REQUIRE(!graph.movable_above(l1, graph.root()));
    REQUIRE(!graph.movable_above(graph.root(), l1));

    auto l2 = graph.add_layer(graph.root());
    REQUIRE(graph.movable_above(l1, l2));
    REQUIRE(graph.movable_above(l2, l1));

    graph.move_above(l1, l2);
    REQUIRE(graph.get_parent(l1) == graph.root());
    REQUIRE(graph.get_parent(l2) == graph.root());
    REQUIRE(graph.get_sublayers(graph.root())[0] == l2);
    REQUIRE(graph.get_sublayers(graph.root())[1] == l1);
  }

  SECTION("move_into")
  {
    layered_node_graph graph;
    auto root = graph.root();

    REQUIRE(!graph.movable_into(root, root));
    REQUIRE(!graph.movable_into(root, nullptr));
    REQUIRE(!graph.movable_into(nullptr, root));
    REQUIRE(!graph.movable_into(nullptr, nullptr));

    auto l1 = graph.add_layer(root);

    REQUIRE(!graph.movable_into(l1, l1));
    REQUIRE(graph.movable_into(l1, root));

    graph.move_into(l1, root);
    REQUIRE(graph.get_sublayers(root)[0] == l1);

    auto l2 = graph.add_layer(l1);

    REQUIRE(!graph.movable_into(l1, l2));
    REQUIRE(graph.movable_into(l2, l1));
    REQUIRE(graph.movable_into(l2, root));

    /*
      root - l1
           - l2
     */
    graph.move_into(l2, root);
    REQUIRE(graph.get_parent(l1) == root);
    REQUIRE(graph.get_parent(l2) == root);
    REQUIRE(graph.get_sublayers(l1).empty());
    REQUIRE(graph.get_sublayers(root)[0] == l1);
    REQUIRE(graph.get_sublayers(root)[1] == l2);
  }

  SECTION("resources")
  {
    layered_node_graph graph;

    auto root = graph.root();
    REQUIRE(graph.register_node_info(get_primitive_node_info_list()));

    auto r1 = graph.add_resource("Int", root, layer_resource_scope::Private);
    auto r2 = graph.add_resource("Double", root, layer_resource_scope::Inherit);

    REQUIRE(graph.exists(r1));
    REQUIRE(graph.exists(r1, root));
    REQUIRE(graph.get_resource_name(r1) == "Int");
    REQUIRE(graph.get_resource_scope(r1) == layer_resource_scope::Private);

    REQUIRE(graph.exists(r2));
    REQUIRE(graph.exists(r2, root));
    REQUIRE(graph.get_resource_name(r2) == "Double");
    REQUIRE(graph.get_resource_scope(r2) == layer_resource_scope::Inherit);

    REQUIRE(graph.get_owning_resources(root).size() == 2);
    REQUIRE(graph.get_owning_resources(root)[0] == r1);
    REQUIRE(graph.get_owning_resources(root)[1] == r2);
    REQUIRE(graph.get_inherited_resources(root).empty());

    auto l1 = graph.add_layer(root);

    REQUIRE(graph.get_inherited_resources(l1).size() == 1);
    REQUIRE(graph.get_inherited_resources(l1)[0] == r2);

    graph.set_resource_scope(r2, layer_resource_scope::Private);
    REQUIRE(graph.get_inherited_resources(l1).empty());

    graph.set_resource_name(r2, "Hello, World");
    REQUIRE(graph.get_resource_name(r2) == "Hello, World");

    graph.remove_resource(r2);
    REQUIRE(!graph.exists(r2));
    REQUIRE(!graph.exists(r2, root));
    REQUIRE(graph.get_owning_resources(root).size() == 1);
    REQUIRE(graph.get_owning_resources(root)[0] == r1);
  }

  SECTION("move_resource")
  {
    layered_node_graph graph;

    REQUIRE(graph.register_node_info(get_primitive_node_info_list()));

    auto root = graph.root();

    auto i1 = graph.add_resource("Int", root, layer_resource_scope::Private);

    REQUIRE(i1);

    auto l1 = graph.add_layer(root);

    REQUIRE(l1);

    graph.move_resource(i1, l1);

    REQUIRE(graph.get_owning_resources(l1)[0] == i1);
    REQUIRE(graph.get_owning_resources(root).empty());
  }
}
