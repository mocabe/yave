//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/core/structured_node_graph.hpp>
#include <yave/module/std/primitive/primitive.hpp>
#include <catch2/catch.hpp>

#include <iostream>

using namespace yave;

TEST_CASE("init")
{
  structured_node_graph ng;
  ng.clear();
  auto ng2 = ng.clone();
  (void)ng2.clone();
}

TEST_CASE("root")
{
  structured_node_graph ng;
  auto root = ng.create_group({});
  ng.set_name(root, "root");
  REQUIRE(root);
  REQUIRE(ng.exists(root));
  REQUIRE(ng.get_group_members(root).empty());
  REQUIRE(ng.get_group_nodes(root).size() == 2);
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

  auto null = node_handle();
  REQUIRE(!ng.exists(null));
  REQUIRE(ng.get_group_members(null).empty());
  REQUIRE(ng.get_group_nodes(null).empty());
  REQUIRE(!ng.get_definition(null));
  REQUIRE(!ng.is_group(null));
  REQUIRE(!ng.is_function(null));
  REQUIRE(!ng.is_definition(null));
  REQUIRE(!ng.is_group_member(null));
  REQUIRE(!ng.get_path(null));

  REQUIRE(!ng.is_definition(ng.get_group_input(root)));
  REQUIRE(!ng.is_definition(ng.get_group_output(root)));
  REQUIRE(ng.get_definition(ng.get_group_input(root)) == node_handle());
  REQUIRE(ng.get_definition(ng.get_group_output(root)) == node_handle());

  REQUIRE(ng.get_path(root) == "/root");
  REQUIRE(
    ng.get_path(ng.get_group_input(root))
    == ("/root/" + *ng.get_name(ng.get_group_input(root))));
  REQUIRE(
    ng.get_path(ng.get_group_output(root))
    == ("/root/" + *ng.get_name(ng.get_group_output(root))));
}

TEST_CASE("root destroy")
{
  structured_node_graph ng;
  auto root = ng.create_group({});
  ng.set_name(root, "root");
  REQUIRE(root);
  REQUIRE(ng.exists(root));
  ng.destroy(root);
  REQUIRE(!ng.exists(root));
  ng.destroy(root);
}

TEST_CASE("root copy")
{
  structured_node_graph ng;
  auto root = ng.create_group({});
  ng.set_name(root, "root");
  auto n = ng.create_copy(root, root);
  REQUIRE(!n);
  REQUIRE(!ng.exists(n));
}

TEST_CASE("gruop")
{
  structured_node_graph ng;
  auto root = ng.create_group({});
  ng.set_name(root, "root");

  REQUIRE(ng.exists(root));
  REQUIRE(ng.get_definition(root));
  REQUIRE(ng.get_name(root) == "root");
  REQUIRE(ng.input_sockets(root).empty());
  REQUIRE(ng.output_sockets(root).empty());
  REQUIRE(ng.get_definition(root) == root);
  REQUIRE(ng.exists(ng.get_group_input(root)));
  REQUIRE(ng.exists(ng.get_group_output(root)));

  SECTION("set name")
  {
    SECTION("collision")
    {
      auto g = ng.create_group(nullptr);
      REQUIRE(ng.exists(g));
      auto name = *ng.get_name(g);
      ng.set_name(g, "root");
      REQUIRE(ng.get_name(g) == name);
    }

    SECTION("call")
    {
      auto g = ng.create_copy(nullptr, root);
      REQUIRE(ng.exists(g));
      ng.set_name(g, "test");
      REQUIRE(ng.get_name(g) == "root");
    }

    SECTION("io")
    {
      auto g    = ng.create_group(nullptr);
      auto name = *ng.get_name(ng.get_group_input(g));
      ng.set_name(ng.get_group_input(g), "test");
      REQUIRE(ng.get_name(ng.get_group_input(g)) == name);

      auto gg = ng.create_group(g);
      ng.set_name(gg, name);
      REQUIRE(*ng.get_name(gg) == name);
    }
  }

  SECTION("clone")
  {
    SECTION("para")
    {
      auto g = ng.create_clone(nullptr, root);
      REQUIRE(ng.exists(g));
      REQUIRE(ng.get_name(g) != ng.get_name(root));

      REQUIRE(ng.create_group(root));
      auto gg = ng.create_clone(nullptr, root);
      REQUIRE(ng.exists(gg));
      REQUIRE(ng.get_name(gg) != ng.get_name(g));
      REQUIRE(ng.get_name(gg) != ng.get_name(root));
      REQUIRE(ng.get_group_members(gg).size() == 1);
      REQUIRE(
        ng.get_definition(ng.get_group_members(gg)[0])
        != ng.get_definition(ng.get_group_members(root)[0]));
    }

    SECTION("good rec")
    {
      auto g = ng.create_clone(root, root);
      REQUIRE(ng.exists(g));
      REQUIRE(g != root);
      REQUIRE(ng.get_parent_group(g) == root);
      REQUIRE(ng.get_group_members(root) == std::vector {g});
      REQUIRE(ng.get_group_members(g).empty());

      auto gg = ng.create_clone(g, root);
      REQUIRE(ng.exists(gg));
      REQUIRE(g != gg);
      REQUIRE(ng.get_parent_group(gg) == g);
      REQUIRE(ng.get_group_members(g) == std::vector {gg});
    }

    SECTION("bad rec")
    {
      auto g = ng.create_group(nullptr);
      REQUIRE(ng.create_copy(g, root));
      REQUIRE(!ng.create_clone(root, g));
      REQUIRE(ng.exists(g));
      REQUIRE(ng.get_group_members(g).size() == 1);
      REQUIRE(ng.exists(root));
      REQUIRE(ng.get_group_members(root).empty());
    }
  }
}

TEST_CASE("root add group out")
{
  structured_node_graph ng;
  auto root = ng.create_group({});
  ng.set_name(root, "root");
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
  structured_node_graph ng;
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
  structured_node_graph ng;
  auto root = ng.create_group({});
  ng.set_name(root, "root");

  auto s = ng.add_output_socket(root, "test");

  REQUIRE(ng.exists(s));
  REQUIRE(ng.get_info(s)->name() == "test");
  REQUIRE(
    ng.get_info(ng.get_info(ng.get_group_output(root))->input_sockets()[0])
      ->name()
    == "test");

  ng.set_name(s, "test2");
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
  structured_node_graph ng;
  auto root = ng.create_group({});
  ng.set_name(root, "root");

  auto decl = get_node_declaration<node::Int>();
  auto func = ng.create_function(decl);
  REQUIRE(ng.exists(func));
  REQUIRE(!ng.is_group(func));
  REQUIRE(!ng.is_group_input(func));
  REQUIRE(!ng.is_group_output(func));
  REQUIRE(ng.is_function(func));
  REQUIRE(ng.is_group_member(func));

  REQUIRE(*ng.get_path(func) == decl.qualified_name());

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
  REQUIRE(ng.get_group_members(root).size() == 1);
  REQUIRE(ng.get_group_nodes(root).size() == 3);

  REQUIRE(*ng.get_path(call) == "/root/" + decl.name());
  REQUIRE(*ng.get_path(ng.get_definition(call)) == decl.qualified_name());

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
  ng.set_name(call, "");
  ng.set_name(ng.input_sockets(call)[0], "");
  REQUIRE(*ng.get_name(call) == decl.name());
  REQUIRE(*ng.get_name(ng.input_sockets(call)[0]) == decl.input_sockets()[0]);
}

TEST_CASE("func conn")
{
  structured_node_graph ng;
  auto root = ng.create_group({});
  ng.set_name(root, "root");

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
  structured_node_graph ng;
  auto root = ng.create_group({});
  ng.set_name(root, "root");

  auto decl = get_node_declaration<node::Int>();
  auto func = ng.create_function(decl);

  {
    auto decl2 = get_node_declaration<node::String>();
    auto func2 = ng.create_function(decl2);
    REQUIRE(ng.exists(func2));
    ng.destroy(func2);
  }

  {
    auto func1 = ng.create_copy(nullptr, func);
    auto func2 = ng.create_clone(nullptr, func);
    REQUIRE(ng.exists(func1));
    REQUIRE(ng.exists(func2));
    REQUIRE(
      !ng.connect(ng.output_sockets(func1)[0], ng.input_sockets(func2)[0]));
    ng.destroy(func1);
    ng.destroy(func2);
    REQUIRE(!ng.exists(func1));
    REQUIRE(!ng.exists(func2));
    REQUIRE(!ng.create_group(nullptr, {root}));
  }

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
  structured_node_graph ng;
  auto root = ng.create_group({});
  ng.set_name(root, "root");
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

  SECTION("misc")
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

    REQUIRE(ng.get_parent_group(ng.get_group_members(g)[0]) == g);
    REQUIRE(ng.get_parent_group(ng.get_group_members(g)[1]) == g);
    REQUIRE(ng.get_parent_group(ng.get_group_members(g)[2]) == g);

    REQUIRE(ng.get_group_members(g)[0] == f1);
    REQUIRE(ng.get_group_nodes(g)[2] == f1);
    ng.bring_front(f1);
    REQUIRE(ng.get_group_members(g).back() == f1);
    REQUIRE(ng.get_group_nodes(g).back() == f1);
    ng.bring_back(f1);
    REQUIRE(ng.get_group_members(g).front() == f1);
    REQUIRE(ng.get_group_nodes(g).front() == f1);
    ng.bring_back(ng.get_group_input(g));
    REQUIRE(ng.get_group_members(g).front() == f1);
    REQUIRE(ng.get_group_nodes(g).front() == ng.get_group_input(g));

    REQUIRE(ng.output_connections(f1).size() == 2);
    REQUIRE(ng.input_connections(f1).size() == 1);
    REQUIRE(ng.output_connections(in).size() == 1);
    REQUIRE(ng.input_connections(out).size() == 2);

    auto gg = ng.create_clone(root, g);
    REQUIRE(ng.exists(gg));
    REQUIRE(ng.get_group_members(gg).size() == 3);

    ng.destroy(g);
    ng.destroy(gg);
    REQUIRE(!ng.exists(g));
    REQUIRE(!ng.exists(gg));
  }
}

TEST_CASE("clone")
{
  structured_node_graph ng;
  auto root = ng.create_group(nullptr);
  ng.set_name(root, "root");
  auto decl = get_node_declaration<node::Int>();
  auto func = ng.create_function(decl);

  auto f1 = ng.create_copy(root, func);
  auto f2 = ng.create_copy(root, func);

  REQUIRE(ng.add_output_socket(root, ""));

  REQUIRE(ng.connect(ng.output_sockets(f1)[0], ng.input_sockets(f2)[0]));
  REQUIRE(ng.connect(
    ng.output_sockets(f2)[0], ng.input_sockets(ng.get_group_output(root))[0]));

  auto ng2   = ng.clone();
  auto root2 = ng2.node(root.id());
  REQUIRE(!ng2.exists(root));
  REQUIRE(ng2.exists(root2));

  REQUIRE(ng2.get_group_members(root2).size() == 2);
  REQUIRE(ng2.exists(ng2.get_group_input(root2)));
  REQUIRE(ng2.exists(ng2.get_group_output(root2)));
  REQUIRE(ng2.get_name(root2) == "root");
  ng2.set_name(root2, "test");
  REQUIRE(ng.get_name(root) == "root");
  REQUIRE(*ng2.get_name(root2) == "test");

  REQUIRE(ng2.add_input_socket(root2, "1"));
  REQUIRE(ng2.input_sockets(root2).size() == 1);
  REQUIRE(ng2.get_name(ng2.input_sockets(root2)[0]) == "1");
  REQUIRE(ng.input_sockets(root).size() == 0);

  auto f12 = ng2.node(f1.id());
  auto f22 = ng2.node(f2.id());
  REQUIRE(ng2.exists(f12));
  REQUIRE(ng2.exists(f22));

  REQUIRE(ng2.output_connections(f12).size() == 1);
  REQUIRE(ng2.output_connections(f22).size() == 1);

  REQUIRE(ng2.get_info(ng2.output_connections(f12)[0])->dst_node() == f22);
  REQUIRE(ng2.exists(ng2.get_group_output(root2)));

  REQUIRE(ng2.is_group_output(ng2.get_group_output(root2)));
  REQUIRE(
    ng2.get_info(ng2.output_connections(f22)[0])->dst_node()
    == ng2.get_group_output(root2));
}

TEST_CASE("path")
{
  structured_node_graph ng;
  auto root = ng.create_group(nullptr);
  ng.set_name(root, "root");
  auto decl = get_node_declaration<node::Int>();
  auto func = ng.create_function(decl);

  REQUIRE(*ng.get_path(root) == "/root");
  REQUIRE(*ng.get_path(func) == decl.qualified_name());

  REQUIRE(ng.search_path("").empty());
  REQUIRE(ng.search_path("/").size() == 2); // root, std
  REQUIRE(ng.search_path("/").front() == root);
  REQUIRE(ng.search_path("//").empty());
  REQUIRE(ng.search_path("/root") == std::vector {root});
  REQUIRE(ng.search_path("/root/").empty());
  REQUIRE(ng.search_path("foo").empty());
  REQUIRE(ng.search_path("/foo").empty());
  REQUIRE(ng.search_path(decl.qualified_name()) == std::vector {func});
  REQUIRE(ng.search_path(decl.qualified_name() + "/").empty());

  auto g = ng.create_group(root);
  ng.set_name(g, "g");
  REQUIRE(*ng.get_path(g) == "/root/g");
  REQUIRE(ng.search_path("/root/") == std::vector {g});
  REQUIRE(ng.search_path("/root/g") == std::vector {g});
  REQUIRE(ng.search_path("/root/foo").empty());
  REQUIRE(ng.search_path("/root////foo").empty());
  REQUIRE(ng.search_path("/root/In").empty());
}