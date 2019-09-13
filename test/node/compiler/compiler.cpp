//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/compiler/node_compiler.hpp>
#include <yave/node/scene/scene_graph.hpp>
#include <yave/node/core/function.hpp>
#include <yave/backend/default/render/primitive_constructor.hpp>
#include <catch2/catch.hpp>

using namespace yave;

class test_backend
{
};

struct Add : NodeFunction<Add, Int, Int, Int>
{
  return_type code() const
  {
    return new Int(*eval_arg<0>() + *eval_arg<1>());
  }
};

template <>
struct node_info_traits<Add>
{
  static node_info get_node_info()
  {
    return node_info("add", {"x", "y"}, {"out"});
  }
};

template <>
struct bind_info_traits<Add, test_backend>
{
  static bind_info get_bind_info()
  {
    return bind_info(
      "add", {"x", "y"}, "out", make_object<InstanceGetterFunction<Add>>(), "");
  }
};

TEST_CASE("add x y", "[node_compiler]")
{
  node_compiler compiler;
  node_graph graph;
  bind_info_manager bim;

  auto add = graph.add(get_node_info<Add>());
  auto i1  = graph.add(get_node_info<node::Int>());
  auto i2  = graph.add(get_node_info<node::Int>());

  REQUIRE(add);
  REQUIRE(i1);
  REQUIRE(i2);

  auto c1 = graph.connect(i1, "value", add, "x");
  auto c2 = graph.connect(i2, "value", add, "y");

  REQUIRE(c1);
  REQUIRE(c2);

  REQUIRE(bim.add(get_bind_info<Add, test_backend>()));
  REQUIRE(bim.add(get_bind_info<node::Int, backend::tags::default_render>()));

  auto exe = compiler.compile({std::move(graph), add}, bim);
}

TEST_CASE("add x x", "[node_compiler]")
{
  node_compiler compiler;
  node_graph graph;
  bind_info_manager bim;

  auto add = graph.add(get_node_info<Add>());
  auto i   = graph.add(get_node_info<node::Int>());

  REQUIRE(add);
  REQUIRE(i);

  auto c1 = graph.connect(i, "value", add, "x");
  auto c2 = graph.connect(i, "value", add, "y");

  REQUIRE(c1);
  REQUIRE(c2);

  REQUIRE(bim.add(get_bind_info<Add, test_backend>()));
  REQUIRE(bim.add(get_bind_info<node::Int, backend::tags::default_render>()));

  auto exe = compiler.compile({std::move(graph), add}, bim);
}

TEST_CASE("add (add x x) x", "[node_compiler]")
{
  node_compiler compiler;
  node_graph graph;
  bind_info_manager bim;

  auto add1 = graph.add(get_node_info<Add>());
  auto add2 = graph.add(get_node_info<Add>());
  auto i   = graph.add(get_node_info<node::Int>());

  REQUIRE(add1);
  REQUIRE(add2);
  REQUIRE(i);

  auto c1 = graph.connect(add2, "out", add1, "x");
  auto c2 = graph.connect(i, "value", add1, "y");
  auto c3 = graph.connect(i, "value", add2, "x");
  auto c4 = graph.connect(i, "value", add2, "y");

  REQUIRE(c1);
  REQUIRE(c2);
  REQUIRE(c3);
  REQUIRE(c4);

  REQUIRE(bim.add(get_bind_info<Add, test_backend>()));
  REQUIRE(bim.add(get_bind_info<node::Int, backend::tags::default_render>()));

  auto exe = compiler.compile({std::move(graph), add1}, bim);
}