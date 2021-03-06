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
  auto fdecl = std::make_shared<node_declaration>(
    get_node_declaration<node::Num::Float>());

  REQUIRE(!tree.find(idecl));
  REQUIRE(!tree.find(fdecl));

  tree.add(idecl);
  REQUIRE(tree.find(idecl));

  tree.add(idecl);

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

  store.add(*idecl);
  store.add(*fdecl);

  // allows duplicate
  store.add(*fdecl);

  REQUIRE(store.find(idecl->full_name()));
  REQUIRE(store.find(fdecl->full_name()));

  store.remove(fdecl->full_name());
  REQUIRE(!store.find(fdecl->full_name()));
}