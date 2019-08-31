//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/parser/node_parser.hpp>
#include <yave/node/obj/keyframe.hpp>
#include <yave/node/obj/frame.hpp>
#include <yave/node/obj/identity.hpp>
#include <catch2/catch.hpp>

using namespace yave;

TEST_CASE("node_parser _extract")
{
  node_graph graph;
  node_parser parser;

  auto prim_info = node_info("prim", {}, {"value"}, true);
  auto norm_info = node_info("node", {"0", "1"}, {"0"}, false);

  SECTION("prim")
  {
    auto n = graph.add(prim_info, 42);
    REQUIRE(n);

    auto parsed = parser.parse(graph, n);

    REQUIRE(parsed);
    REQUIRE(parsed->graph.nodes().size() == 1);
    REQUIRE(parsed->graph.nodes()[0].id() == parsed->root.id());

    auto parsed_root = parsed->graph.nodes()[0];
    REQUIRE(parsed->graph.get_info(parsed_root) == prim_info);
    REQUIRE(parsed->graph.get_primitive(parsed_root));
    REQUIRE(parsed->graph.get_primitive(parsed_root) == primitive_t(42));
  }

  SECTION("norm")
  {
    auto prim = graph.add(prim_info, 42);
    auto norm = graph.add(norm_info);

    REQUIRE(prim);
    REQUIRE(norm);

    SECTION("prim")
    {
      auto parsed = parser.parse(graph, prim);
      REQUIRE(parsed);
      REQUIRE(parsed->graph.nodes().size() == 1);
    }

    SECTION("not sufficient")
    {
      auto parsed = parser.parse(graph, norm);
      REQUIRE(!parsed);
    }

    SECTION("single")
    {
      auto c = graph.connect(prim, "value", norm, "0");
      REQUIRE(c);
      auto parsed = parser.parse(graph, norm);
      REQUIRE(parsed);
      REQUIRE(parsed->graph.nodes().size() == 2);
      REQUIRE(parser.get_errors().empty());
    }

    SECTION("multi")
    {
      auto c0 = graph.connect(prim, "value", norm, "0");
      auto c1 = graph.connect(prim, "value", norm, "1");

      REQUIRE(c0);
      REQUIRE(c1);

      auto parsed = parser.parse(graph, norm);
      REQUIRE(parsed);
      REQUIRE(parsed->graph.nodes().size() == 2);
    }
  }
}

TEST_CASE("node_parser _desugar")
{
  node_graph graph;
  node_parser parser;

  // Use dummy top node to prevent root node is changed by desugar pass.
  auto id_info = get_node_info<node::Identity>();
  auto id      = graph.add(id_info);

  SECTION("keyframe")
  {
    auto kf_info = get_node_info<node::KeyframeInt>();
    auto kf      = graph.add(kf_info, 42);

    // kf -> id
    REQUIRE(graph.connect(
      kf, kf_info.output_sockets()[0], id, id_info.input_sockets()[0]));

    SECTION("single")
    {
      auto parsed = parser.parse(graph, id);
      REQUIRE(!parsed);
    }

    SECTION("with input")
    {
      auto frame_info = get_node_info<node::Frame>();
      auto frame      = graph.add(frame_info);

      // frame -> kf (-> id)
      REQUIRE(graph.connect(
        frame, frame_info.output_sockets()[0], kf, kf_info.input_sockets()[0]));

      auto parsed = parser.parse(graph, id);
      REQUIRE(parsed);
    }
  }
}