//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <catch2/catch.hpp>

#include <yave/node/parser/node_parser.hpp>
#include <yave/core/rts.hpp>

using namespace yave;

TEST_CASE("node_parser")
{
  node_graph graph;
  node_info_manager info_mngr(get_primitive_info_list());
  bind_info_manager bind_mngr(get_primitive_bind_info_list());
  node_parser parser(graph, bind_mngr);

  error_list errors;

  SECTION("Int")
  {
    auto Int_info = info_mngr.find("Int");
    auto Int_bind = bind_mngr.find("Int");

    REQUIRE(Int_info);
    REQUIRE(Int_bind.size() == 1);

    auto n = graph.add(*Int_info);
    // should be Frame->Int
    auto tp = parser.type_prime_tree(n, "value", errors);
    REQUIRE(same_type(tp, get_primitive_type(int(0))));
  }
}