// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <catch2/catch.hpp>

#include <yave/node/core/managed_node_graph.hpp>

using namespace yave;

TEST_CASE("root group")
{
  managed_node_graph ng;
  auto root = ng.root_group();
  REQUIRE(root);
  REQUIRE(ng.is_group(root));
  REQUIRE(ng.get_group_members(root).empty());
  REQUIRE(!ng.is_group_output(root));
  REQUIRE(!ng.is_group_input(root));
  REQUIRE(!ng.get_parent_group(root));
  REQUIRE(!ng.get_group_input(root));
  REQUIRE(!ng.get_group_output(root));

  ng.destroy(ng.root_group());
  REQUIRE(ng.is_group(root));
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

  auto i = ng.get_group_input(g);
  auto o = ng.get_group_output(g);

  REQUIRE(i);
  REQUIRE(o);

  REQUIRE(ng.is_group_input(i));
  REQUIRE(ng.is_group_output(o));

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

  auto info = node_info {"node", {"0", "1", "2"}, {"0", "1"}};

  REQUIRE(ng.register_node_info(info));

  SECTION("simple")
  {
    auto n = ng.create(ng.root_group(), info.name());
    REQUIRE(n);
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
    auto n1 = ng.create(ng.root_group(), info.name());
    auto n2 = ng.create(ng.root_group(), info.name());
    auto n3 = ng.create(ng.root_group(), info.name());

    // root + 3
    REQUIRE(ng.get_node_graph().nodes().size() == 4);

    SECTION("0")
    {
      //
      // n1 -> n2 -> n3
      //
      auto c1 = ng.connect(ng.output_sockets(n1)[0], ng.input_sockets(n2)[0]);
      auto c2 = ng.connect(ng.output_sockets(n2)[0], ng.input_sockets(n3)[0]);
      REQUIRE(c1);
      REQUIRE(c2);


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
      REQUIRE(ng.get_node_graph().nodes().size() == 9);

      auto ics = ng.input_connections(g);
      auto ocs = ng.output_connections(g);

      REQUIRE(ics.size() == 1);
      REQUIRE(ocs.size() == 1);

      REQUIRE(ng.get_info(ics[0])->src_socket() == ng.output_sockets(n1)[0]);
      REQUIRE(ng.get_info(ocs[0])->dst_socket() == ng.input_sockets(n3)[0]);

      REQUIRE(!ng.group(ng.root_group(), {n2}));

      //
      // n1 -> [[ -> n2 -> ]] -> n3
      //
      auto g1 = ng.group(ng.root_group(), {g});
      REQUIRE(g1);

      ics = ng.input_connections(n2);
      ocs = ng.output_connections(n2);
      REQUIRE(ics.size() == 1);
      REQUIRE(ocs.size() == 1);

      // 9 + 5
      REQUIRE(ng.get_node_graph().nodes().size() == 14);

      //
      // n1 -> [[[ -> n2 -> ]]] -> n3
      //
      auto g2 = ng.group(g, {n2});
      REQUIRE(g2);

      ics = ng.input_connections(n2);
      ocs = ng.output_connections(n2);
      REQUIRE(ics.size() == 1);
      REQUIRE(ocs.size() == 1);

      // 14 + 5
      REQUIRE(ng.get_node_graph().nodes().size() == 19);

      // n1 -> n2 -> n3
      //
      ng.ungroup(g1);
      ics = ng.input_connections(n2);
      ocs = ng.output_connections(n2);
      REQUIRE(ics.size() == 1);
      REQUIRE(ocs.size() == 1);

      REQUIRE(ng.get_node_graph().nodes().size() == 14);

      ng.ungroup(g2);
      ics = ng.input_connections(n2);
      ocs = ng.output_connections(n2);
      REQUIRE(ics.size() == 1);
      REQUIRE(ocs.size() == 1);

      REQUIRE(ng.get_node_graph().nodes().size() == 9);

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
      auto n4    = ng.create(ng.root_group(), info.name());
      auto n5    = ng.create(ng.root_group(), info.name());
      auto n6    = ng.create(ng.root_group(), info.name());

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

      // n1,n2 -> [  n3,n4,n5  ] -> n6
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

      // n1,n2 -> [n3,n4,n5,n7] -> n6
      auto n7 = ng.create(g, info.name());
      REQUIRE(n7);

      auto n7_i0 = ng.input_sockets(n7)[0];
      auto n7_o0 = ng.output_sockets(n7)[0];

      REQUIRE(ng.connect(n2_o0, ng.input_sockets(g)[2]));
      REQUIRE(ng.connect(ng.output_sockets(g_i)[2], n7_i0));
      REQUIRE(ng.connect(n7_o0, ng.input_sockets(g_o)[2]));

      REQUIRE(!ng.connect(ng.output_sockets(g)[2], n6_i0));
      REQUIRE(!ng.connect(ng.output_sockets(g)[2], ng.input_sockets(n2)[0]));

      ng.ungroup(g);
      REQUIRE(ng.get_node_graph().nodes().size() == 8);
    }
  }
}