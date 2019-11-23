//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/backend/default/render/list.hpp>
#include <yave/backend/default/render/primitive.hpp>
#include <catch2/catch.hpp>

#include <iostream>

using namespace yave;
using namespace yave::backend::default_render;

TEST_CASE("ListNil")
{
  auto nil = make_object<ListNil>();

  SECTION("init")
  {
    REQUIRE_NOTHROW(type_of(nil));
    REQUIRE_NOTHROW(eval(nil));
  }

  SECTION("bind")
  {
    auto defs =
      get_node_definitions<node::ListNil, backend::tags::default_render>();
    REQUIRE(same_type(get_type(nil), get_type(defs[0].get_instance(nullptr))));
  }
}

TEST_CASE("ListCons")
{
  auto cons = make_object<ListCons>();

  SECTION("init")
  {
    REQUIRE_NOTHROW(type_of(cons));
    REQUIRE_NOTHROW(eval(cons));
  }

  SECTION("bind")
  {
    auto defs =
      get_node_definitions<node::ListCons, backend::tags::default_render>();
    REQUIRE(same_type(get_type(cons), get_type(defs[0].get_instance(nullptr))));
  }
}

TEST_CASE("ListDecompose_Head")
{
  auto head = make_object<ListDecompose_Head>();

  SECTION("init")
  {
    REQUIRE_NOTHROW(type_of(head));
    REQUIRE_NOTHROW(eval(head));
  }

  SECTION("bind")
  {
    auto defs = get_node_definitions<
      node::ListDecompose_Head,
      backend::tags::default_render>();
    REQUIRE(same_type(get_type(head), get_type(defs[0].get_instance(nullptr))));
  }
}

TEST_CASE("ListDecompose_Tail")
{
  auto tail = make_object<ListDecompose_Tail>();

  SECTION("init")
  {
    REQUIRE_NOTHROW(type_of(tail));
    REQUIRE_NOTHROW(eval(tail));
  }

  SECTION("bind")
  {
    auto defs = get_node_definitions<
      node::ListDecompose_Tail,
      backend::tags::default_render>();
    REQUIRE(same_type(get_type(tail), get_type(defs[0].get_instance(nullptr))));
  }
}

TEST_CASE("List")
{
  auto nil  = make_object<ListNil>();
  auto cons = make_object<ListCons>();
  auto head = make_object<ListDecompose_Head>();
  auto tail = make_object<ListDecompose_Tail>();

  auto i42 = make_object<PrimitiveConstructor<Int>>(
    make_object<PrimitiveContainer>((int)42));
  auto i24 = make_object<PrimitiveConstructor<Int>>(
    make_object<PrimitiveContainer>((int)24));

  auto frame = make_object<FrameTime>();

  SECTION("42 : []")
  {
    auto tree = cons << i42 << nil;
    auto exe  = tree << frame;

    check_type_static<List<Int>>(exe);
    check_type_dynamic<List<Int>>(exe);

    auto result = eval(exe);
    REQUIRE(*eval(result->head()) == 42);
    REQUIRE(eval(result->tail())->is_nil());
  }

  SECTION("42 : 24 : []")
  {
    auto tree = cons << i42 << (cons << i24 << nil);
    auto exe  = tree << frame;

    check_type_static<List<Int>>(exe);
    check_type_dynamic<List<Int>>(exe);

    auto result = eval(exe);

    REQUIRE(*eval(result->head()) == 42);
    REQUIRE(*eval(eval(result->tail())->head()) == 24);
    REQUIRE(eval(eval(result->tail())->tail())->is_nil());
  }
}