//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/parser/node_parser.hpp>
#include <yave/obj/primitive/primitive.hpp>
#include <yave/rts/value_cast.hpp>

#include <catch2/catch.hpp>

using namespace yave;

TEST_CASE("node_parser v2")
{
  structured_node_graph ng;
  node_parser parser;

  // single output
  auto decl1 = node_declaration("decl1", "/test", "", {}, {"out"});
  // multi output
  auto decl2 = node_declaration("decl2", "/test", "", {}, {"out1", "out2"});
  // singl in-out
  auto decl3 = node_declaration("decl3", "/test", "", {"in"}, {"out"});
  // single in-out with default argument
  auto decl4 = node_declaration(
    "decl4", "/test", "", {"in"}, {"out"}, {{0, make_object<Int>()}});
  // multi in-out
  auto decl5 = node_declaration(
    "decl5", "/test", "", {"in1", "in2", "in3"}, {"out1", "out2"});

  REQUIRE(ng.search_path("/").empty());

  SECTION("no root")
  {
    REQUIRE(!parser.parse(std::move(ng)));
  }

  SECTION("no root out")
  {
    auto root = ng.create_group(nullptr);
    ng.set_name(root, "root");
    REQUIRE(!parser.parse(std::move(ng)));
  }

  SECTION("root")
  {
    auto root = ng.create_group(nullptr);
    ng.set_name(root, "root");
    REQUIRE(ng.add_output_socket(root, "out"));

    SECTION("empty")
    {
      REQUIRE(!parser.parse(std::move(ng)));
    }

    SECTION("ro <- g[]")
    {
      auto g = ng.create_group(root);
      REQUIRE(ng.add_output_socket(g, "o0"));
      REQUIRE(ng.connect(
        ng.output_sockets(g)[0],
        ng.input_sockets(ng.get_group_output(root))[0]));
      REQUIRE(!parser.parse(ng.clone()));

      REQUIRE(ng.add_input_socket(g, "in"));
      REQUIRE(!parser.parse(ng.clone()));
    }

    SECTION("ro <- decl1")
    {
      auto f1 = ng.create_function(decl1);
      auto n  = ng.create_copy(root, f1);
      REQUIRE(ng.exists(n));
      REQUIRE(ng.connect(
        ng.output_sockets(n)[0],
        ng.input_sockets(ng.get_group_output(root))[0]));
      REQUIRE(parser.parse(std::move(ng)));
    }

    SECTION("ro <- decl2")
    {
      auto f1 = ng.create_function(decl2);
      auto n  = ng.create_copy(root, f1);
      REQUIRE(ng.exists(n));
      REQUIRE(ng.connect(
        ng.output_sockets(n)[1],
        ng.input_sockets(ng.get_group_output(root))[0]));
      REQUIRE(parser.parse(std::move(ng)));
    }

    SECTION("ro <- decl3 <- decl1")
    {
      auto f1 = ng.create_function(decl3);
      auto f2 = ng.create_function(decl1);
      auto n1 = ng.create_copy(root, f1);
      auto n2 = ng.create_copy(root, f2);

      REQUIRE(ng.connect(
        ng.output_sockets(n1)[0],
        ng.input_sockets(ng.get_group_output(root))[0]));

      // decl3 is lambda
      REQUIRE(parser.parse(ng.clone()));

      REQUIRE(ng.connect(ng.output_sockets(n2)[0], ng.input_sockets(n1)[0]));
      REQUIRE(parser.parse(std::move(ng)));
    }

    SECTION("ro <- decl4 <- decl1")
    {
      auto f1 = ng.create_function(decl4);
      auto f2 = ng.create_function(decl1);
      auto n1 = ng.create_copy(root, f1);
      auto n2 = ng.create_copy(root, f2);

      REQUIRE(ng.connect(
        ng.output_sockets(n1)[0],
        ng.input_sockets(ng.get_group_output(root))[0]));

      // decl4 has defualt argument
      REQUIRE(parser.parse(ng.clone()));

      REQUIRE(ng.connect(ng.output_sockets(n2)[0], ng.input_sockets(n1)[0]));
      REQUIRE(parser.parse(std::move(ng)));
    }

    SECTION("ro <- g[ <- decl1 ] (<-...)")
    {
      auto f1 = ng.create_function(decl1);
      auto g  = ng.create_group(root);
      ng.set_name(g, "g");
      REQUIRE(ng.add_output_socket(g, "out"));
      auto n = ng.create_copy(g, f1);
      REQUIRE(ng.connect(
        ng.output_sockets(n)[0], ng.input_sockets(ng.get_group_output(g))[0]));
      REQUIRE(ng.connect(
        ng.output_sockets(g)[0],
        ng.input_sockets(ng.get_group_output(root))[0]));
      REQUIRE(parser.parse(ng.clone()));
      // now g is lambda
      REQUIRE(ng.add_input_socket(g, "in"));
      REQUIRE(ng.add_input_socket(g, "in"));
      REQUIRE(parser.parse(ng.clone()));
      // g misses input
      auto m = ng.create_copy(root, f1);
      REQUIRE(ng.connect(ng.output_sockets(m)[0], ng.input_sockets(g)[1]));
      REQUIRE(!parser.parse(ng.clone()));
    }

    SECTION("ro <- g[ <- decl3 <- ] (<-...)")
    {
      auto f = ng.create_function(decl3);
      auto g = ng.create_group(root);
      ng.set_name(g, "g");
      REQUIRE(ng.add_input_socket(g, "in"));
      REQUIRE(ng.add_output_socket(g, "out"));

      auto n = ng.create_copy(g, f);

      REQUIRE(ng.connect(
        ng.output_sockets(g)[0],
        ng.input_sockets(ng.get_group_output(root))[0]));
      REQUIRE(ng.connect(
        ng.output_sockets(n)[0], ng.input_sockets(ng.get_group_output(g))[0]));
      REQUIRE(ng.connect(
        ng.output_sockets(ng.get_group_input(g))[0], ng.input_sockets(n)[0]));

      // lambda
      REQUIRE(parser.parse(ng.clone()));

      REQUIRE(ng.add_input_socket(g, "in"));
      REQUIRE(parser.parse(ng.clone()));

      auto m = ng.create_copy(root, f);
      REQUIRE(ng.connect(ng.output_sockets(m)[0], ng.input_sockets(g)[0]));
      // missint at g input
      REQUIRE(!parser.parse(ng.clone()));
    }

    SECTION("ro <- decl5 <<<-<< g[ <<-< decl1 ]")
    {
      auto d1 = ng.create_function(decl1);
      auto d5 = ng.create_function(decl5);

      auto f5 = ng.create_copy(root, d5);
      auto f1 = ng.create_copy(root, d1);
      auto g  = ng.create_group(root, {f1});

      auto go1 = ng.add_output_socket(g, "out1");
      auto go2 = ng.add_output_socket(g, "out2");

      REQUIRE(ng.connect(
        ng.output_sockets(f5)[1],
        ng.input_sockets(ng.get_group_output(root))[0]));

      REQUIRE(ng.connect(go1, ng.input_sockets(f5)[0]));
      REQUIRE(ng.connect(go2, ng.input_sockets(f5)[1]));
      REQUIRE(ng.connect(go2, ng.input_sockets(f5)[2]));

      REQUIRE(!parser.parse(ng.clone()));

      REQUIRE(ng.connect(
        ng.output_sockets(f1)[0], ng.input_sockets(ng.get_group_output(g))[0]));
      REQUIRE(!parser.parse(ng.clone()));

      REQUIRE(ng.connect(
        ng.output_sockets(f1)[0], ng.input_sockets(ng.get_group_output(g))[1]));
      REQUIRE(parser.parse(ng.clone()));
    }
  }
}

TEST_CASE("node_parser _extract")
{
  managed_node_graph graph;
  node_declaration_store decls;
  node_parser parser;

  class X;

  auto prim_decl = node_declaration("prim", "/dummy", "", {}, {"prim_o0"});

  auto norm_decl =
    node_declaration("node", "/dummy", "", {"norm_i0", "norm_i1"}, {"norm_o0"});

  REQUIRE(decls.add(prim_decl));
  REQUIRE(decls.add(norm_decl));

  auto g = graph.root_group();
  graph.add_group_output_socket(g, "global_out");

  REQUIRE(graph.output_sockets(graph.root_group()).size() == 1);

  // global out socket
  auto gos = graph.input_sockets(graph.get_group_output(g))[0];

  SECTION("prim")
  {
    // prim
    auto n = graph.create(g, prim_decl);
    auto s = graph.output_sockets(n)[0];
    REQUIRE(n);
    REQUIRE(s);

    graph.set_data(s, make_object<Int>());

    REQUIRE(has_type<Int>(graph.get_data(s)));

    auto prim = value_cast<Int>(graph.get_data(s));
    *prim     = 42;

    REQUIRE(graph.get_data(s));
    REQUIRE(*value_cast<Int>(graph.get_data(s)) == 42);

    // connect to global_out
    auto c = graph.connect(graph.output_sockets(n)[0], gos);

    REQUIRE(c);

    auto parsed = parser.parse(graph);
    REQUIRE(parsed);
    auto parsed_n = parsed->get_group_members(parsed->root_group())[0];
    auto parsed_s = parsed->output_sockets(parsed_n)[0];

    REQUIRE(parsed);
    REQUIRE(parsed->get_group_members(parsed->root_group()).size() == 1);
    REQUIRE(parsed_n.id() == n.id());
    REQUIRE(parsed_s.id() == s.id());

    // primitive values are shared
    *prim = 24;

    REQUIRE(parsed->get_data(parsed_s));
    REQUIRE(*value_cast<Int>(graph.get_data(s)) == 24);
    REQUIRE(*value_cast<Int>(parsed->get_data(parsed_s)) == 24);
  }

  SECTION("norm")
  {
    auto prim = graph.create(g, prim_decl);
    auto norm = graph.create(g, norm_decl);

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

    SECTION("lambda")
    {
      // gos <- norm
      REQUIRE(graph.connect(norm_o0, gos));
      auto parsed = parser.parse(graph);
      REQUIRE(parsed);
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

    SECTION("misc")
    {
      auto g1    = graph.group(g, {norm, prim});
      auto g1_o0 = graph.add_group_output_socket(g1, "g1_o0");
      auto g1_o1 = graph.add_group_output_socket(g1, "g1_o1");

      REQUIRE(g1);
      REQUIRE(g1_o0);
      REQUIRE(g1_o1);

      SECTION("0")
      {
        REQUIRE(graph.connect(g1_o0, gos));
        REQUIRE(!parser.parse(graph));
      }

      SECTION("1")
      {
        REQUIRE(graph.connect(g1_o0, gos));
        REQUIRE(
          graph.connect(prim_value, graph.get_group_socket_inside(g1_o0)));
        REQUIRE(parser.parse(graph));
      }

      SECTION("2")
      {
        REQUIRE(graph.connect(g1_o0, gos));
        REQUIRE(
          graph.connect(prim_value, graph.get_group_socket_inside(g1_o1)));
        REQUIRE(!parser.parse(graph));
      }
    }
  }
}