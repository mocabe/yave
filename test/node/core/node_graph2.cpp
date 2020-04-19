//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/core/node_graph2.hpp>
#include <yave/module/std/primitive/primitive.hpp>
#include <catch2/catch.hpp>

using namespace yave;

TEST_CASE("init")
{
  node_graph2 ng;
  auto root = ng.create_group({});
  ng.set_group_name(root, "root");
  REQUIRE(root);
  REQUIRE(ng.exists(root));
  REQUIRE(ng.get_group_members(root).empty());
  REQUIRE(ng.get_parent_group(root) == node_handle());
  REQUIRE(ng.get_definition(root) == root);
  REQUIRE(ng.input_sockets(root).empty());
  REQUIRE(ng.output_sockets(root).empty());
  REQUIRE(ng.input_connections(root).empty());
  REQUIRE(ng.output_connections(root).empty());
  REQUIRE(ng.get_group_nodes(root).size() == 2);
  REQUIRE(ng.exists(ng.get_group_nodes(root)[0]));
  REQUIRE(ng.exists(ng.get_group_nodes(root)[1]));
  REQUIRE(ng.is_group(root));
  REQUIRE(!ng.is_function(root));
  REQUIRE(ng.get_info(root));
  REQUIRE(ng.get_info(root)->name() == "root");
  REQUIRE(ng.get_info(root)->input_sockets().empty());
  REQUIRE(ng.get_info(root)->output_sockets().empty());
  REQUIRE(ng.get_group_members(root).empty());
  REQUIRE(ng.exists(ng.get_group_input(root)));
  REQUIRE(ng.exists(ng.get_group_output(root)));
  REQUIRE(ng.get_parent_group(ng.get_group_input(root)) == root);
  REQUIRE(ng.get_parent_group(ng.get_group_output(root)) == root);
  REQUIRE(
    ng.get_definition(ng.get_group_input(root)) == ng.get_group_input(root));
  REQUIRE(
    ng.get_definition(ng.get_group_output(root)) == ng.get_group_output(root));
}

TEST_CASE("root destroy")
{
  node_graph2 ng;
  auto root = ng.create_group({});
  ng.set_group_name(root, "root");
  REQUIRE(root);
  REQUIRE(ng.exists(root));
  ng.destroy(root);
  REQUIRE(!ng.exists(root));
  ng.destroy(root);
}

TEST_CASE("root copy")
{
  node_graph2 ng;
  auto root = ng.create_group({});
  ng.set_group_name(root, "root");
  auto n = ng.create_copy(root, root);
  REQUIRE(!n);
  REQUIRE(!ng.exists(n));
}

TEST_CASE("root add group out")
{
  node_graph2 ng;
  auto root = ng.create_group({});
  ng.set_group_name(root, "root");
  REQUIRE(ng.get_info(root)->input_sockets().empty());
  REQUIRE(ng.get_info(root)->output_sockets().empty());

  // {} -> test1
  auto s1 = ng.add_output_socket(root, "test1");
  REQUIRE(ng.exists(s1));
  REQUIRE(ng.node(s1) == root);
  REQUIRE(ng.get_info(root)->output_sockets().size() == 1);
  REQUIRE(ng.get_info(root)->input_sockets().size() == 0);
  REQUIRE(ng.get_info(root)->output_sockets()[0] == s1);
  REQUIRE(ng.get_info(s1)->name() == "test1");

  // test1 -> test1, test2
  auto s2 = ng.add_output_socket(root, "test2");
  REQUIRE(s2);
  REQUIRE(ng.exists(s2));
  REQUIRE(ng.node(s2) == root);
  REQUIRE(ng.get_info(root)->output_sockets().size() == 2);
  REQUIRE(ng.get_info(root)->input_sockets().size() == 0);
  REQUIRE(ng.get_info(root)->output_sockets()[1] == s2);
  REQUIRE(ng.get_info(s2)->name() == "test2");

  // test1, test2 -> test3, test1, test2
  auto s3 = ng.add_output_socket(root, "test3", 0);
  REQUIRE(s3);
  REQUIRE(ng.exists(s3));
  REQUIRE(ng.node(s3) == root);
  REQUIRE(ng.get_info(root)->output_sockets().size() == 3);
  REQUIRE(ng.get_info(root)->input_sockets().size() == 0);
  REQUIRE(ng.get_info(root)->output_sockets()[0] == s3);
  REQUIRE(ng.get_info(root)->output_sockets()[1] == s1);
  REQUIRE(ng.get_info(root)->output_sockets()[2] == s2);

  REQUIRE(ng.get_group_members(root).empty());
  REQUIRE(ng.get_info(ng.get_group_output(root))->input_sockets().size() == 3);
  REQUIRE(ng.get_info(ng.get_group_output(root))->output_sockets().size() == 0);
  REQUIRE(ng.get_info(ng.get_group_input(root))->input_sockets().size() == 0);
  REQUIRE(ng.get_info(ng.get_group_input(root))->output_sockets().size() == 0);

  // s3 s1 s2 -> s3 s2
  ng.remove_socket(ng.output_sockets(root)[1]);
  REQUIRE(ng.get_info(root)->output_sockets().size() == 2);
  REQUIRE(ng.get_info(root)->output_sockets()[0] == s3);
  REQUIRE(ng.get_info(root)->output_sockets()[1] == s2);
  REQUIRE(ng.get_info(ng.get_group_output(root))->input_sockets().size() == 2);
  REQUIRE(
    ng.get_info(ng.get_info(ng.get_group_output(root))->input_sockets()[0])
      ->name()
    == "test3");
  REQUIRE(
    ng.get_info(ng.get_info(ng.get_group_output(root))->input_sockets()[1])
      ->name()
    == "test2");
}

TEST_CASE("group socket extra")
{
  node_graph2 ng;
  auto root = ng.create_group({});

  auto in  = ng.get_group_input(root);
  auto out = ng.get_group_output(root);
  REQUIRE(ng.input_sockets(in).empty());
  REQUIRE(ng.output_sockets(in).empty());
  REQUIRE(ng.input_sockets(out).empty());
  REQUIRE(ng.output_sockets(out).empty());

  REQUIRE(ng.add_input_socket(root, "1"));
  REQUIRE(ng.add_output_socket(in, "2"));
  REQUIRE(!ng.add_input_socket(in, "3"));
  REQUIRE(ng.input_sockets(root).size() == 2);
  REQUIRE(ng.output_sockets(ng.get_group_input(root)).size() == 2);
  REQUIRE(ng.output_sockets(root).empty());
  for (auto&& s : ng.input_sockets(root))
    ng.remove_socket(s);

  REQUIRE(ng.add_output_socket(root, "1"));
  REQUIRE(ng.add_input_socket(out, "2"));
  REQUIRE(!ng.add_output_socket(out, "3"));
  REQUIRE(ng.output_sockets(root).size() == 2);
  REQUIRE(ng.input_sockets(ng.get_group_output(root)).size() == 2);
  REQUIRE(ng.input_sockets(root).empty());
  for (auto&& s : ng.output_sockets(root))
    ng.remove_socket(s);
}

TEST_CASE("root set socket name")
{
  node_graph2 ng;
  auto root = ng.create_group({});
  ng.set_group_name(root, "root");

  auto s = ng.add_output_socket(root, "test");

  REQUIRE(ng.exists(s));
  REQUIRE(ng.get_info(s)->name() == "test");
  REQUIRE(
    ng.get_info(ng.get_info(ng.get_group_output(root))->input_sockets()[0])
      ->name()
    == "test");

  ng.set_socket_name(s, "test2");
  REQUIRE(ng.get_info(s)->name() == "test2");
  REQUIRE(
    ng.get_info(ng.get_info(ng.get_group_output(root))->input_sockets()[0])
      ->name()
    == "test2");

  // s -> {}
  ng.remove_socket(ng.output_sockets(root)[0]);
  REQUIRE(ng.get_info(root)->output_sockets().empty());
  REQUIRE(ng.get_info(ng.get_group_output(root))->output_sockets().empty());
}

TEST_CASE("root add func")
{
  node_graph2 ng;
  auto root = ng.create_group({});
  ng.set_group_name(root, "root");

  auto decl = get_node_declaration<node::Int>();
  auto func = ng.create_function(decl);
  REQUIRE(ng.exists(func));
  REQUIRE(!ng.is_group(func));
  REQUIRE(!ng.is_group_input(func));
  REQUIRE(!ng.is_group_output(func));
  REQUIRE(ng.is_function(func));
  REQUIRE(ng.is_group_member(func));
  REQUIRE(ng.get_parent_group(func) == node_handle());

  for (size_t i = 0; i < decl.input_sockets().size(); ++i)
    REQUIRE(
      ng.get_info(ng.get_info(func)->input_sockets()[i])->name()
      == decl.input_sockets()[i]);

  for (size_t i = 0; i < decl.output_sockets().size(); ++i)
    REQUIRE(
      ng.get_info(ng.get_info(func)->output_sockets()[i])->name()
      == decl.output_sockets()[i]);

  REQUIRE(!ng.create_function(decl));

  // add call
  auto call = ng.create_copy(root, func);
  REQUIRE(ng.exists(call));
  REQUIRE(ng.get_parent_group(call) == root);

  for (size_t i = 0; i < decl.input_sockets().size(); ++i)
    REQUIRE(
      ng.get_info(ng.get_info(call)->input_sockets()[i])->name()
      == decl.input_sockets()[i]);

  for (size_t i = 0; i < decl.output_sockets().size(); ++i)
    REQUIRE(
      ng.get_info(ng.get_info(call)->output_sockets()[i])->name()
      == decl.output_sockets()[i]);

  // fail
  REQUIRE(!ng.add_input_socket(call, ""));
  REQUIRE(!ng.add_output_socket(call, ""));
  ng.set_group_name(call, "");
  ng.set_socket_name(ng.input_sockets(call)[0], "");
  REQUIRE(*ng.get_name(call) == decl.name());
  REQUIRE(*ng.get_name(ng.input_sockets(call)[0]) == decl.input_sockets()[0]);
}

TEST_CASE("func conn")
{
  node_graph2 ng;
  auto root = ng.create_group({});
  ng.set_group_name(root, "root");

  auto decl = get_node_declaration<node::Int>();
  auto func = ng.create_function(decl);

  SECTION("global")
  {
    auto fi = ng.input_sockets(func)[0];
    auto fo = ng.output_sockets(func)[0];
    REQUIRE(!ng.connect(fi, fi));
    REQUIRE(!ng.connect(fi, fo));
    REQUIRE(!ng.connect(fo, fo));
    REQUIRE(!ng.connect(fo, fi));
  }

  SECTION("self")
  {
    auto f  = ng.create_copy(root, func);
    auto fi = ng.input_sockets(f)[0];
    auto fo = ng.output_sockets(f)[0];
    REQUIRE(!ng.connect(fi, fi));
    REQUIRE(!ng.connect(fi, fo));
    REQUIRE(!ng.connect(fo, fo));
    REQUIRE(!ng.connect(fo, fi));
  }

  SECTION("multi")
  {
    auto f1 = ng.create_copy(root, func);
    auto f2 = ng.create_copy(root, f1);
    auto f3 = ng.create_copy(root, f2);
    REQUIRE(ng.exists(f1));
    REQUIRE(ng.exists(f2));
    REQUIRE(ng.exists(f3));
    REQUIRE(ng.get_definition(f1) == func);
    REQUIRE(ng.get_definition(f2) == func);
    REQUIRE(ng.get_definition(f3) == func);

    auto f1i = ng.input_sockets(f1)[0];
    auto f1o = ng.output_sockets(f1)[0];
    auto f2i = ng.input_sockets(f2)[0];
    auto f2o = ng.output_sockets(f2)[0];
    auto f3i = ng.input_sockets(f3)[0];

    REQUIRE(!ng.connect(f1i, f2i));
    REQUIRE(!ng.connect(f1o, f2o));

    REQUIRE(ng.connect(f1o, f2i));
    REQUIRE(ng.connect(f1o, f2i));
    REQUIRE(ng.connections(f1o).size() == 1);
    REQUIRE(ng.connections(f2i).size() == 1);
    REQUIRE(ng.connections(f1o) == ng.connections(f2i));

    REQUIRE(ng.connect(f1o, f3i));
    REQUIRE(!ng.connect(f2o, f1i));
  }
}

TEST_CASE("func destroy")
{
  node_graph2 ng;
  auto root = ng.create_group({});
  ng.set_group_name(root, "root");

  auto decl = get_node_declaration<node::Int>();
  auto func = ng.create_function(decl);

  REQUIRE(!ng.create_copy(nullptr, func));
  REQUIRE(!ng.create_clone(nullptr, func));

  auto f1 = ng.create_copy(root, func);
  auto f2 = ng.create_clone(root, func);
  REQUIRE(ng.exists(f1));
  REQUIRE(ng.exists(f2));

  REQUIRE(ng.get_definition(f1) == func);
  REQUIRE(ng.get_definition(f2) == func);

  REQUIRE(ng.get_parent_group(f1) == root);
  REQUIRE(ng.get_parent_group(f2) == root);

  ng.destroy(f1);
  REQUIRE(!ng.exists(f1));
  REQUIRE(ng.exists(func));
  REQUIRE(ng.exists(f2));
  ng.destroy(func);
  REQUIRE(!ng.exists(func));
  REQUIRE(!ng.exists(f2));
}

TEST_CASE("group")
{
  node_graph2 ng;
  auto root = ng.create_group({});
  ng.set_group_name(root, "root");
  auto decl = get_node_declaration<node::Int>();
  auto func = ng.create_function(decl);

  SECTION("")
  {
    auto g = ng.create_group(root);
    REQUIRE(ng.exists(g));
    REQUIRE(ng.is_group(g));
    REQUIRE(ng.is_definition(g));
    REQUIRE(ng.input_sockets(g).empty());
    REQUIRE(ng.output_sockets(g).empty());
    REQUIRE(ng.get_group_members(root).size() == 1);
    ng.destroy(g);
    REQUIRE(!ng.exists(g));
    REQUIRE(ng.get_group_members(root).empty());
  }

  SECTION("")
  {
    auto f1 = ng.create_copy(root, func);
    auto g  = ng.create_group(root, {f1});
    REQUIRE(f1);
    REQUIRE(g);
    REQUIRE(ng.exists(f1));
    REQUIRE(ng.exists(g));
    REQUIRE(ng.exists(root));
    REQUIRE(ng.get_group_members(root)[0] == g);
    REQUIRE(ng.get_group_members(g)[0] == f1);
  }

  SECTION("")
  {
    // <------- root ------>
    // [out] <- [f1] <- [in]
    auto f1 = ng.create_copy(root, func);
    auto os = ng.add_output_socket(root, "out");
    auto is = ng.add_input_socket(root, "in");
    auto ic = ng.connect(
      ng.output_sockets(ng.get_group_input(root))[0], ng.input_sockets(f1)[0]);
    auto oc = ng.connect(
      ng.output_sockets(f1)[0], ng.input_sockets(ng.get_group_output(root))[0]);

    REQUIRE(ng.exists(f1));
    REQUIRE(ng.exists(os));
    REQUIRE(ng.exists(is));
    REQUIRE(ng.exists(oc));
    REQUIRE(ng.exists(ic));
    REQUIRE(ng.connections(os).empty());
    REQUIRE(ng.connections(is).empty());
    REQUIRE(ng.output_connections(f1).size() == 1);
    REQUIRE(ng.input_connections(f1).size() == 1);
    REQUIRE(ng.output_connections(ng.get_group_input(root)).size() == 1);
    REQUIRE(ng.input_connections(ng.get_group_output(root)).size() == 1);

    // <----------- root ------------>
    //          <----- g ---->
    // [out] <- [ <- [f1] <- ] <- [in]
    auto g = ng.create_group(root, {f1});
    REQUIRE(ng.exists(g));
    REQUIRE(!ng.exists(oc));
    REQUIRE(!ng.exists(ic));

    REQUIRE(ng.get_group_members(root).size() == 1);
    REQUIRE(ng.exists(ng.get_group_members(root)[0]));

    REQUIRE(ng.get_group_members(g).size() == 1);
    REQUIRE(ng.get_group_members(g)[0] == f1);

    REQUIRE(ng.output_sockets(g).size() == 1);
    REQUIRE(ng.input_sockets(g).size() == 1);

    REQUIRE(ng.connections(ng.output_sockets(g)[0]).size() == 1);
    REQUIRE(ng.connections(ng.input_sockets(g)[0]).size() == 1);
    REQUIRE(ng.connections(ng.output_sockets(f1)[0]).size() == 1);
    REQUIRE(ng.connections(ng.input_sockets(f1)[0]).size() == 1);

    REQUIRE(
      ng.get_group_output(root)
      == ng.get_info(ng.connections(ng.output_sockets(g)[0])[0])->dst_node());
    REQUIRE(
      ng.get_group_input(root)
      == ng.get_info(ng.connections(ng.input_sockets(g)[0])[0])->src_node());

    REQUIRE(
      ng.get_group_output(g)
      == ng.get_info(ng.connections(ng.output_sockets(f1)[0])[0])->dst_node());
    REQUIRE(
      ng.get_group_input(g)
      == ng.get_info(ng.connections(ng.input_sockets(f1)[0])[0])->src_node());

    REQUIRE(
      ng.get_info(ng.output_sockets(g)[0])->name() == decl.output_sockets()[0]);
    REQUIRE(
      ng.get_info(ng.input_sockets(g)[0])->name() == decl.input_sockets()[0]);

    REQUIRE(
      ng.get_info(ng.input_sockets(ng.get_group_output(g))[0])->name()
      == decl.output_sockets()[0]);
    REQUIRE(
      ng.get_info(ng.output_sockets(ng.get_group_input(g))[0])->name()
      == decl.input_sockets()[0]);
  }

  SECTION("")
  {
    auto f1 = ng.create_copy(root, func);
    auto f2 = ng.create_copy(root, func);
    auto f3 = ng.create_copy(root, func);

    auto test = [&](auto f) {
      REQUIRE(ng.exists(f));
      REQUIRE(ng.get_definition(f) == func);
      REQUIRE(ng.input_sockets(f).size() == 1);
      REQUIRE(ng.output_sockets(f).size() == 1);
    };
    test(f1);
    test(f2);
    test(f3);

    auto in  = ng.get_group_input(root);
    auto out = ng.get_group_output(root);

    auto is  = ng.add_output_socket(in, "");
    auto os1 = ng.add_input_socket(out, "");
    auto os2 = ng.add_input_socket(out, "");

    REQUIRE(ng.exists(is));
    REQUIRE(ng.exists(os1));
    REQUIRE(ng.exists(os2));

    auto f1o = ng.output_sockets(f1)[0];
    auto f1i = ng.input_sockets(f1)[0];
    auto f2o = ng.output_sockets(f2)[0];
    auto f2i = ng.input_sockets(f2)[0];
    auto f3o = ng.output_sockets(f3)[0];
    auto f3i = ng.input_sockets(f3)[0];

    REQUIRE(ng.connect(is, f1i));
    REQUIRE(ng.connect(f1o, f2i));
    REQUIRE(ng.connect(f1o, f3i));
    REQUIRE(ng.connect(f2o, os1));
    REQUIRE(ng.connect(f3o, os2));

    auto g = ng.create_group(root, {f1, f2, f3});
    REQUIRE(ng.exists(g));
    REQUIRE(ng.get_group_members(root)[0] == g);
    REQUIRE(ng.output_sockets(g).size() == 2);
    REQUIRE(ng.input_sockets(g).size() == 1);

    REQUIRE(ng.output_connections(f1).size() == 2);
    REQUIRE(ng.input_connections(f1).size() == 1);
    REQUIRE(ng.output_connections(in).size() == 1);
    REQUIRE(ng.input_connections(out).size() == 2);

    auto gg = ng.create_clone(root, g);
    REQUIRE(ng.exists(gg));
    REQUIRE(ng.get_group_members(gg).size() == 3);
  }
}