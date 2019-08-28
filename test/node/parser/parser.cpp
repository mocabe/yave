//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/parser/node_parser.hpp>
#include <catch2/catch.hpp>

using namespace yave;

TEST_CASE("node_parser unit test")
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
    REQUIRE(parsed->graph.nodes()[0] == parsed->root);

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