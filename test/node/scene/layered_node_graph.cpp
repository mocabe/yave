//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/scene/layered_node_graph.hpp>
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

    REQUIRE(graph.get_sublayers(root)[0] == l1);
    REQUIRE(graph.get_sublayers(l1)[2] == l4);
    REQUIRE(graph.get_sublayers(l1)[1] == l2);
    REQUIRE(graph.get_sublayers(l1)[0] == l3);
    REQUIRE(graph.get_sublayers(l2)[0] == l5);
  }
}
