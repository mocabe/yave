//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/compiler/node_compiler.hpp>
#include <yave/node/scene/scene_graph.hpp>
#include <yave/node/core/function.hpp>
#include <yave/backend/default/common/primitive_constructor.hpp>
#include <yave/backend/default/common/control_flow.hpp>
#include <yave/backend/default/common/list.hpp>
#include <catch2/catch.hpp>

using namespace yave;

// backend tag
class test_backend
{
};

namespace n {

  // node tag
  class Add
  {
  };

} // namespace n

struct AddI : NodeFunction<AddI, Int, Int, Int>
{
  return_type code() const
  {
    return new Int(*eval_arg<0>() + *eval_arg<1>());
  }
};

struct AddD : NodeFunction<AddD, Double, Double, Double>
{
  return_type code() const
  {
    return new Double(*eval_arg<0>() + *eval_arg<1>());
  }
};

template <>
struct yave::node_declaration_traits<n::Add>
{
  static auto get_node_declaration()
  {
    class X;
    return node_declaration(
      "add",
      {"x", "y"},
      {"out"},
      {object_type<node_closure<X, X, X>>()},
      node_type::normal);
  }
};

template <>
struct yave::node_definition_traits<n::Add, test_backend>
{
  static auto get_node_definitions() -> std::vector<node_definition>
  {
    // Int version
    auto defi = node_definition(
      get_node_declaration<n::Add>().name(),
      get_node_declaration<n::Add>().output_sockets()[0],
      make_object<InstanceGetterFunction<AddI>>(),
      "AddI");

    // Double version
    auto defd = node_definition(
      get_node_declaration<n::Add>().name(),
      get_node_declaration<n::Add>().output_sockets()[0],
      make_object<InstanceGetterFunction<AddD>>(),
      "AddD");

    return {defi, defd};
  }
};

auto to_node_info(const node_declaration& decl)
{
  return node_info(
    decl.name(), decl.input_sockets(), decl.output_sockets(), decl.node_type());
};

TEST_CASE("add", "[node_compiler]")
{
  node_compiler compiler;
  node_graph graph;
  node_declaration_store decls;
  node_definition_store defs;

  auto double_decl = get_node_declaration<node::Double>();
  auto int_decl    = get_node_declaration<node::Int>();
  auto bool_decl   = get_node_declaration<node::Bool>();
  auto if_decl     = get_node_declaration<node::If>();
  auto nil_decl    = get_node_declaration<node::ListNil>();
  auto cons_decl   = get_node_declaration<node::ListCons>();
  auto add_decl    = get_node_declaration<n::Add>();

  auto add_defs = get_node_definitions<n::Add, test_backend>();
  auto int_defs =
    get_node_definitions<node::Int, backend_tags::default_common>();
  auto double_defs =
    get_node_definitions<node::Double, backend_tags::default_common>();
  auto bool_defs =
    get_node_definitions<node::Bool, backend_tags::default_common>();
  auto if_defs = get_node_definitions<node::If, backend_tags::default_common>();
  auto nil_defs =
    get_node_definitions<node::ListNil, backend_tags::default_common>();
  auto cons_defs =
    get_node_definitions<node::ListCons, backend_tags::default_common>();

  REQUIRE(decls.add(int_decl));
  REQUIRE(decls.add(add_decl));
  REQUIRE(decls.add(double_decl));
  REQUIRE(decls.add(bool_decl));
  REQUIRE(decls.add(if_decl));
  REQUIRE(decls.add(nil_decl));
  REQUIRE(decls.add(cons_decl));

  defs.add(add_defs);
  defs.add(int_defs);
  defs.add(double_defs);
  defs.add(bool_defs);
  defs.add(if_defs);
  defs.add(nil_defs);
  defs.add(cons_defs);

  auto add_info    = to_node_info(add_decl);
  auto int_info    = to_node_info(int_decl);
  auto double_info = to_node_info(double_decl);
  auto bool_info   = to_node_info(bool_decl);
  auto if_info     = to_node_info(if_decl);
  auto nil_info    = to_node_info(nil_decl);
  auto cons_info   = to_node_info(cons_decl);

  SECTION("add x y")
  {
    auto add = graph.add(add_info);
    auto i1  = graph.add(int_info);
    auto i2  = graph.add(int_info);

    REQUIRE(add);
    REQUIRE(i1);
    REQUIRE(i2);

    auto c1 =
      graph.connect(graph.output_sockets(i1)[0], graph.input_sockets(add)[0]);
    auto c2 =
      graph.connect(graph.output_sockets(i2)[0], graph.input_sockets(add)[1]);

    REQUIRE(c1);
    REQUIRE(c2);

    REQUIRE(compiler.compile({std::move(graph), add}, decls, defs));
  }

  SECTION("add x x")
  {
    auto add = graph.add(add_info);
    auto i   = graph.add(int_info);

    REQUIRE(add);
    REQUIRE(i);

    auto c1 =
      graph.connect(graph.output_sockets(i)[0], graph.input_sockets(add)[0]);
    auto c2 =
      graph.connect(graph.output_sockets(i)[0], graph.input_sockets(add)[1]);

    REQUIRE(c1);
    REQUIRE(c2);

    REQUIRE(compiler.compile({std::move(graph), add}, decls, defs));
  }

  SECTION("add (add x x) x")
  {
    auto add1 = graph.add(add_info);
    auto add2 = graph.add(add_info);
    auto i    = graph.add(int_info);

    REQUIRE(add1);
    REQUIRE(add2);
    REQUIRE(i);

    auto add1_x   = graph.input_sockets(add1)[0];
    auto add1_y   = graph.input_sockets(add1)[1];
    auto add2_x   = graph.input_sockets(add2)[0];
    auto add2_y   = graph.input_sockets(add2)[1];
    auto add2_out = graph.output_sockets(add2)[0];
    auto i_value  = graph.output_sockets(i)[0];

    auto c1 = graph.connect(add2_out, add1_x);
    auto c2 = graph.connect(i_value, add1_y);
    auto c3 = graph.connect(i_value, add2_x);
    auto c4 = graph.connect(i_value, add2_y);

    REQUIRE(c1);
    REQUIRE(c2);
    REQUIRE(c3);
    REQUIRE(c4);

    REQUIRE(compiler.compile({std::move(graph), add1}, decls, defs));
  }

  SECTION("add x d")
  {
    auto add = graph.add(add_info);
    auto i   = graph.add(int_info);
    auto d   = graph.add(double_info);

    REQUIRE(add);
    REQUIRE(i);
    REQUIRE(d);

    auto i_value = graph.output_sockets(i)[0];
    auto d_value = graph.output_sockets(d)[0];
    auto add_x   = graph.input_sockets(add)[0];
    auto add_y   = graph.input_sockets(add)[1];

    auto c1 = graph.connect(i_value, add_x);
    auto c2 = graph.connect(d_value, add_y);

    REQUIRE(c1);
    REQUIRE(c2);

    REQUIRE(!compiler.compile({std::move(graph), add}, decls, defs));
  }

  SECTION("add (add x d) x")
  {
    auto add1 = graph.add(add_info);
    auto add2 = graph.add(add_info);
    auto i    = graph.add(int_info);
    auto d    = graph.add(double_info);

    REQUIRE(add1);
    REQUIRE(add2);
    REQUIRE(i);
    REQUIRE(d);

    auto add2_out = graph.output_sockets(add2)[0];
    auto i_value  = graph.output_sockets(i)[0];
    auto d_value  = graph.output_sockets(d)[0];
    auto add1_x   = graph.input_sockets(add1)[0];
    auto add1_y   = graph.input_sockets(add1)[1];
    auto add2_x   = graph.input_sockets(add2)[0];
    auto add2_y   = graph.input_sockets(add2)[1];

    auto c1 = graph.connect(add2_out, add1_x);
    auto c2 = graph.connect(i_value, add1_y);
    auto c3 = graph.connect(i_value, add2_x);
    auto c4 = graph.connect(d_value, add2_y);

    REQUIRE(c1);
    REQUIRE(c2);
    REQUIRE(c3);
    REQUIRE(c4);

    REQUIRE(!compiler.compile({std::move(graph), add1}, decls, defs));
  }

  SECTION("if b x y")
  {
    auto _if = graph.add(if_info);
    auto i   = graph.add(int_info);
    auto b   = graph.add(bool_info);

    REQUIRE(_if);
    REQUIRE(i);
    REQUIRE(b);

    auto b_value  = graph.output_sockets(b)[0];
    auto i_value  = graph.output_sockets(i)[0];
    auto _if_cond = graph.input_sockets(_if)[0];
    auto _if_then = graph.input_sockets(_if)[1];
    auto _if_else = graph.input_sockets(_if)[2];

    auto c1 = graph.connect(b_value, _if_cond);
    auto c2 = graph.connect(i_value, _if_then);
    auto c3 = graph.connect(i_value, _if_else);

    REQUIRE(c1);
    REQUIRE(c2);
    REQUIRE(c3);

    REQUIRE(compiler.compile({std::move(graph), _if}, decls, defs));
  }

  SECTION("if b (if b x y) z")
  {
    auto if1 = graph.add(if_info);
    auto if2 = graph.add(if_info);
    auto i   = graph.add(int_info);
    auto b   = graph.add(bool_info);

    REQUIRE(if1);
    REQUIRE(if2);
    REQUIRE(i);
    REQUIRE(b);

    auto b_value = graph.output_sockets(b)[0];
    auto i_value = graph.output_sockets(i)[0];

    auto if1_cond = graph.input_sockets(if1)[0];
    auto if1_then = graph.input_sockets(if1)[1];
    auto if1_else = graph.input_sockets(if1)[2];

    auto if2_cond = graph.input_sockets(if2)[0];
    auto if2_then = graph.input_sockets(if2)[1];
    auto if2_else = graph.input_sockets(if2)[2];
    auto if2_out  = graph.output_sockets(if2)[0];

    auto c1 = graph.connect(b_value, if1_cond);
    auto c2 = graph.connect(if2_out, if1_then);
    auto c3 = graph.connect(b_value, if2_cond);
    auto c4 = graph.connect(i_value, if2_then);
    auto c5 = graph.connect(i_value, if2_else);
    auto c6 = graph.connect(i_value, if1_else);

    REQUIRE(c1);
    REQUIRE(c2);
    REQUIRE(c3);
    REQUIRE(c4);
    REQUIRE(c5);
    REQUIRE(c6);

    REQUIRE(compiler.compile({std::move(graph), if1}, decls, defs));
  }

  SECTION("if b x d")
  {
    auto _if = graph.add(if_info);
    auto i   = graph.add(int_info);
    auto d   = graph.add(double_info);
    auto b   = graph.add(bool_info);

    REQUIRE(_if);
    REQUIRE(i);
    REQUIRE(d);
    REQUIRE(b);

    auto i_value  = graph.output_sockets(i)[0];
    auto d_value  = graph.output_sockets(d)[0];
    auto b_value  = graph.output_sockets(b)[0];
    auto _if_cond = graph.input_sockets(_if)[0];
    auto _if_then = graph.input_sockets(_if)[1];
    auto _if_else = graph.input_sockets(_if)[2];

    auto c1 = graph.connect(b_value, _if_cond);
    auto c2 = graph.connect(i_value, _if_then);
    auto c3 = graph.connect(d_value, _if_else);

    REQUIRE(c1);
    REQUIRE(c2);
    REQUIRE(c3);

    REQUIRE(!compiler.compile({std::move(graph), _if}, decls, defs));
  }

  SECTION("add<int> x y")
  {
    auto add = graph.add(add_info);
    auto i   = graph.add(int_info);

    REQUIRE(add);
    REQUIRE(i);

    auto i_value = graph.output_sockets(i)[0];
    auto add_x   = graph.input_sockets(add)[0];
    auto add_y   = graph.input_sockets(add)[1];

    auto c1 = graph.connect(i_value, add_x);
    auto c2 = graph.connect(i_value, add_y);

    REQUIRE(c1);
    REQUIRE(c2);

    REQUIRE(compiler.compile({std::move(graph), add}, decls, defs));
  }

  SECTION("add<double> x y")
  {
    auto add = graph.add(add_info);
    auto d   = graph.add(double_info);

    REQUIRE(add);
    REQUIRE(d);

    auto d_value = graph.output_sockets(d)[0];
    auto add_x   = graph.input_sockets(add)[0];
    auto add_y   = graph.input_sockets(add)[1];

    auto c1 = graph.connect(d_value, add_x);
    auto c2 = graph.connect(d_value, add_y);

    REQUIRE(c1);
    REQUIRE(c2);

    REQUIRE(compiler.compile({std::move(graph), add}, decls, defs));
  }

  SECTION("add<?> x y")
  {
    auto add = graph.add(add_info);
    auto i   = graph.add(int_info);
    auto d   = graph.add(double_info);

    REQUIRE(add);
    REQUIRE(i);
    REQUIRE(d);

    auto d_value = graph.output_sockets(d)[0];
    auto i_value = graph.output_sockets(i)[0];
    auto add_x   = graph.input_sockets(add)[0];
    auto add_y   = graph.input_sockets(add)[1];

    auto c1 = graph.connect(i_value, add_x);
    auto c2 = graph.connect(d_value, add_y);

    REQUIRE(c1);
    REQUIRE(c2);

    REQUIRE(!compiler.compile({std::move(graph), add}, decls, defs));
  }

  SECTION("42 : []")
  {
    auto i    = graph.add(int_info);
    auto nil  = graph.add(nil_info);
    auto cons = graph.add(cons_info);

    REQUIRE(i);
    REQUIRE(nil);
    REQUIRE(cons);

    auto i_value   = graph.output_sockets(i)[0];
    auto nil_value = graph.output_sockets(nil)[0];
    auto cons_head = graph.input_sockets(cons)[0];
    auto cons_tail = graph.input_sockets(cons)[1];

    auto c1 = graph.connect(i_value, cons_head);
    auto c2 = graph.connect(nil_value, cons_tail);

    REQUIRE(c1);
    REQUIRE(c2);

    REQUIRE(compiler.compile({std::move(graph), cons}, decls, defs));
  }
}