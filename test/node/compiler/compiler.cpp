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

struct AddF : NodeFunction<AddF, Float, Float, Float>
{
  return_type code() const
  {
    return make_object<Float>(*eval_arg<0>() + *eval_arg<1>());
  }
};

template <>
struct yave::node_declaration_traits<n::Add>
{
  static auto get_node_declaration()
  {
    class X;
    return node_declaration("Add", "/test", "", {"x", "y"}, {"out"});
  }
};

template <>
struct yave::node_definition_traits<n::Add, test_backend>
{
  static auto get_node_definitions() -> std::vector<node_definition>
  {
    // Int version
    auto defi = node_definition(
      get_node_declaration<n::Add>().qualified_name(),
      0,
      make_object<AddI>(),
      "AddI");

    // Float version
    auto defd = node_definition(
      get_node_declaration<n::Add>().qualified_name(),
      0,
      make_object<AddF>(),
      "AddF");

    return {defi, defd};
  }
};

TEST_CASE("node_compiler V2")
{
  structured_node_graph ng;
  node_parser parser;
  node_compiler compiler;
  node_definition_store defs;

  // clang-format off

  auto int_decl   = get_node_declaration<node::Int>();
  auto bool_decl  = get_node_declaration<node::Bool>();
  auto float_decl = get_node_declaration<node::Float>();
  auto if_decl    = get_node_declaration<node::If>();
  auto nil_decl   = get_node_declaration<node::ListNil>();
  auto cons_decl  = get_node_declaration<node::ListCons>();
  auto add_decl   = get_node_declaration<n::Add>();

  auto add_defs   = get_node_definitions<n::Add, test_backend>();
  auto int_defs   = get_node_definitions<node::Int, modules::_std::tag>();
  auto bool_defs  = get_node_definitions<node::Bool, modules::_std::tag>();
  auto float_defs = get_node_definitions<node::Float, modules::_std::tag>();
  auto if_defs    = get_node_definitions<node::If, modules::_std::tag>();
  auto nil_defs   = get_node_definitions<node::ListNil, modules::_std::tag>();
  auto cons_defs  = get_node_definitions<node::ListCons, modules::_std::tag>();

  // clang-format on

  defs.add(add_defs);
  defs.add(int_defs);
  defs.add(float_defs);
  defs.add(bool_defs);
  defs.add(if_defs);
  defs.add(nil_defs);
  defs.add(cons_defs);

  auto int_func   = ng.create_function(int_decl);
  auto add_func   = ng.create_function(add_decl);
  auto float_func = ng.create_function(float_decl);
  auto bool_func  = ng.create_function(bool_decl);
  auto if_func    = ng.create_function(if_decl);
  auto nil_func   = ng.create_function(nil_decl);
  auto cons_func  = ng.create_function(cons_decl);

  REQUIRE(int_func);
  REQUIRE(add_func);
  REQUIRE(float_func);
  REQUIRE(bool_func);
  REQUIRE(if_func);
  REQUIRE(nil_func);
  REQUIRE(cons_func);

  auto root = ng.create_group({nullptr}, {});
  auto out  = ng.add_output_socket(root, "out");
  ng.set_name(root, "root");

  auto compile_ng = [&] {
    return compiler.compile(
      parser
        .parse(
          {.node_graph    = std::move(ng),
           .output_socket = out,
           .current_group = {}})
        .take_node_graph()
        .value(),
      defs);
  };

  auto os = ng.input_sockets(ng.get_group_output(root))[0];

  SECTION("int")
  {
    auto n = ng.create_copy(root, int_func);
    REQUIRE(ng.connect(ng.output_sockets(n)[0], os));
    REQUIRE(compile_ng());
  }

  SECTION("int float")
  {
    auto i = ng.create_copy(root, int_func);
    auto f = ng.create_copy(root, float_func);
    REQUIRE(i);
    REQUIRE(f);
    REQUIRE(ng.connect(ng.output_sockets(i)[0], os));
    REQUIRE(ng.connect(ng.output_sockets(f)[0], ng.input_sockets(i)[0]));
    // missmatch
    REQUIRE(!compile_ng());
  }

  SECTION("add")
  {
    auto add = ng.create_copy(root, add_func);
    REQUIRE(add);
    REQUIRE(ng.connect(ng.output_sockets(add)[0], os));
    // ambiguous
    REQUIRE(!compile_ng());
  }

  SECTION("add int int")
  {
    auto add = ng.create_copy(root, add_func);
    auto i1  = ng.create_copy(root, int_func);
    auto i2  = ng.create_copy(root, int_func);

    REQUIRE(add);
    REQUIRE(i1);
    REQUIRE(i2);

    REQUIRE(ng.connect(ng.output_sockets(add)[0], os));
    REQUIRE(ng.connect(ng.output_sockets(i1)[0], ng.input_sockets(add)[0]));
    REQUIRE(ng.connect(ng.output_sockets(i2)[0], ng.input_sockets(add)[1]));
    REQUIRE(compile_ng());
  }

  SECTION("add float float")
  {
    auto add = ng.create_copy(root, add_func);
    auto f1  = ng.create_copy(root, float_func);
    auto f2  = ng.create_copy(root, float_func);

    REQUIRE(add);
    REQUIRE(f1);
    REQUIRE(f2);

    REQUIRE(ng.connect(ng.output_sockets(add)[0], os));
    REQUIRE(ng.connect(ng.output_sockets(f1)[0], ng.input_sockets(add)[0]));
    REQUIRE(ng.connect(ng.output_sockets(f2)[0], ng.input_sockets(add)[1]));
    REQUIRE(compile_ng());
  }

  SECTION("add int float")
  {
    auto add = ng.create_copy(root, add_func);
    auto i   = ng.create_copy(root, int_func);
    auto f   = ng.create_copy(root, float_func);

    REQUIRE(add);
    REQUIRE(i);
    REQUIRE(f);

    REQUIRE(ng.connect(ng.output_sockets(add)[0], os));
    REQUIRE(ng.connect(ng.output_sockets(i)[0], ng.input_sockets(add)[0]));
    REQUIRE(ng.connect(ng.output_sockets(f)[0], ng.input_sockets(add)[1]));
    // missmatch
    REQUIRE(!compile_ng());
  }

  SECTION("add (add int int) int")
  {
    auto add1 = ng.create_copy(root, add_func);
    auto add2 = ng.create_copy(root, add_func);
    auto i    = ng.create_copy(root, int_func);

    REQUIRE(add1);
    REQUIRE(add2);
    REQUIRE(i);

    auto add1_x   = ng.input_sockets(add1)[0];
    auto add1_y   = ng.input_sockets(add1)[1];
    auto add1_out = ng.output_sockets(add1)[0];
    auto add2_x   = ng.input_sockets(add2)[0];
    auto add2_y   = ng.input_sockets(add2)[1];
    auto add2_out = ng.output_sockets(add2)[0];
    auto i_value  = ng.output_sockets(i)[0];

    REQUIRE(ng.connect(add1_out, os));

    REQUIRE(ng.connect(add2_out, add1_x));
    REQUIRE(ng.connect(i_value, add1_y));
    REQUIRE(ng.connect(i_value, add2_x));
    REQUIRE(ng.connect(i_value, add2_y));

    REQUIRE(compile_ng());
  }

  SECTION("add (add int float) int")
  {
    auto add1 = ng.create_copy(root, add_func);
    auto add2 = ng.create_copy(root, add_func);
    auto i    = ng.create_copy(root, int_func);
    auto d    = ng.create_copy(root, float_func);

    REQUIRE(add1);
    REQUIRE(add2);
    REQUIRE(i);
    REQUIRE(d);

    auto add1_out = ng.output_sockets(add1)[0];
    auto add2_out = ng.output_sockets(add2)[0];
    auto i_value  = ng.output_sockets(i)[0];
    auto d_value  = ng.output_sockets(d)[0];
    auto add1_x   = ng.input_sockets(add1)[0];
    auto add1_y   = ng.input_sockets(add1)[1];
    auto add2_x   = ng.input_sockets(add2)[0];
    auto add2_y   = ng.input_sockets(add2)[1];

    REQUIRE(ng.connect(add1_out, os));
    REQUIRE(ng.connect(add2_out, add1_x));
    REQUIRE(ng.connect(i_value, add1_y));
    REQUIRE(ng.connect(i_value, add2_x));
    REQUIRE(ng.connect(d_value, add2_y));

    REQUIRE(!compile_ng());
  }

  SECTION("if bool int int")
  {
    auto _if = ng.create_copy(root, if_func);
    auto i   = ng.create_copy(root, int_func);
    auto b   = ng.create_copy(root, bool_func);

    REQUIRE(_if);
    REQUIRE(i);
    REQUIRE(b);

    auto b_value  = ng.output_sockets(b)[0];
    auto i_value  = ng.output_sockets(i)[0];
    auto _if_cond = ng.input_sockets(_if)[0];
    auto _if_then = ng.input_sockets(_if)[1];
    auto _if_else = ng.input_sockets(_if)[2];
    auto _if_out  = ng.output_sockets(_if)[0];

    REQUIRE(ng.connect(_if_out, os));
    REQUIRE(ng.connect(b_value, _if_cond));
    REQUIRE(ng.connect(i_value, _if_then));
    REQUIRE(ng.connect(i_value, _if_else));

    REQUIRE(compile_ng());
  }

  SECTION("if bool (if bool int int) int")
  {
    auto if1 = ng.create_copy(root, if_func);
    auto if2 = ng.create_copy(root, if_func);
    auto i   = ng.create_copy(root, int_func);
    auto b   = ng.create_copy(root, bool_func);

    REQUIRE(if1);
    REQUIRE(if2);
    REQUIRE(i);
    REQUIRE(b);

    auto b_value = ng.output_sockets(b)[0];
    auto i_value = ng.output_sockets(i)[0];

    auto if1_cond = ng.input_sockets(if1)[0];
    auto if1_then = ng.input_sockets(if1)[1];
    auto if1_else = ng.input_sockets(if1)[2];
    auto if1_out  = ng.output_sockets(if1)[0];

    auto if2_cond = ng.input_sockets(if2)[0];
    auto if2_then = ng.input_sockets(if2)[1];
    auto if2_else = ng.input_sockets(if2)[2];
    auto if2_out  = ng.output_sockets(if2)[0];

    REQUIRE(ng.connect(if1_out, os));
    REQUIRE(ng.connect(b_value, if1_cond));
    REQUIRE(ng.connect(if2_out, if1_then));
    REQUIRE(ng.connect(b_value, if2_cond));
    REQUIRE(ng.connect(i_value, if2_then));
    REQUIRE(ng.connect(i_value, if2_else));
    REQUIRE(ng.connect(i_value, if1_else));

    REQUIRE(compile_ng());
  }

  SECTION("if bool int float")
  {
    auto _if = ng.create_copy(root, if_func);
    auto i   = ng.create_copy(root, int_func);
    auto d   = ng.create_copy(root, float_func);
    auto b   = ng.create_copy(root, bool_func);

    REQUIRE(_if);
    REQUIRE(i);
    REQUIRE(d);
    REQUIRE(b);

    auto i_value  = ng.output_sockets(i)[0];
    auto d_value  = ng.output_sockets(d)[0];
    auto b_value  = ng.output_sockets(b)[0];
    auto _if_cond = ng.input_sockets(_if)[0];
    auto _if_then = ng.input_sockets(_if)[1];
    auto _if_else = ng.input_sockets(_if)[2];
    auto _if_out  = ng.output_sockets(_if)[0];

    REQUIRE(ng.connect(_if_out, os));
    REQUIRE(ng.connect(b_value, _if_cond));
    REQUIRE(ng.connect(i_value, _if_then));
    REQUIRE(ng.connect(d_value, _if_else));

    REQUIRE(!compile_ng());
  }

  SECTION("42 : []")
  {
    auto i    = ng.create_copy(root, int_func);
    auto nil  = ng.create_copy(root, nil_func);
    auto cons = ng.create_copy(root, cons_func);

    REQUIRE(i);
    REQUIRE(nil);
    REQUIRE(cons);

    auto i_value   = ng.output_sockets(i)[0];
    auto nil_value = ng.output_sockets(nil)[0];
    auto cons_head = ng.input_sockets(cons)[0];
    auto cons_tail = ng.input_sockets(cons)[1];
    auto cons_out  = ng.output_sockets(cons)[0];

    REQUIRE(ng.connect(cons_out, os));
    REQUIRE(ng.connect(i_value, cons_head));
    REQUIRE(ng.connect(nil_value, cons_tail));

    REQUIRE(compile_ng());
  }

  SECTION("f = [x y -> x + y]")
  {
    auto f = ng.create_group(root, {});
    ng.set_name(f, "(x y -> x + y)");
    auto a = ng.create_copy(f, add_func);
    REQUIRE(ng.add_output_socket(f, "out"));
    REQUIRE(ng.add_input_socket(f, "x"));
    REQUIRE(ng.add_input_socket(f, "y"));

    REQUIRE(ng.connect(
      ng.output_sockets(a)[0], ng.input_sockets(ng.get_group_output(f))[0]));
    REQUIRE(ng.connect(
      ng.output_sockets(ng.get_group_input(f))[0], ng.input_sockets(a)[0]));
    REQUIRE(ng.connect(
      ng.output_sockets(ng.get_group_input(f))[1], ng.input_sockets(a)[1]));

    REQUIRE(ng.connect(ng.output_sockets(f)[0], os));

    SECTION("f")
    {
      REQUIRE(!compile_ng());
    }

    SECTION("f int int")
    {
      auto i = ng.create_copy(root, int_func);
      REQUIRE(ng.connect(ng.output_sockets(i)[0], ng.input_sockets(f)[0]));
      REQUIRE(ng.connect(ng.output_sockets(i)[0], ng.input_sockets(f)[1]));
      REQUIRE(compile_ng());
    }

    SECTION("f int int")
    {
      auto i = ng.create_copy(root, float_func);
      REQUIRE(ng.connect(ng.output_sockets(i)[0], ng.input_sockets(f)[0]));
      REQUIRE(ng.connect(ng.output_sockets(i)[0], ng.input_sockets(f)[1]));
      REQUIRE(compile_ng());
    }

    SECTION("f int float")
    {
      auto i = ng.create_copy(root, int_func);
      auto j = ng.create_copy(root, float_func);
      REQUIRE(ng.connect(ng.output_sockets(i)[0], ng.input_sockets(f)[0]));
      REQUIRE(ng.connect(ng.output_sockets(j)[0], ng.input_sockets(f)[1]));
      REQUIRE(!compile_ng());
    }

    SECTION("f int (f2 float float)")
    {
      auto f2 = ng.create_copy(root, f);
      auto i  = ng.create_copy(root, int_func);
      auto j  = ng.create_copy(root, float_func);
      REQUIRE(ng.connect(ng.output_sockets(i)[0], ng.input_sockets(f)[0]));
      REQUIRE(ng.connect(ng.output_sockets(f2)[0], ng.input_sockets(f)[1]));
      REQUIRE(ng.connect(ng.output_sockets(j)[0], ng.input_sockets(f2)[0]));
      REQUIRE(ng.connect(ng.output_sockets(j)[0], ng.input_sockets(f2)[1]));
      REQUIRE(!compile_ng());
    }
  }
}