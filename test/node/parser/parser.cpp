//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <catch2/catch.hpp>

#include <yave/node/parser/node_parser.hpp>
#include <yave/core/rts.hpp>
#include <yave/node/objects/function.hpp>
#include <yave/node/objects/instance_getter.hpp>

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
    // should be Frame->Int
    auto [tp, errors] = parser.type_prime_tree(n, "value");
    REQUIRE(errors.empty());
    REQUIRE(same_type(tp, get_primitive_type(int(0))));
  }

  SECTION("empty")
  {
    node_info info {"test", {"x"}, {"out"}};

    auto i = graph.add(*info_mngr.find("Int"));
    auto n = graph.add(info);

    REQUIRE(graph.connect(i, "value", n, "x"));
    auto [tp, errors] = parser.type_prime_tree(n, "out");
    REQUIRE(!errors.empty());
  }

  SECTION("Plus")
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

    {
      auto [tp, errors] = parser.type_prime_tree(plus, "value");
      REQUIRE(errors.empty());
      REQUIRE(same_type(tp, object_type<closure<Frame, Int>>()));
    }

    struct DoublePlus : NodeFunction<DoublePlus, Double, Double, Double>
    {
      return_type code() const
      {
        return new Double(*eval_arg<0>() + *eval_arg<1>());
      }
    };

    bind_info double_bind {"PlusInt",
                           {"x", "y"},
                           "value",
                           make_object<InstanceGetterFunction<DoublePlus>>(),
                           "x + y"};

    REQUIRE(bind_mngr.add(double_bind));

    {
      auto [tp, errors] = parser.type_prime_tree(plus, "value");
      REQUIRE(errors.empty());
      REQUIRE(same_type(tp, object_type<closure<Frame, Int>>()));
    }

    auto Double_info = info_mngr.find("Double");
    auto Double_bind = bind_mngr.find("Double");

    REQUIRE(Double_info);

    auto d1 = graph.add(*Double_info);
    auto d2 = graph.add(*Double_info);

    graph.disconnect(c1);
    graph.disconnect(c2);

    c1 = graph.connect(d1, "value", plus, "x");
    c2 = graph.connect(d2, "value", plus, "y");

    REQUIRE(c1);
    REQUIRE(c2);

    REQUIRE(graph.exists(c1));
    REQUIRE(graph.exists(c2));

    {
      auto [tp, errors] = parser.type_prime_tree(plus, "value");
      REQUIRE(errors.empty());
      REQUIRE(same_type(tp, object_type<closure<Frame, Double>>()));
    }

    struct InclDouble : NodeFunction<InclDouble, Double, Double>
    {
      return_type code() const
      {
        return new Double((*eval_arg<0>()) + 1);
      }
    };

    bind_info incl_bind {"PlusInt",
                         {"x"},
                         "value",
                         make_object<InstanceGetterFunction<InclDouble>>(),
                         ""};

    REQUIRE(bind_mngr.add(incl_bind));

    {
      auto [tp, errors] = parser.type_prime_tree(plus, "value");
    }
  }
}