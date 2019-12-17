//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/parser/node_parser.hpp>
#include <yave/node/decl/keyframe.hpp>
#include <yave/node/decl/frame_time.hpp>
#include <yave/node/decl/identity.hpp>
#include <catch2/catch.hpp>

using namespace yave;

TEST_CASE("node_parser _extract")
{
  managed_node_graph graph;
  node_parser parser;

  auto prim_info = node_info("prim", {}, {"value"}, node_type::primitive);
  auto norm_info = node_info("node", {"0", "1"}, {"0"}, node_type::normal);

  REQUIRE(graph.register_node_info(prim_info));
  REQUIRE(graph.register_node_info(norm_info));

  auto g = graph.root_group();
  graph.add_group_output_socket(g, "global_out");

  // global out socket
  auto gos = graph.input_sockets(graph.get_group_output(g))[0];

  SECTION("prim")
  {
    // prim
    auto n = graph.create(g, prim_info.name());
    REQUIRE(n);

    REQUIRE(graph.set_primitive(n, 42));

    // connect to global_out
    auto c = graph.connect(graph.output_sockets(n)[0], gos);

    REQUIRE(c);

    auto parsed   = parser.parse(graph);
    auto parsed_n = parsed->get_group_members(parsed->root_group())[0];

    REQUIRE(parsed);
    REQUIRE(parsed->get_group_members(parsed->root_group()).size() == 1);
    REQUIRE(parsed_n.id() == n.id());

    REQUIRE(graph.set_primitive(n, 24));
    REQUIRE(parsed->get_primitive(parsed_n));
    REQUIRE(*parsed->get_primitive(parsed_n) == primitive_t(42));
  }

  SECTION("norm")
  {
    auto prim = graph.create(g, prim_info.name());
    auto norm = graph.create(g, norm_info.name());

    auto prim_value = graph.output_sockets(prim)[0];
    auto norm_i0    = graph.input_sockets(norm)[0];
    auto norm_i1    = graph.input_sockets(norm)[1];
    auto norm_o0    = graph.output_sockets(norm)[0];

    REQUIRE(prim);
    REQUIRE(norm);

    SECTION("no globl out")
    {
      // gos
      auto parsed = parser.parse(graph);
      REQUIRE(!parsed);
    }

    SECTION("not sufficient")
    {
      // gos <- norm
      REQUIRE(graph.connect(norm_o0, gos));
      auto parsed = parser.parse(graph);
      REQUIRE(!parsed);
    }

    SECTION("single")
    {
      REQUIRE(graph.connect(norm_o0, gos));
      REQUIRE(graph.connect(prim_value, norm_i0));

      SECTION("0")
      {
        // gos <- norm <- prim
      }

      SECTION("1")
      {
        // gos <- [<- norm <-] <- prim
        REQUIRE(graph.group(g, {norm}));
      }

      SECTION("2")
      {
        // gos <- norm <- [<- prim ]
        REQUIRE(graph.group(g, {prim}));
      }

      SECTION("3")
      {
        REQUIRE(graph.group(g, {prim, norm}));
      }

      SECTION("4")
      {
        // gos <- [<- norm <-] <- prim
        //                 <-]
        auto g1 = graph.group(g, {norm});
        REQUIRE(g1);
        REQUIRE(graph.add_group_input_socket(g1, "test"));
        REQUIRE(graph.connect(
          graph.output_sockets(graph.get_group_input(g1))[1], norm_i1));
      }

      auto parsed = parser.parse(graph);
      REQUIRE(!parsed);
    }

    SECTION("multi")
    {
      REQUIRE(graph.connect(norm_o0, gos));
      REQUIRE(graph.connect(prim_value, norm_i0));
      REQUIRE(graph.connect(prim_value, norm_i1));

      SECTION("0")
      {
      }

      SECTION("1")
      {
        REQUIRE(graph.group(g, {norm}));
      }

      SECTION("2")
      {
        REQUIRE(graph.group(g, {prim}));
      }

      SECTION("3")
      {
        REQUIRE(graph.group(g, {norm, prim}));
      }

      auto parsed = parser.parse(graph);
      REQUIRE(parsed);
    }
  }
}