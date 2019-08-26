//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/scene/scene_graph.hpp>
#include <catch2/catch.hpp>

using namespace yave;

TEST_CASE("scene_graph")
{
  scene_graph graph;
  auto root = graph.root();

  SECTION("layer control")
  {
    /// owning resources    - automatic resources
    ///                     - manual resources
    /// inherited resources - visible resources
    ///                     - inherited resource references

    /// initial root node is empty compos layer
    REQUIRE(root);
    REQUIRE(graph.exists(root));
    // compos
    REQUIRE(graph.is_compos_layer(root));
    REQUIRE(!graph.is_image_layer(root));
    // no sublayer
    REQUIRE(graph.get_sublayers(root).empty());
    // no visible resources
    REQUIRE(graph.get_resources(root).empty());
    REQUIRE(graph.get_resources_owning(root).empty());
    REQUIRE(graph.get_resources_reference(root).empty());

    REQUIRE(graph.is_visible(root));

    SECTION("null")
    {
      REQUIRE(!graph.is_compos_layer(nullptr));
      REQUIRE(!graph.is_image_layer(nullptr));
      REQUIRE(graph.get_resources(nullptr).empty());
      REQUIRE(graph.get_resources_owning(nullptr).empty());
      REQUIRE(graph.get_resources_reference(nullptr).empty());
      REQUIRE(graph.get_sublayers(nullptr).empty());
      REQUIRE(!graph.add_layer(nullptr, layer_type::image));
      REQUIRE(!graph.add_layer(nullptr, layer_type::compos));
    }

    SECTION("del")
    {
      graph.remove_layer(root);
      REQUIRE(graph.exists(root));
    }

    SECTION("image")
    {
      // image
      auto l1 = graph.add_layer(root, layer_type::image);

      REQUIRE(l1);
      REQUIRE(graph.exists(l1));
      REQUIRE(graph.is_image_layer(l1));
      // image layers never have sublayers
      REQUIRE(graph.get_sublayers(l1).empty());
      // image output node
      REQUIRE(graph.get_resources(l1).size() == 1);
      REQUIRE(graph.get_resources_owning(l1).size() == 1);
      REQUIRE(graph.get_resources_reference(l1).empty());

      // image layer cannot have sublayer
      REQUIRE(!graph.add_layer(l1, layer_type::image));
      REQUIRE(!graph.add_layer(l1, layer_type::compos));

      SECTION("del")
      {
        graph.remove_layer(l1);
        REQUIRE(!graph.exists(l1));
      }
    }

    SECTION("compos")
    {
      auto l1 = graph.add_layer(root, layer_type::compos);

      REQUIRE(l1);
      REQUIRE(graph.exists(l1));
      REQUIRE(!graph.is_image_layer(l1));
      REQUIRE(graph.is_compos_layer(l1));

      REQUIRE(graph.get_sublayers(l1).empty());

      REQUIRE(graph.get_resources(l1).empty());
      REQUIRE(graph.get_resources_owning(l1).empty());
      REQUIRE(graph.get_resources_reference(l1).empty());
    }
  }

  SECTION("move functions")
  {
    SECTION("default")
    {
      REQUIRE(!graph.movable_into(root, root));
      REQUIRE(!graph.movable_below(root, root));
      REQUIRE(!graph.movable_above(root, root));
    }

    SECTION("move into")
    {
      SECTION("image")
      {
        auto l1 = graph.add_layer(root, layer_type::image);
        REQUIRE(!graph.movable_into(l1, root));
        REQUIRE(!graph.movable_into(root, l1));
      }
      SECTION("compos")
      {
        auto l1 = graph.add_layer(root, layer_type::compos);
        REQUIRE(!graph.movable_into(l1, root));
        REQUIRE(!graph.movable_into(root, l1));
      }

      SECTION("move")
      {
        /* move single layer */

        auto l1 = graph.add_layer(root, layer_type::image);
        auto l2 = graph.add_layer(root, layer_type::compos);

        REQUIRE(graph.movable_into(l1, l2));
        REQUIRE(!graph.movable_into(l2, l1));

        graph.move_into(l1, l2);

        REQUIRE(graph.exists(l1));
        REQUIRE(graph.exists(l2));

        REQUIRE(graph.get_parent(l1) == l2);
        REQUIRE(graph.get_parent(l2) == root);

        /* move nested layer */

        auto l3 = graph.add_layer(root, layer_type::compos);
        auto l4 = graph.add_layer(l3, layer_type::image);

        graph.move_into(l2, l3);

        REQUIRE(graph.get_sublayers(l3)[0] == l4);
        REQUIRE(graph.get_sublayers(l3)[1] == l2);
        REQUIRE(graph.get_parent(l1) == l2);
        REQUIRE(graph.get_parent(l2) == l3);
      }
    }

    SECTION("move above")
    {
      SECTION("image")
      {
        auto l1 = graph.add_layer(root, layer_type::image);
        REQUIRE(!graph.movable_above(l1, root));
        REQUIRE(!graph.movable_above(root, l1));

        auto l2 = graph.add_layer(root, layer_type::image);
        REQUIRE(graph.movable_above(l1, l2));
        REQUIRE(graph.movable_above(l2, l1));
      }
      SECTION("compos")
      {
        auto l1 = graph.add_layer(root, layer_type::compos);
        REQUIRE(!graph.movable_above(l1, root));
        REQUIRE(!graph.movable_above(root, l1));

        auto l2 = graph.add_layer(root, layer_type::compos);
        REQUIRE(graph.movable_above(l1, l2));
        REQUIRE(graph.movable_above(l2, l1));
      }
      SECTION("move")
      {
        auto l1 = graph.add_layer(root, layer_type::image);
        auto l2 = graph.add_layer(root, layer_type::compos);

        REQUIRE(graph.get_sublayers(root)[0] == l1);
        REQUIRE(graph.get_sublayers(root)[1] == l2);

        REQUIRE(graph.movable_above(l1, l2));
        REQUIRE(graph.movable_above(l2, l1));
        graph.move_above(l1, l2);

        REQUIRE(graph.get_sublayers(root)[0] == l2);
        REQUIRE(graph.get_sublayers(root)[1] == l1);

        auto l3 = graph.add_layer(l2, layer_type::image);

        graph.move_above(l2, l1);
        REQUIRE(graph.get_sublayers(root)[0] == l1);
        REQUIRE(graph.get_sublayers(root)[1] == l2);
        REQUIRE(graph.get_parent(l3) == l2);
      }
    }

    SECTION("move below")
    {
      SECTION("image")
      {
        auto l1 = graph.add_layer(root, layer_type::image);
        REQUIRE(!graph.movable_below(l1, root));
        REQUIRE(!graph.movable_below(root, l1));
      }
      SECTION("compos")
      {
        auto l1 = graph.add_layer(root, layer_type::compos);
        REQUIRE(!graph.movable_below(l1, root));
        REQUIRE(!graph.movable_below(root, l1));
      }
      SECTION("move")
      {
        auto l1 = graph.add_layer(root, layer_type::image);
        auto l2 = graph.add_layer(root, layer_type::compos);

        REQUIRE(graph.get_sublayers(root)[0] == l1);
        REQUIRE(graph.get_sublayers(root)[1] == l2);

        REQUIRE(graph.movable_below(l1, l2));
        REQUIRE(graph.movable_below(l2, l1));
        graph.move_below(l2, l1);

        REQUIRE(graph.get_sublayers(root)[0] == l2);
        REQUIRE(graph.get_sublayers(root)[1] == l1);

        auto l3 = graph.add_layer(l2, layer_type::image);

        graph.move_below(l1, l2);
        REQUIRE(graph.get_sublayers(root)[0] == l1);
        REQUIRE(graph.get_sublayers(root)[1] == l2);
        REQUIRE(graph.get_parent(l3) == l2);
      }
    }
  }
}