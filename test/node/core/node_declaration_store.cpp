//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <catch2/catch.hpp>

#include <yave/node/core/node_declaration_store.hpp>
#include <yave/module/std/num/num.hpp>

using namespace yave;

TEST_CASE("node_declaration_tree")
{
  node_declaration_tree tree;

  auto idecl =
    std::make_shared<node_declaration>(get_node_declaration<node::Num::Int>());
  auto fdecl =
    std::make_shared<node_declaration>(get_node_declaration<node::Num::Float>());

  REQUIRE(!tree.find(idecl));
  REQUIRE(!tree.find(fdecl));

  REQUIRE(tree.insert(idecl));
  REQUIRE(tree.find(idecl));

  REQUIRE(!tree.insert(idecl));

  tree.remove(idecl);
  REQUIRE(!tree.find(idecl));
}

TEST_CASE("node_declaration_store")
{
  node_declaration_store store;

  auto idecl =
    std::make_shared<node_declaration>(get_node_declaration<node::Num::Int>());
  auto fdecl = std::make_shared<node_declaration>(
    get_node_declaration<node::Num::Float>());

  REQUIRE(store.add(*idecl));
  REQUIRE(store.add(*fdecl));

  // allows duplicate
  REQUIRE(store.add(*fdecl));

  REQUIRE(store.find(get_full_name(*idecl)));
  REQUIRE(store.find(get_full_name(*fdecl)));

  store.remove(get_full_name(*fdecl));
  REQUIRE(!store.find(get_full_name(*fdecl)));
}