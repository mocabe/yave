//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/scene/scene_graph.hpp>
#include <catch2/catch.hpp>

using namespace yave;

TEST_CASE("scene_graph")
{
  SECTION("layer control")
  {
    scene_graph graph;

    /// owning resources    - automatic resources
    ///                     - manual resources
    /// inherited resources - visible resources
    ///                     - inherited resource references

    /// initial root node is empty compos layer
    auto root = graph.root();
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
}