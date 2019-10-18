//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/compiler/node_compiler.hpp>
#include <yave/node/scene/scene_graph.hpp>
#include <yave/node/core/function.hpp>
#include <yave/backend/default/render/primitive_constructor.hpp>
#include <yave/backend/default/render/control_flow.hpp>
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
struct yave::node_info_traits<Add>
{
  static node_info get_node_info()
  {
    return node_info("add", {"x", "y"}, {"out"});
  }
};

template <>
struct yave::bind_info_traits<Add, test_backend>
{
  static bind_info get_bind_info()
  {
    return bind_info(
      "add", {"x", "y"}, "out", make_object<InstanceGetterFunction<Add>>(), "");
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
struct yave::node_info_traits<AddD>
{
  static node_info get_node_info()
  {
    return node_info("add", {"x", "y"}, {"out"});
  }
};

template <>
struct yave::bind_info_traits<AddD, test_backend>
{
  static bind_info get_bind_info()
  {
    return bind_info(
      "add",
      {"x", "y"},
      "out",
      make_object<InstanceGetterFunction<AddD>>(),
      "");
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

  auto c1 =
    graph.connect(graph.output_sockets(i1)[0], graph.input_sockets(add)[0]);
  auto c2 =
    graph.connect(graph.output_sockets(i2)[0], graph.input_sockets(add)[1]);

  REQUIRE(c1);
  REQUIRE(c2);

  REQUIRE(bim.add(get_bind_info<Add, test_backend>()));
  REQUIRE(bim.add(get_bind_info<node::Int, backend::tags::default_render>()));

  REQUIRE(compiler.compile({std::move(graph), add}, bim));
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

  auto c1 =
    graph.connect(graph.output_sockets(i)[0], graph.input_sockets(add)[0]);
  auto c2 =
    graph.connect(graph.output_sockets(i)[0], graph.input_sockets(add)[1]);

  REQUIRE(c1);
  REQUIRE(c2);

  REQUIRE(bim.add(get_bind_info<Add, test_backend>()));
  REQUIRE(bim.add(get_bind_info<node::Int, backend::tags::default_render>()));

  REQUIRE(compiler.compile({std::move(graph), add}, bim));
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

  REQUIRE(bim.add(get_bind_info<Add, test_backend>()));
  REQUIRE(bim.add(get_bind_info<node::Int, backend::tags::default_render>()));

  REQUIRE(compiler.compile({std::move(graph), add1}, bim));
}

TEST_CASE("add x d", "[node_compiler]")
{
  node_compiler compiler;
  node_graph graph;
  bind_info_manager bim;

  auto add = graph.add(get_node_info<Add>());
  auto i   = graph.add(get_node_info<node::Int>());
  auto d   = graph.add(get_node_info<node::Double>());

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

  REQUIRE(bim.add(get_bind_info<Add, test_backend>()));
  REQUIRE(bim.add(get_bind_info<node::Int, backend::tags::default_render>()));
  REQUIRE(bim.add(get_bind_info<node::Double, backend::tags::default_render>()));

  REQUIRE(!compiler.compile({std::move(graph), add}, bim));
}

TEST_CASE("add (add x d) x", "[node_compiler]")
{
  node_compiler compiler;
  node_graph graph;
  bind_info_manager bim;

  auto add1 = graph.add(get_node_info<Add>());
  auto add2 = graph.add(get_node_info<Add>());
  auto i    = graph.add(get_node_info<node::Int>());
  auto d    = graph.add(get_node_info<node::Double>());

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

  REQUIRE(bim.add(get_bind_info<Add, test_backend>()));
  REQUIRE(bim.add(get_bind_info<node::Int, backend::tags::default_render>()));
  REQUIRE(bim.add(get_bind_info<node::Double, backend::tags::default_render>()));

  REQUIRE(!compiler.compile({std::move(graph), add1}, bim));
}

TEST_CASE("if b x y", "[node_compiler]")
{
  node_compiler compiler;
  node_graph graph;
  bind_info_manager bim;

  auto _if = graph.add(get_node_info<node::If>());
  auto i   = graph.add(get_node_info<node::Int>());
  auto b   = graph.add(get_node_info<node::Bool>());

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

  REQUIRE(bim.add(get_bind_info<node::Int, backend::tags::default_render>()));
  REQUIRE(bim.add(get_bind_info<node::Bool, backend::tags::default_render>()));
  REQUIRE(bim.add(get_bind_info<node::If, backend::tags::default_render>()));

  REQUIRE(compiler.compile({std::move(graph), _if}, bim));
}

TEST_CASE("if b (if b x y) z", "[node_compiler]")
{
  node_compiler compiler;
  node_graph graph;
  bind_info_manager bim;

  auto if1 = graph.add(get_node_info<node::If>());
  auto if2 = graph.add(get_node_info<node::If>());
  auto i   = graph.add(get_node_info<node::Int>());
  auto b   = graph.add(get_node_info<node::Bool>());

  REQUIRE(if1);
  REQUIRE(if2);
  REQUIRE(i);
  REQUIRE(b);

  auto b_value = graph.output_sockets(b)[0];
  auto i_value = graph.output_sockets(i)[0];

  auto if1_cond = graph.input_sockets(if1)[0];
  auto if1_then = graph.input_sockets(if1)[1];
  auto if1_else = graph.input_sockets(if1)[2];

  auto if2_cond = graph.input_sockets(if1)[0];
  auto if2_then = graph.input_sockets(if1)[1];
  auto if2_else = graph.input_sockets(if1)[2];
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

  REQUIRE(bim.add(get_bind_info<node::Int, backend::tags::default_render>()));
  REQUIRE(bim.add(get_bind_info<node::Bool, backend::tags::default_render>()));
  REQUIRE(bim.add(get_bind_info<node::If, backend::tags::default_render>()));

  REQUIRE(compiler.compile({std::move(graph), if1}, bim));
}

TEST_CASE("if b x d", "[node_compiler]")
{
  node_compiler compiler;
  node_graph graph;
  bind_info_manager bim;

  auto _if = graph.add(get_node_info<node::If>());
  auto i   = graph.add(get_node_info<node::Int>());
  auto d   = graph.add(get_node_info<node::Double>());
  auto b   = graph.add(get_node_info<node::Bool>());

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

  REQUIRE(bim.add(get_bind_info<node::Int, backend::tags::default_render>()));
  REQUIRE(bim.add(get_bind_info<node::Double, backend::tags::default_render>()));
  REQUIRE(bim.add(get_bind_info<node::Bool, backend::tags::default_render>()));
  REQUIRE(bim.add(get_bind_info<node::If, backend::tags::default_render>()));

  REQUIRE(!compiler.compile({std::move(graph), _if}, bim));
}

TEST_CASE("add<int> x y", "[node_compiler]")
{
  node_compiler compiler;
  node_graph graph;
  bind_info_manager bim;

  auto add = graph.add(get_node_info<Add>());
  auto i   = graph.add(get_node_info<node::Int>());

  REQUIRE(add);
  REQUIRE(i);

  auto i_value = graph.output_sockets(i)[0];
  auto add_x   = graph.input_sockets(add)[0];
  auto add_y   = graph.input_sockets(add)[1];

  auto c1 = graph.connect(i_value, add_x);
  auto c2 = graph.connect(i_value, add_y);

  REQUIRE(c1);
  REQUIRE(c2);

  REQUIRE(bim.add(get_bind_info<Add, test_backend>()));
  REQUIRE(bim.add(get_bind_info<AddD, test_backend>()));
  REQUIRE(bim.add(get_bind_info<node::Int, backend::tags::default_render>()));

  REQUIRE(compiler.compile({std::move(graph), add}, bim));
}

TEST_CASE("add<double> x y", "[node_compiler]")
{
  node_compiler compiler;
  node_graph graph;
  bind_info_manager bim;

  auto add = graph.add(get_node_info<Add>());
  auto d   = graph.add(get_node_info<node::Double>());

  REQUIRE(add);
  REQUIRE(d);

  auto d_value = graph.output_sockets(d)[0];
  auto add_x   = graph.input_sockets(add)[0];
  auto add_y   = graph.input_sockets(add)[1];

  auto c1 = graph.connect(d_value, add_x);
  auto c2 = graph.connect(d_value, add_y);

  REQUIRE(c1);
  REQUIRE(c2);

  REQUIRE(bim.add(get_bind_info<Add, test_backend>()));
  REQUIRE(bim.add(get_bind_info<AddD, test_backend>()));
  REQUIRE(bim.add(get_bind_info<node::Double, backend::tags::default_render>()));

  REQUIRE(compiler.compile({std::move(graph), add}, bim));
}

TEST_CASE("add<?> x y", "[node_compiler]")
{
  node_compiler compiler;
  node_graph graph;
  bind_info_manager bim;

  auto add = graph.add(get_node_info<Add>());
  auto i   = graph.add(get_node_info<node::Int>());
  auto d   = graph.add(get_node_info<node::Double>());

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

  REQUIRE(bim.add(get_bind_info<Add, test_backend>()));
  REQUIRE(bim.add(get_bind_info<AddD, test_backend>()));
  REQUIRE(bim.add(get_bind_info<node::Int, backend::tags::default_render>()));
  REQUIRE(bim.add(get_bind_info<node::Double, backend::tags::default_render>()));

  REQUIRE(!compiler.compile({std::move(graph), add}, bim));
}