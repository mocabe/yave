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
}