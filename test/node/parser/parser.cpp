//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/compiler/compile.hpp>
#include <yave/compiler/message.hpp>
#include <yave/obj/primitive/primitive.hpp>
#include <yave/rts/value_cast.hpp>

#include <yave/support/log.hpp>

#include <catch2/catch.hpp>

using namespace yave;

TEST_CASE("parse")
{
  structured_node_graph ng;

  // single output
  auto decl1 = node_declaration("test.decl1", "", {}, {"out"});
  // multi output
  auto decl2 = node_declaration("test.decl2", "", {}, {"out1", "out2"});
  // singl in-out
  auto decl3 = node_declaration("test.decl3", "", {"in"}, {"out"});
  // single in-out with default argument
  auto decl4 = node_declaration(
    "test.decl4", "", {"in"}, {"out"}, {{0, make_object<Int>()}});
  // multi in-out
  auto decl5 =
    node_declaration("test.decl5", "", {"in1", "in2", "in3"}, {"out1", "out2"});

  REQUIRE(ng.search_path("/").empty());

  // parse cloned node graph
  auto test_parse =
    [](const structured_node_graph& ng, const socket_handle& os) {
      auto _ng   = ng.clone();
      auto _os   = _ng.socket(os.id());
      auto _defs = node_definition_store();

      auto pipe = compiler::init_pipeline();

      pipe
        .and_then([&](auto& p) {
          input(p, std::move(_ng), std::move(_os), std::move(_defs));
        })
        .and_then([&](auto& p) { parse(p); });

      return pipe.success();
    };

  SECTION("invalid out")
  {
    REQUIRE(!test_parse(ng, {}));
  }

  SECTION("root")
  {
    auto root  = ng.create_group({nullptr}, {});
    auto out_s = ng.add_output_socket(root, "out");
    ng.set_name(root, "root");

    REQUIRE(ng.exists(root));
    REQUIRE(ng.exists(out_s));

    SECTION("empty")
    {
      REQUIRE(!test_parse(ng, out_s));
    }

    SECTION("ro <- g[]")
    {
      auto g = ng.create_group(root, {});
      REQUIRE(ng.add_output_socket(g, "o0"));
      REQUIRE(ng.connect(
        ng.output_sockets(g)[0],
        ng.input_sockets(ng.get_group_output(root))[0]));

      REQUIRE(!test_parse(ng, out_s));

      REQUIRE(ng.add_input_socket(g, "in"));
      REQUIRE(!test_parse(ng, out_s));
    }

    SECTION("ro <- decl1")
    {
      auto f1 = ng.create_function(decl1);
      auto n  = ng.create_copy(root, f1);
      REQUIRE(ng.exists(n));
      REQUIRE(ng.connect(
        ng.output_sockets(n)[0],
        ng.input_sockets(ng.get_group_output(root))[0]));

      REQUIRE(test_parse(ng, out_s));
    }

    SECTION("ro <- decl2")
    {
      auto f1 = ng.create_function(decl2);
      auto n  = ng.create_copy(root, f1);
      REQUIRE(ng.exists(n));
      REQUIRE(ng.connect(
        ng.output_sockets(n)[1],
        ng.input_sockets(ng.get_group_output(root))[0]));

      REQUIRE(test_parse(ng, out_s));
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
      REQUIRE(test_parse(ng, out_s));

      REQUIRE(ng.connect(ng.output_sockets(n2)[0], ng.input_sockets(n1)[0]));

      REQUIRE(test_parse(ng, out_s));
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
      REQUIRE(test_parse(ng, out_s));

      REQUIRE(ng.connect(ng.output_sockets(n2)[0], ng.input_sockets(n1)[0]));
      REQUIRE(test_parse(ng, out_s));
    }

    SECTION("ro <- g[ <- decl1 ] (<-...)")
    {
      auto f1 = ng.create_function(decl1);
      auto g  = ng.create_group(root, {});
      ng.set_name(g, "g");
      REQUIRE(ng.add_output_socket(g, "out"));
      auto n = ng.create_copy(g, f1);
      REQUIRE(ng.connect(
        ng.output_sockets(n)[0], ng.input_sockets(ng.get_group_output(g))[0]));
      REQUIRE(ng.connect(
        ng.output_sockets(g)[0],
        ng.input_sockets(ng.get_group_output(root))[0]));

      REQUIRE(test_parse(ng, out_s));

      // now g is lambda
      REQUIRE(ng.add_input_socket(g, "in"));
      REQUIRE(ng.add_input_socket(g, "in"));
      REQUIRE(test_parse(ng, out_s));
      // g misses input
      auto m = ng.create_copy(root, f1);
      REQUIRE(ng.connect(ng.output_sockets(m)[0], ng.input_sockets(g)[1]));
      REQUIRE(!test_parse(ng, out_s));
    }

    SECTION("ro <- g[ <- decl3 <- ] (<-...)")
    {
      auto f = ng.create_function(decl3);
      auto g = ng.create_group(root, {});
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
      REQUIRE(test_parse(ng, out_s));
      REQUIRE(ng.add_input_socket(g, "in"));
      REQUIRE(test_parse(ng, out_s));

      auto m = ng.create_copy(root, f);
      REQUIRE(ng.connect(ng.output_sockets(m)[0], ng.input_sockets(g)[0]));
      // missint at g input
      REQUIRE(!test_parse(ng, out_s));
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

      REQUIRE(!test_parse(ng, out_s));

      REQUIRE(ng.connect(
        ng.output_sockets(f1)[0], ng.input_sockets(ng.get_group_output(g))[0]));
      REQUIRE(!test_parse(ng, out_s));

      REQUIRE(ng.connect(
        ng.output_sockets(f1)[0], ng.input_sockets(ng.get_group_output(g))[1]));
      REQUIRE(test_parse(ng, out_s));
    }
  }
}