//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/compiler/node_compiler.hpp>
#include <yave/node/parser/node_parser.hpp>
#include <yave/node/objects/function.hpp>
#include <yave/node/objects/instance_getter.hpp>
#include <yave/core/rts.hpp>
#include <catch2/catch.hpp>

using namespace yave;

TEST_CASE("node_parser")
{
  node_graph graph;
  node_info_manager info_mngr(get_primitive_info_list());
  bind_info_manager bind_mngr(get_primitive_bind_info_list());
  node_parser parser(graph, bind_mngr);

  SECTION("Int")
  {
    auto Int_info = info_mngr.find("Int");
    auto Int_bind = bind_mngr.find("Int");

    REQUIRE(Int_info);
    REQUIRE(Int_bind.size() == 1);

    auto n = graph.add(*Int_info);
    graph.set_primitive(n, {(int)42});

    {
      // should be Frame->Int
      socket_instance_manager sim;
      auto [parsed_graph, errors] = parser.type_prime_tree(n, "value", sim);
      REQUIRE(parsed_graph);
      REQUIRE(errors.empty());
      REQUIRE(sim.size() == 1);
      REQUIRE(parsed_graph->nodes().size() == 1);
      REQUIRE(parsed_graph->roots().size() == 1);
      REQUIRE(same_type(
        parsed_graph->get_info(parsed_graph->nodes().front())->type(),
        get_primitive_type(int(0))));

      node_compiler compiler;
      auto exe = compiler.compile(*parsed_graph, parsed_graph->roots().front());
      auto result = exe.execute({0});
      REQUIRE(*value_cast<Int>(result) == 42);

      graph.set_primitive(n, {(int)24});
      exe    = compiler.compile(*parsed_graph, parsed_graph->roots().front());
      result = exe.execute({0});
      REQUIRE(*value_cast<Int>(result) == 24);
    }
  }

  SECTION("Simple Plus")
  {
    struct PlusInt : NodeFunction<PlusInt, Int, Int, Int>
    {
      return_type code() const
      {
        return new Int(*eval_arg<0>() + *eval_arg<1>());
      }
    };

    node_info info {"PlusInt", {"x", "y"}, {"value"}};
    bind_info bind {"PlusInt",
                    {"x", "y"},
                    "value",
                    make_object<InstanceGetterFunction<PlusInt>>(),
                    "x + y"};

    REQUIRE(info_mngr.add(info));
    REQUIRE(bind_mngr.add(bind));

    auto Int_info = info_mngr.find("Int");
    auto Int_bind = bind_mngr.find("Int");

    REQUIRE(Int_info);

    auto plus = graph.add(info);
    auto i1   = graph.add(*Int_info);
    auto i2   = graph.add(*Int_info);

    auto c1 = graph.connect(i1, "value", plus, "x");
    auto c2 = graph.connect(i2, "value", plus, "y");

    REQUIRE(c1);
    REQUIRE(c2);

    REQUIRE(graph.exists(c1));
    REQUIRE(graph.exists(c2));

    graph.set_primitive(i1, {(int)42});
    graph.set_primitive(i2, {(int)24});

    {
      socket_instance_manager sim;
      auto [graph, errors] = parser.type_prime_tree(plus, "value", sim);
      REQUIRE(errors.empty());
      REQUIRE(graph);
      REQUIRE(graph->nodes().size() == 3);
      REQUIRE(graph->roots().size() == 1);
      REQUIRE(sim.size() == 3);
      REQUIRE(same_type(
        graph->get_info(graph->roots().front())->type(),
        object_type<closure<Frame, Int>>()));

      node_compiler compiler;
      auto exe    = compiler.compile(*graph, graph->roots().front());
      auto result = exe.execute({0});

      REQUIRE(*value_cast<Int>(result) == 66);
    }
  }
}