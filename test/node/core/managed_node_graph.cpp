// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <catch2/catch.hpp>

#include <yave/node/core/managed_node_graph.hpp>
#include <yave/node/core/function.hpp>

#include <fmt/format.h>

using namespace yave;

TEST_CASE("root group")
{
  managed_node_graph ng;

  SECTION("0")
  {
  }

  SECTION("1")
  {
    ng = ng.clone();
  }

  auto root = ng.root_group();
  REQUIRE(root);
  REQUIRE(ng.is_group(root));
  REQUIRE(!ng.is_group_output(root));
  REQUIRE(!ng.is_group_input(root));
  REQUIRE(!ng.get_parent_group(root));

  ng.destroy(ng.root_group());
  REQUIRE(ng.is_group(root));

  REQUIRE(ng.get_group_input(root));
  REQUIRE(ng.get_group_output(root));
  REQUIRE(ng.get_group_members(root).empty());

  REQUIRE(ng.nodes(ng.root_group()).size() == 2);

  for (auto&& n : ng.nodes(ng.root_group())) {
    REQUIRE((ng.is_group_output(n) || ng.is_group_input(n)));
  }

  auto root_in = ng.get_group_input(root);
  auto root_ot = ng.get_group_output(root);

  REQUIRE(ng.get_info(root_in));
  REQUIRE(ng.get_info(root_ot));

  REQUIRE(ng.get_info(root_in)->input_sockets().empty());
  REQUIRE(ng.get_info(root_in)->output_sockets().empty());
  REQUIRE(ng.get_info(root_ot)->input_sockets().empty());
  REQUIRE(ng.get_info(root_ot)->output_sockets().empty());

  REQUIRE(ng.add_group_input_socket(root, "test", 0));
  REQUIRE(ng.add_group_output_socket(root, "test", 0));
  REQUIRE(ng.output_sockets(root).size() == 1);
  REQUIRE(ng.input_sockets(root).size() == 1);

  auto s1 = ng.output_sockets(root)[0];
  auto s2 = ng.input_sockets(root)[0];
  REQUIRE(ng.get_info(s1)->node() == root);
  REQUIRE(ng.get_info(s2)->node() == root);
  REQUIRE(ng.node(s1) == root);
  REQUIRE(ng.node(s2) == root);
}

TEST_CASE("empty group")
{
  managed_node_graph ng;
  auto g  = ng.group(ng.root_group(), {});
  auto gi = ng.get_info(g);
  REQUIRE(g);
  REQUIRE(gi);
  REQUIRE(ng.get_parent_group(g) == ng.root_group());

  REQUIRE(gi->output_sockets().empty());
  REQUIRE(gi->input_sockets().empty());

  REQUIRE(gi->type() == managed_node_type::group);
  REQUIRE(gi->is_group());
  REQUIRE(!gi->is_group_input());
  REQUIRE(!gi->is_group_output());
  REQUIRE(!gi->is_normal());
  REQUIRE(!gi->is_primitive());

  auto i = ng.get_group_input(g);
  auto o = ng.get_group_output(g);

  REQUIRE(i);
  REQUIRE(o);

  REQUIRE(ng.is_group_input(i));
  REQUIRE(ng.is_group_output(o));

  REQUIRE(ng.get_parent_group(i) == g);
  REQUIRE(ng.get_parent_group(o) == g);

  REQUIRE(ng.get_info(i)->is_group_input());
  REQUIRE(ng.get_info(o)->is_group_output());

  REQUIRE(ng.exists(g));

  SECTION("ungroup")
  {
    ng.ungroup(g);
    REQUIRE(!ng.exists(g));
  }

  SECTION("destroy")
  {
    ng.destroy(g);
    REQUIRE(!ng.exists(g));
  }
}

TEST_CASE("group with content")
{
  managed_node_graph ng;

  class X;

  auto decl = node_declaration(
    "node",
    {"0", "1", "2"},
    {"0", "1"},
    object_type<node_closure<X, X, X, X>>());

  REQUIRE(ng.register_node_decl(decl));

  SECTION("simple")
  {
    auto n = ng.create(ng.root_group(), decl.name());
    REQUIRE(n);
    REQUIRE(ng.get_info(n)->is_normal());
    REQUIRE(ng.get_group_members(ng.root_group()).front() == n);

    auto g = ng.group(ng.root_group(), {n});
    REQUIRE(g);
    REQUIRE(ng.get_parent_group(g) == ng.root_group());
    REQUIRE(ng.get_group_members(g).front() == n);
    REQUIRE(ng.get_group_members(ng.root_group()).front() == g);

    auto nn = ng.create(g, "node");

    REQUIRE(ng.get_group_members(g).size() == 2);
    REQUIRE(ng.get_parent_group(n) == g);
    REQUIRE(ng.get_parent_group(nn) == g);

    SECTION("ungroup")
    {
      ng.ungroup(g);
      REQUIRE(!ng.exists(g));
      REQUIRE(ng.get_parent_group(n) == ng.root_group());
      REQUIRE(ng.get_parent_group(nn) == ng.root_group());
    }

    SECTION("destroy")
    {
      ng.destroy(g);
      REQUIRE(!ng.exists(g));
      REQUIRE(!ng.exists(n));
      REQUIRE(!ng.exists(nn));
    }
  }

  SECTION("with input and outputs")
  {
    auto size = ng.node_graph().nodes().size();
    auto n1        = ng.create(ng.root_group(), decl.name());
    auto n2        = ng.create(ng.root_group(), decl.name());
    auto n3        = ng.create(ng.root_group(), decl.name());

    // root + 3
    REQUIRE(ng.node_graph().nodes().size() == size + 3);
    size += 3;

    SECTION("0")
    {
      //
      // n1 -> n2 -> n3
      //
      auto c1 = ng.connect(ng.output_sockets(n1)[0], ng.input_sockets(n2)[0]);
      auto c2 = ng.connect(ng.output_sockets(n2)[0], ng.input_sockets(n3)[0]);
      REQUIRE(c1);
      REQUIRE(c2);

      REQUIRE(ng.get_info(c1)->src_socket() == ng.output_sockets(n1)[0]);
      REQUIRE(ng.get_info(c1)->dst_socket() == ng.input_sockets(n2)[0]);
      REQUIRE(ng.get_info(c1)->src_node() == n1);
      REQUIRE(ng.get_info(c1)->dst_node() == n2);

      REQUIRE(ng.get_info(c2)->src_socket() == ng.output_sockets(n2)[0]);
      REQUIRE(ng.get_info(c2)->dst_socket() == ng.input_sockets(n3)[0]);
      REQUIRE(ng.get_info(c2)->src_node() == n2);
      REQUIRE(ng.get_info(c2)->dst_node() == n3);

      REQUIRE(ng.nodes(ng.root_group()).size() == 5);

      //
      // n1 -> [ -> n2 -> ] -> n3
      //
      auto g = ng.group(ng.root_group(), {n2});
      REQUIRE(g);
      REQUIRE(ng.exists(g));
      REQUIRE(ng.get_parent_group(g) == ng.root_group());
      REQUIRE(ng.get_group_members(g)[0] == n2);
      REQUIRE(!ng.exists(c1));
      REQUIRE(!ng.exists(c2));

      // root + n1,n2,n3 + g, g-IO(2), g-bits(2)
      REQUIRE(ng.node_graph().nodes().size() == size + 5);
      size += 5;

      auto ics = ng.input_connections(g);
      auto ocs = ng.output_connections(g);

      REQUIRE(ics.size() == 1);
      REQUIRE(ocs.size() == 1);

      REQUIRE(ng.get_info(ics[0])->src_socket() == ng.output_sockets(n1)[0]);
      REQUIRE(ng.get_info(ocs[0])->dst_socket() == ng.input_sockets(n3)[0]);

      REQUIRE(ng.get_info(ics[0])->src_node() == n1);
      REQUIRE(ng.get_info(ics[0])->dst_node() == g);
      REQUIRE(ng.get_info(ocs[0])->src_node() == g);
      REQUIRE(ng.get_info(ocs[0])->dst_node() == n3);

      REQUIRE(!ng.group(ng.root_group(), {n2}));

      REQUIRE(ng.nodes(g).size() == 3);
      REQUIRE(ng.nodes(ng.root_group()).size() == 5);

      //
      // n1 -> [[ -> n2 -> ]] -> n3
      //
      auto g1 = ng.group(ng.root_group(), {g});
      REQUIRE(g1);

      ics = ng.input_connections(n2);
      ocs = ng.output_connections(n2);
      REQUIRE(ics.size() == 1);
      REQUIRE(ocs.size() == 1);

      REQUIRE(ng.get_info(ng.output_connections(n1)[0])->src_node() == n1);
      REQUIRE(ng.get_info(ng.output_connections(n1)[0])->dst_node() == g1);
      REQUIRE(ng.get_info(ng.input_connections(n3)[0])->src_node() == g1);
      REQUIRE(ng.get_info(ng.input_connections(n3)[0])->dst_node() == n3);

      REQUIRE(ng.get_info(ng.input_connections(g1)[0])->src_node() == n1);
      REQUIRE(ng.get_info(ng.input_connections(g1)[0])->dst_node() == g1);
      REQUIRE(ng.get_info(ng.output_connections(g1)[0])->src_node() == g1);
      REQUIRE(ng.get_info(ng.output_connections(g1)[0])->dst_node() == n3);

      REQUIRE(ng.nodes(g1).size() == 3);
      REQUIRE(ng.nodes(g).size() == 3);
      REQUIRE(ng.nodes(ng.root_group()).size() == 5);

      // + 5
      REQUIRE(ng.node_graph().nodes().size() == size + 5);
      size += 5;

      //
      // n1 -> [[[ -> n2 -> ]]] -> n3
      //
      auto g2 = ng.group(g, {n2});
      REQUIRE(g2);

      ics = ng.input_connections(n2);
      ocs = ng.output_connections(n2);
      REQUIRE(ics.size() == 1);
      REQUIRE(ocs.size() == 1);

      // + 5
      REQUIRE(ng.node_graph().nodes().size() == size + 5);
      size += 5;

      // n1 -> n2 -> n3
      //
      ng.ungroup(g1);
      ics = ng.input_connections(n2);
      ocs = ng.output_connections(n2);
      REQUIRE(ics.size() == 1);
      REQUIRE(ocs.size() == 1);

      REQUIRE(ng.node_graph().nodes().size() == size - 5);
      size -= 5;

      ng.ungroup(g2);
      ics = ng.input_connections(n2);
      ocs = ng.output_connections(n2);
      REQUIRE(ics.size() == 1);
      REQUIRE(ocs.size() == 1);

      REQUIRE(ng.node_graph().nodes().size() == size - 5);
      size -= 5;

      ng.ungroup(g);
      REQUIRE(!ng.exists(g));
      REQUIRE(!ng.exists(g1));
      REQUIRE(!ng.exists(g2));
      REQUIRE(!ng.is_group(g));
      REQUIRE(!ng.is_group(g1));
      REQUIRE(!ng.is_group(g2));

      ics = ng.input_connections(n2);
      ocs = ng.output_connections(n2);
      REQUIRE(ics.size() == 1);
      REQUIRE(ocs.size() == 1);

      REQUIRE(ng.get_info(ics[0])->src_socket() == ng.output_sockets(n1)[0]);
      REQUIRE(ng.get_info(ocs[0])->dst_socket() == ng.input_sockets(n3)[0]);
    }

    SECTION("1")
    {
      auto n4 = ng.create(ng.root_group(), decl.name());
      auto n5 = ng.create(ng.root_group(), decl.name());
      auto n6 = ng.create(ng.root_group(), decl.name());

      auto n3_i0 = ng.input_sockets(n3)[0];
      auto n4_i0 = ng.input_sockets(n4)[0];
      auto n4_i1 = ng.input_sockets(n4)[1];
      auto n4_i2 = ng.input_sockets(n4)[2];
      auto n5_i0 = ng.input_sockets(n5)[0];
      auto n5_i1 = ng.input_sockets(n5)[1];
      auto n6_i0 = ng.input_sockets(n6)[0];

      auto n1_o0 = ng.output_sockets(n1)[0];
      auto n2_o0 = ng.output_sockets(n2)[0];
      auto n3_o0 = ng.output_sockets(n3)[0];
      auto n3_o1 = ng.output_sockets(n3)[1];
      auto n4_o0 = ng.output_sockets(n4)[0];
      auto n5_o0 = ng.output_sockets(n5)[0];

      REQUIRE(ng.connect(n1_o0, n3_i0));
      REQUIRE(ng.connect(n2_o0, n4_i0));
      REQUIRE(ng.connect(n2_o0, n5_i0));
      REQUIRE(ng.connect(n3_o0, n4_i1));
      REQUIRE(ng.connect(n3_o1, n5_i1));
      REQUIRE(ng.connect(n5_o0, n4_i2));
      REQUIRE(ng.connect(n4_o0, n6_i0));

      //
      // n1,n2 -> [  n3,n4,n5  ] -> n6
      //
      auto g = ng.group(ng.root_group(), {n3, n4, n5});
      REQUIRE(g);

      auto g_o = ng.get_group_output(g);
      auto g_i = ng.get_group_input(g);
      REQUIRE(g_o);
      REQUIRE(g_i);

      REQUIRE(ng.add_group_output_socket(g, "test"));
      REQUIRE(ng.output_sockets(g).size() == 2);
      REQUIRE(ng.input_sockets(g_o).size() == 2);

      REQUIRE(ng.connect(n4_o0, ng.input_sockets(g_o)[1]));

      REQUIRE(ng.add_group_input_socket(g, "test"));
      REQUIRE(ng.add_group_output_socket(g, "test"));

      REQUIRE(ng.get_info(ng.output_connections(n1)[0])->src_node() == n1);
      REQUIRE(ng.get_info(ng.output_connections(n1)[0])->dst_node() == g);
      REQUIRE(ng.get_info(ng.input_connections(n6)[0])->src_node() == g);
      REQUIRE(ng.get_info(ng.input_connections(n6)[0])->dst_node() == n6);

      //
      // n1,n2 -> [n3,n4,n5,n7] -> n6
      //
      auto n7 = ng.create(g, decl.name());
      REQUIRE(n7);

      auto n7_i0 = ng.input_sockets(n7)[0];
      auto n7_o0 = ng.output_sockets(n7)[0];

      REQUIRE(ng.connect(n2_o0, ng.input_sockets(g)[2]));
      REQUIRE(ng.connect(ng.output_sockets(g_i)[2], n7_i0));
      REQUIRE(ng.connect(n7_o0, ng.input_sockets(g_o)[2]));

      REQUIRE(!ng.connect(ng.output_sockets(g)[2], n6_i0));
      REQUIRE(!ng.connect(ng.output_sockets(g)[2], ng.input_sockets(n2)[0]));

      ng.ungroup(g);
      REQUIRE(ng.node_graph().nodes().size() == size + 4);
    }
  }
}

TEST_CASE("pos")
{
  managed_node_graph ng {};

  class X;

  auto decl = node_declaration(
    "node",
    {"0", "1", "2"},
    {"0", "1"},
    object_type<node_closure<X, X, X, X>>());

  REQUIRE(ng.register_node_decl(decl));

  auto g = ng.root_group();
  auto n = ng.create(g, "node");

  REQUIRE(ng.get_info(g)->pos() == tvec2<float> {});
  REQUIRE(*ng.get_pos(g) == tvec2<float> {});
  REQUIRE(ng.get_info(n)->pos() == tvec2<float> {});
  REQUIRE(*ng.get_pos(n) == tvec2<float> {});

  ng.set_pos(g, {4, 2});
  ng.set_pos(n, {2, 4});

  REQUIRE(ng.get_pos(g) == tvec2<float> {4, 2});
  REQUIRE(ng.get_pos(n) == tvec2<float> {2, 4});

  SECTION("clone")
  {
    auto ng2 = ng.clone();
    REQUIRE(ng2.get_pos(ng2.root_group()) == tvec2<float> {4, 2});
    REQUIRE(ng2.get_pos(ng2.root_group()) == tvec2<float> {4, 2});
  }
}