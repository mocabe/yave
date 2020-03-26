//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/compiler/node_compiler.hpp>
#include <yave/node/core/function.hpp>
#include <yave/module/std/primitive/primitive.hpp>
#include <yave/module/std/list/list.hpp>
#include <yave/module/std/logic/if.hpp>
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
    return make_object<Int>(*eval_arg<0>() + *eval_arg<1>());
  }
};

struct AddD : NodeFunction<AddD, Double, Double, Double>
{
  return_type code() const
  {
    return make_object<Double>(*eval_arg<0>() + *eval_arg<1>());
  }
};

template <>
struct yave::node_declaration_traits<n::Add>
{
  static auto get_node_declaration()
  {
    class X;
    return node_declaration("add", "", "", {"x", "y"}, {"out"});
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
      0,
      make_object<AddI>(),
      "AddI");

    // Double version
    auto defd = node_definition(
      get_node_declaration<n::Add>().name(),
      0,
      make_object<AddD>(),
      "AddD");

    return {defi, defd};
  }
};

TEST_CASE("add", "[node_compiler]")
{
  managed_node_graph graph;
  node_compiler compiler;
  node_declaration_store decls;
  node_definition_store defs;

  // clang-format off 

  auto int_decl    = get_node_declaration<node::Int>();
  auto bool_decl   = get_node_declaration<node::Bool>();
  auto float_decl  = get_node_declaration<node::Float>();
  auto if_decl     = get_node_declaration<node::If>();
  auto nil_decl    = get_node_declaration<node::ListNil>();
  auto cons_decl   = get_node_declaration<node::ListCons>();
  auto add_decl    = get_node_declaration<n::Add>();

  auto add_defs    = get_node_definitions<n::Add, test_backend>();
  auto int_defs    = get_node_definitions<node::Int, modules::_std::tag>();
  auto bool_defs   = get_node_definitions<node::Bool, modules::_std::tag>();
  auto float_defs  = get_node_definitions<node::String, modules::_std::tag>();
  auto if_defs     = get_node_definitions<node::If, modules::_std::tag>();
  auto nil_defs    = get_node_definitions<node::ListNil, modules::_std::tag>();
  auto cons_defs   = get_node_definitions<node::ListCons, modules::_std::tag>();

  // clang-format on

  REQUIRE(decls.add(int_decl));
  REQUIRE(decls.add(add_decl));
  REQUIRE(decls.add(float_decl));
  REQUIRE(decls.add(bool_decl));
  REQUIRE(decls.add(if_decl));
  REQUIRE(decls.add(nil_decl));
  REQUIRE(decls.add(cons_decl));

  defs.add(add_defs);
  defs.add(int_defs);
  defs.add(float_defs);
  defs.add(bool_defs);
  defs.add(if_defs);
  defs.add(nil_defs);
  defs.add(cons_defs);

  REQUIRE(graph.add_group_output_socket(graph.root_group(), "global_out"));

  auto g   = graph.root_group();
  auto gos = graph.input_sockets(graph.get_group_output(g))[0];

  SECTION("add x y")
  {
    auto add = graph.create(g, add_decl);
    auto i1  = graph.create(g, int_decl);
    auto i2  = graph.create(g, int_decl);

    REQUIRE(add);
    REQUIRE(i1);
    REQUIRE(i2);

    REQUIRE(graph.connect(graph.output_sockets(add)[0], gos));

    REQUIRE(
      graph.connect(graph.output_sockets(i1)[0], graph.input_sockets(add)[0]));
    REQUIRE(
      graph.connect(graph.output_sockets(i2)[0], graph.input_sockets(add)[1]));

    REQUIRE(compiler.compile(std::move(graph), defs));
  }

  SECTION("add x x")
  {
    auto add = graph.create(g, add_decl);
    auto i   = graph.create(g, int_decl);

    REQUIRE(add);
    REQUIRE(i);

    REQUIRE(graph.connect(graph.output_sockets(add)[0], gos));

    REQUIRE(
      graph.connect(graph.output_sockets(i)[0], graph.input_sockets(add)[0]));
    REQUIRE(
      graph.connect(graph.output_sockets(i)[0], graph.input_sockets(add)[1]));

    REQUIRE(compiler.compile(std::move(graph), defs));
  }

  SECTION("add (add x x) x")
  {
    auto add1 = graph.create(g, add_decl);
    auto add2 = graph.create(g, add_decl);
    auto i    = graph.create(g, int_decl);

    REQUIRE(add1);
    REQUIRE(add2);
    REQUIRE(i);

    auto add1_x   = graph.input_sockets(add1)[0];
    auto add1_y   = graph.input_sockets(add1)[1];
    auto add1_out = graph.output_sockets(add1)[0];
    auto add2_x   = graph.input_sockets(add2)[0];
    auto add2_y   = graph.input_sockets(add2)[1];
    auto add2_out = graph.output_sockets(add2)[0];
    auto i_value  = graph.output_sockets(i)[0];

    REQUIRE(graph.connect(add1_out, gos));

    REQUIRE(graph.connect(add2_out, add1_x));
    REQUIRE(graph.connect(i_value, add1_y));
    REQUIRE(graph.connect(i_value, add2_x));
    REQUIRE(graph.connect(i_value, add2_y));

    REQUIRE(compiler.compile(std::move(graph), defs));
  }

  SECTION("add x d")
  {
    auto add = graph.create(g, add_decl);
    auto i   = graph.create(g, int_decl);
    auto d   = graph.create(g, float_decl);

    REQUIRE(add);
    REQUIRE(i);
    REQUIRE(d);

    auto i_value = graph.output_sockets(i)[0];
    auto d_value = graph.output_sockets(d)[0];
    auto add_x   = graph.input_sockets(add)[0];
    auto add_y   = graph.input_sockets(add)[1];
    auto add_out = graph.output_sockets(add)[0];

    REQUIRE(graph.connect(add_out, gos));
    REQUIRE(graph.connect(i_value, add_x));
    REQUIRE(graph.connect(d_value, add_y));

    REQUIRE(!compiler.compile(std::move(graph), defs));
  }

  SECTION("add (add x d) x")
  {
    auto add1 = graph.create(g, add_decl);
    auto add2 = graph.create(g, add_decl);
    auto i    = graph.create(g, int_decl);
    auto d    = graph.create(g, float_decl);

    REQUIRE(add1);
    REQUIRE(add2);
    REQUIRE(i);
    REQUIRE(d);

    auto add1_out = graph.output_sockets(add1)[0];
    auto add2_out = graph.output_sockets(add2)[0];
    auto i_value  = graph.output_sockets(i)[0];
    auto d_value  = graph.output_sockets(d)[0];
    auto add1_x   = graph.input_sockets(add1)[0];
    auto add1_y   = graph.input_sockets(add1)[1];
    auto add2_x   = graph.input_sockets(add2)[0];
    auto add2_y   = graph.input_sockets(add2)[1];

    REQUIRE(graph.connect(add1_out, gos));
    REQUIRE(graph.connect(add2_out, add1_x));
    REQUIRE(graph.connect(i_value, add1_y));
    REQUIRE(graph.connect(i_value, add2_x));
    REQUIRE(graph.connect(d_value, add2_y));

    REQUIRE(!compiler.compile(std::move(graph), defs));
  }

  SECTION("if b x y")
  {
    auto _if = graph.create(g, if_decl);
    auto i   = graph.create(g, int_decl);
    auto b   = graph.create(g, bool_decl);

    REQUIRE(_if);
    REQUIRE(i);
    REQUIRE(b);

    auto b_value  = graph.output_sockets(b)[0];
    auto i_value  = graph.output_sockets(i)[0];
    auto _if_cond = graph.input_sockets(_if)[0];
    auto _if_then = graph.input_sockets(_if)[1];
    auto _if_else = graph.input_sockets(_if)[2];
    auto _if_out  = graph.output_sockets(_if)[0];

    REQUIRE(graph.connect(_if_out, gos));
    REQUIRE(graph.connect(b_value, _if_cond));
    REQUIRE(graph.connect(i_value, _if_then));
    REQUIRE(graph.connect(i_value, _if_else));

    REQUIRE(compiler.compile(std::move(graph), defs));
  }

  SECTION("if b (if b x y) z")
  {
    auto if1 = graph.create(g, if_decl);
    auto if2 = graph.create(g, if_decl);
    auto i   = graph.create(g, int_decl);
    auto b   = graph.create(g, bool_decl);

    REQUIRE(if1);
    REQUIRE(if2);
    REQUIRE(i);
    REQUIRE(b);

    auto b_value = graph.output_sockets(b)[0];
    auto i_value = graph.output_sockets(i)[0];

    auto if1_cond = graph.input_sockets(if1)[0];
    auto if1_then = graph.input_sockets(if1)[1];
    auto if1_else = graph.input_sockets(if1)[2];
    auto if1_out  = graph.output_sockets(if1)[0];

    auto if2_cond = graph.input_sockets(if2)[0];
    auto if2_then = graph.input_sockets(if2)[1];
    auto if2_else = graph.input_sockets(if2)[2];
    auto if2_out  = graph.output_sockets(if2)[0];

    REQUIRE(graph.connect(if1_out, gos));
    REQUIRE(graph.connect(b_value, if1_cond));
    REQUIRE(graph.connect(if2_out, if1_then));
    REQUIRE(graph.connect(b_value, if2_cond));
    REQUIRE(graph.connect(i_value, if2_then));
    REQUIRE(graph.connect(i_value, if2_else));
    REQUIRE(graph.connect(i_value, if1_else));

    REQUIRE(compiler.compile(std::move(graph), defs));
  }

  SECTION("if b x d")
  {
    auto _if = graph.create(g, if_decl);
    auto i   = graph.create(g, int_decl);
    auto d   = graph.create(g, float_decl);
    auto b   = graph.create(g, bool_decl);

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
    auto _if_out  = graph.output_sockets(_if)[0];

    REQUIRE(graph.connect(_if_out, gos));
    REQUIRE(graph.connect(b_value, _if_cond));
    REQUIRE(graph.connect(i_value, _if_then));
    REQUIRE(graph.connect(d_value, _if_else));

    REQUIRE(!compiler.compile(std::move(graph), defs));
}

  SECTION("add<int> x y")
  {
    auto add = graph.create(g, add_decl);
    auto i   = graph.create(g, int_decl);

    REQUIRE(add);
    REQUIRE(i);

    auto i_value = graph.output_sockets(i)[0];
    auto add_x   = graph.input_sockets(add)[0];
    auto add_y   = graph.input_sockets(add)[1];
    auto add_out = graph.output_sockets(add)[0];

    REQUIRE(graph.connect(add_out, gos));
    REQUIRE(graph.connect(i_value, add_x));
    REQUIRE(graph.connect(i_value, add_y));

    REQUIRE(compiler.compile(std::move(graph), defs));
  }

  SECTION("add<double> x y")
  {
    auto add = graph.create(g, add_decl);
    auto d   = graph.create(g, float_decl);

    REQUIRE(add);
    REQUIRE(d);

    auto d_value = graph.output_sockets(d)[0];
    auto add_x   = graph.input_sockets(add)[0];
    auto add_y   = graph.input_sockets(add)[1];
    auto add_out = graph.output_sockets(add)[0];

    REQUIRE(graph.connect(add_out, gos));
    REQUIRE(graph.connect(d_value, add_x));
    REQUIRE(graph.connect(d_value, add_y));

    REQUIRE(compiler.compile(std::move(graph), defs));
  }

  SECTION("add<?> x y")
  {
    auto add = graph.create(g, add_decl);
    auto i   = graph.create(g, int_decl);
    auto d   = graph.create(g, float_decl);

    REQUIRE(add);
    REQUIRE(i);
    REQUIRE(d);

    auto d_value = graph.output_sockets(d)[0];
    auto i_value = graph.output_sockets(i)[0];
    auto add_x   = graph.input_sockets(add)[0];
    auto add_y   = graph.input_sockets(add)[1];
    auto add_out = graph.output_sockets(add)[0];

    REQUIRE(graph.connect(add_out, gos));
    REQUIRE(graph.connect(i_value, add_x));
    REQUIRE(graph.connect(d_value, add_y));

    REQUIRE(!compiler.compile(std::move(graph), defs));
  }

  SECTION("42 : []")
  {
    auto i    = graph.create(g, int_decl);
    auto nil  = graph.create(g, nil_decl);
    auto cons = graph.create(g, cons_decl);

    REQUIRE(i);
    REQUIRE(nil);
    REQUIRE(cons);

    auto i_value   = graph.output_sockets(i)[0];
    auto nil_value = graph.output_sockets(nil)[0];
    auto cons_head = graph.input_sockets(cons)[0];
    auto cons_tail = graph.input_sockets(cons)[1];
    auto cons_out  = graph.output_sockets(cons)[0];

    REQUIRE(graph.connect(cons_out, gos));
    REQUIRE(graph.connect(i_value, cons_head));
    REQUIRE(graph.connect(nil_value, cons_tail));

    REQUIRE(compiler.compile(std::move(graph), defs));
  }
}