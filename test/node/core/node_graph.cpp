//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <catch2/catch.hpp>

#include <yave/node/core/node_graph.hpp>

using namespace yave;

template <class T>
bool vector_eq(const std::vector<T>& lhs, const std::vector<T>& rhs)
{
  if (lhs.size() != rhs.size())
    return false;

  for (auto&& l : lhs) {
    if (![&] {
          for (auto&& r : rhs) {
            if (l == r)
              return true;
          }
          return false;
        }())
      return false;
  }
  return true;
}

TEST_CASE("node_graph init")
{
  node_graph ng;

  REQUIRE(!ng.exists(node_handle()));
  REQUIRE(!ng.exists(connection_handle()));
  REQUIRE_NOTHROW(ng.remove(node_handle()));

  REQUIRE(ng.nodes().empty());
  REQUIRE(ng.nodes("").empty());

  REQUIRE(!ng.get_info(connection_handle()));
  REQUIRE(!ng.get_info(socket_handle()));
  REQUIRE(!ng.get_info(node_handle()));

  REQUIRE(!ng.get_name(socket_handle()));
  REQUIRE(!ng.get_name(node_handle()));

  REQUIRE(!ng.is_input_socket(socket_handle()));
  REQUIRE(!ng.is_output_socket(socket_handle()));

  REQUIRE(ng.input_connections(node_handle()).empty());
  REQUIRE(ng.output_connections(node_handle()).empty());
  REQUIRE(ng.connections(socket_handle()).empty());

  REQUIRE(ng.connections().empty());

  REQUIRE(ng.input_sockets(node_handle()).empty());
  REQUIRE(ng.output_sockets(node_handle()).empty());

  REQUIRE(!ng.is_primitive(node_handle()));
  REQUIRE(!ng.get_primitive(node_handle()));
  REQUIRE_NOTHROW(ng.set_primitive(node_handle(), {}));

  REQUIRE_NOTHROW(ng.clear());
}

TEST_CASE("node_graph control")
{
  node_graph ng;

  SECTION("add")
  {
    node_info info {"test node", {"input1", "input2"}, {"output1", "output2"}};

    auto n1 = ng.add(info);
    REQUIRE(n1);
    REQUIRE(ng.exists(n1));
    REQUIRE(ng.nodes().size() == 1);
    auto n2 = ng.add(info);
    REQUIRE(ng.exists(n2));
    REQUIRE(ng.nodes().size() == 2);

    node_info prim_info {"prim test node", {}, {"value"}, node_type::primitive};
    auto n3 = ng.add(prim_info);
    REQUIRE(prim_info.is_primitive());
    REQUIRE(ng.is_primitive(n3));
    REQUIRE(n3);
    REQUIRE(ng.exists(n3));
    REQUIRE(ng.nodes().size() == 3);

    auto i1 = ng.get_info(n1);
    auto i2 = ng.get_info(n2);
    auto i3 = ng.get_info(n3);
    REQUIRE(i1);
    REQUIRE(i2);
    REQUIRE(i3);

    REQUIRE(i1->name() == info.name());
    REQUIRE(i2->name() == info.name());
    REQUIRE(i3->name() == prim_info.name());

    REQUIRE(i1->input_sockets().size() == 2);
    REQUIRE(i1->output_sockets().size() == 2);

    REQUIRE(i3->is_primitive());
    REQUIRE(i3->input_sockets().empty());
    REQUIRE(i3->output_sockets().size() == 1);
  }

  SECTION("remove")
  {
    node_info info {"test node", {"input1", "input2"}, {"output1", "output2"}};

    auto n1 = ng.add(info);
    auto n2 = ng.add(info);
    auto n3 = ng.add(info);

    REQUIRE(ng.connect(ng.output_sockets(n1)[0], ng.input_sockets(n2)[0]));
    REQUIRE(ng.connect(ng.output_sockets(n2)[0], ng.input_sockets(n3)[0]));
    REQUIRE(!ng.connect(ng.output_sockets(n3)[0], ng.input_sockets(n1)[0]));

    REQUIRE(ng.connections().size() == 2);
    REQUIRE(ng.nodes("test node").size() == 3);

    ng.remove(n2);

    REQUIRE(ng.nodes("test node").size() == 2);
    REQUIRE(ng.connections(ng.input_sockets(n3)[0]).empty());
    REQUIRE(ng.connections(ng.output_sockets(n3)[0]).empty());
    REQUIRE(ng.connections(ng.output_sockets(n1)[0]).empty());
    REQUIRE(ng.connections(ng.input_sockets(n1)[0]).empty());

    ng.remove(n1);
    REQUIRE(ng.nodes("test node").size() == 1);
    REQUIRE(ng.connections(ng.input_sockets(n3)[0]).empty());
    REQUIRE(ng.connections(ng.output_sockets(n3)[0]).empty()); //???

    ng.remove(n3);
    REQUIRE(ng.nodes("test node").empty());
  }

  SECTION("connect")
  {
    node_info info {"test node", {"input"}, {"output"}};
    auto n1 = ng.add(info);
    auto n2 = ng.add(info);
    REQUIRE(n1);
    REQUIRE(n2);

    auto n1_i = ng.input_sockets(n1)[0];
    auto n1_o = ng.output_sockets(n1)[0];
    auto n2_i = ng.input_sockets(n2)[0];
    auto n2_o = ng.output_sockets(n2)[0];

    REQUIRE(ng.connect(n1_o, n2_i));
    REQUIRE(ng.connect(n1_o, n2_i));
    REQUIRE(ng.connect(n1_o, n2_i) == ng.connect(n1_o, n2_i));

    REQUIRE(ng.connections().size() == 1);

    REQUIRE(!ng.has_connection(ng.input_sockets(n1)[0]));
    REQUIRE(ng.has_connection(ng.output_sockets(n1)[0]));
    REQUIRE(!ng.has_connection(ng.output_sockets(n2)[0]));
    REQUIRE(ng.has_connection(ng.input_sockets(n2)[0]));

    // multiple input
    auto n3 = ng.add(info);
    REQUIRE(n3);
    auto n3_i = ng.input_sockets(n3)[0];
    auto n3_o = ng.output_sockets(n3)[0];

    REQUIRE(!ng.connect(n3_o, n2_i));

    // self connect
    REQUIRE(!ng.connect(n1_o, n1_i));
  }

  SECTION("disconnect")
  {
    node_info info {"test node", {"input"}, {"output"}};
    auto n1 = ng.add(info);
    auto n2 = ng.add(info);

    REQUIRE(n1);
    REQUIRE(n2);

    auto n1_i = ng.input_sockets(n1)[0];
    auto n1_o = ng.output_sockets(n1)[0];
    auto n2_i = ng.input_sockets(n2)[0];
    auto n2_o = ng.output_sockets(n2)[0];

    auto c = ng.connect(n1_o, n2_i);
    REQUIRE(c);

    ng.disconnect(c);
    REQUIRE(!ng.has_connection(ng.output_sockets(n1)[0]));
    REQUIRE(!ng.has_connection(ng.input_sockets(n2)[0]));

    REQUIRE(ng.connect(n1_o, n2_i));
  }

  SECTION("find node")
  {
    node_info info1 {"test node", {"input"}, {"output"}};
    node_info info2 {"test node2", {"input", "input2"}, {"output"}};
    auto n1 = ng.add(info1);
    auto n2 = ng.add(info1);
    auto n3 = ng.add(info2);
    REQUIRE(n1);
    REQUIRE(n2);
    REQUIRE(n3);

    REQUIRE(ng.nodes("test node").size() == 2);
    REQUIRE(ng.nodes("test node2").size() == 1);
  }

  SECTION("is_connected")
  {
    SECTION("1")
    {
      node_info info {"test node", {"input"}, {"output"}};
      auto n1 = ng.add(info);
      auto n2 = ng.add(info);
      REQUIRE(ng.connect(ng.output_sockets(n1)[0], ng.input_sockets(n2)[0]));

      REQUIRE(ng.has_connection(ng.output_sockets(n1)[0]));
      REQUIRE(ng.has_connection(ng.input_sockets(n2)[0]));
    }
    SECTION("2")
    {
      node_info info1 {"test node", {}, {"output"}};
      node_info info2 {"test node", {"input"}, {"output"}};

      auto n1 = ng.add(info1);
      auto n2 = ng.add(info2);

      REQUIRE(ng.connect(ng.output_sockets(n1)[0], ng.input_sockets(n2)[0]));

      REQUIRE(ng.has_connection(ng.output_sockets(n1)[0]));
      REQUIRE(ng.has_connection(ng.input_sockets(n2)[0]));
      REQUIRE(!ng.has_connection(ng.output_sockets(n2)[0]));
      REQUIRE(!ng.has_connection(
        socket_handle((socket_handle::descriptor_type)0xdeadbeef, uid {42})));
    }
  }

  SECTION("io sockets")
  {
    node_info info1 {"test node", {}, {"output"}};
    node_info info2 {"test node", {"input"}, {"output", "2"}};

    auto n1 = ng.add(info1);
    auto n2 = ng.add(info2);

    REQUIRE(ng.input_sockets(n1).size() == 0);
    REQUIRE(ng.output_sockets(n1).size() == 1);

    REQUIRE(ng.input_sockets(n2).size() == 1);
    REQUIRE(ng.output_sockets(n2).size() == 2);
  }

  SECTION("connections")
  {
    node_info info1 {"test node", {"input"}, {"output"}};
    node_info info2 {
      "test node test node", {"input1", "input2"}, {"output1", "output2"}};

    auto n1 = ng.add(info1);
    auto n2 = ng.add(info2);

    auto n1_i = ng.input_sockets(n1)[0];
    auto n1_o = ng.output_sockets(n1)[0];

    auto n2_i1 = ng.input_sockets(n2)[0];
    auto n2_i2 = ng.input_sockets(n2)[1];
    auto n2_o1 = ng.output_sockets(n2)[0];
    auto n2_o2 = ng.output_sockets(n2)[1];

    auto c1 = ng.connect(n1_o, n2_i1); // n1.out -> n2.in[0]
    auto c2 = ng.connect(n2_o1, n1_i); // loop
    auto c3 = ng.connect(n1_o, n2_i2); // n1.out -> n2.in[1]
    auto c4 = ng.connect(n1_i, n2_i1); // invalid

    REQUIRE(c1);
    REQUIRE(!c2);
    REQUIRE(c3);
    REQUIRE(!c4);

    REQUIRE(ng.connections(n1_i).size() == 0);
    REQUIRE(ng.connections(n1_o).size() == 2);

    REQUIRE(ng.connections(n2_i1).size() == 1);
    REQUIRE(ng.connections(n2_i2).size() == 1);
    REQUIRE(ng.connections(n2_o1).size() == 0);
    REQUIRE(ng.connections(n2_o2).size() == 0);
  }

  SECTION("root_of")
  {
    node_info info11 {"test11", {"input"}, {"output"}};

    REQUIRE(ng.root_of(node_handle()).empty());

    auto n1   = ng.add(info11);
    auto n1_i = ng.input_sockets(n1)[0];
    auto n1_o = ng.output_sockets(n1)[0];

    // n1
    REQUIRE(ng.root_of(n1).size() == 1);
    REQUIRE(ng.root_of(n1)[0] == n1);

    // n1 -> n2
    auto n2   = ng.add(info11);
    auto n2_i = ng.input_sockets(n2)[0];
    auto n2_o = ng.output_sockets(n2)[0];

    auto c1 = ng.connect(n1_o, n2_i);
    REQUIRE(c1);
    REQUIRE(ng.root_of(n1).size() == 1);
    REQUIRE(ng.root_of(n1)[0] == n2);

    node_info info22 {"test22", {"input1", "input2"}, {"output1", "output2"}};

    // n3 -> n4,n5
    auto n3 = ng.add(info22);
    auto n4 = ng.add(info11);
    auto n5 = ng.add(info11);

    auto n3_i1 = ng.input_sockets(n3)[0];
    auto n3_i2 = ng.input_sockets(n3)[1];
    auto n3_o1 = ng.output_sockets(n3)[0];
    auto n3_o2 = ng.output_sockets(n3)[1];

    auto n4_i = ng.input_sockets(n4)[0];
    auto n4_o = ng.output_sockets(n4)[0];

    auto n5_i = ng.input_sockets(n5)[0];
    auto n5_o = ng.output_sockets(n5)[0];

    auto c3 = ng.connect(n3_o1, n4_i);
    auto c4 = ng.connect(n3_o2, n5_i);

    REQUIRE(c3);
    REQUIRE(c4);
    REQUIRE(ng.root_of(n3).size() == 2);
    REQUIRE(vector_eq(ng.root_of(n3), std::vector {n4, n5}));

    // n3 -> n4,n5 -> n6
    auto n6    = ng.add(info22);
    auto n6_i1 = ng.input_sockets(n6)[0];
    auto n6_i2 = ng.input_sockets(n6)[1];

    auto c5 = ng.connect(n4_o, n6_i1);
    auto c6 = ng.connect(n5_o, n6_i2);
    REQUIRE(c5);
    REQUIRE(c6);
    REQUIRE(ng.root_of(n3).size() == 1);
    REQUIRE(ng.root_of(n3)[0] == n6);
  }

  SECTION("roots")
  {
    auto info11 = node_info {"test11", {"input"}, {"output"}};
    auto info22 =
      node_info {"test22", {"input1", "input2"}, {"output1", "output2"}};

    auto n1 = ng.add(info11);
    auto n2 = ng.add(info11);

    auto n1_i = ng.input_sockets(n1)[0];
    auto n1_o = ng.output_sockets(n1)[0];
    auto n2_i = ng.input_sockets(n2)[0];
    auto n2_o = ng.output_sockets(n2)[0];

    auto n3 = ng.add(info22);
    auto n4 = ng.add(info22);

    auto n3_i1 = ng.input_sockets(n3)[0];
    auto n3_i2 = ng.input_sockets(n3)[1];
    auto n3_o1 = ng.output_sockets(n3)[0];
    auto n3_o2 = ng.output_sockets(n3)[1];

    auto n4_i1 = ng.input_sockets(n4)[0];
    auto n4_i2 = ng.input_sockets(n4)[1];
    auto n4_o1 = ng.output_sockets(n4)[0];
    auto n4_o2 = ng.output_sockets(n4)[1];

    REQUIRE(vector_eq(ng.roots(), std::vector {n1, n2, n3, n4}));

    auto c1 = ng.connect(n1_o, n2_i);
    REQUIRE(c1);
    REQUIRE(vector_eq(ng.roots(), std::vector {n2, n3, n4}));

    auto c2 = ng.connect(n1_o, n3_i1);
    REQUIRE(c2);
    REQUIRE(vector_eq(ng.roots(), std::vector {n2, n3, n4}));

    auto c3 = ng.connect(n1_o, n3_i2);
    REQUIRE(c3);
    REQUIRE(vector_eq(ng.roots(), std::vector {n2, n3, n4}));

    auto c4 = ng.connect(n2_o, n4_i1);
    auto c5 = ng.connect(n3_o1, n4_i2);
    REQUIRE(c4);
    REQUIRE(c5);
    REQUIRE(vector_eq(ng.roots(), std::vector {n4}));
  }

  SECTION("dfs")
  {
    auto info1 = node_info("n1", {"0", "1", "2"}, {"0"}, node_type::primitive);
    auto info2 = node_info("n2", {}, {"0"}, node_type::primitive);

    auto n1 = ng.add(info1, 1);
    auto n2 = ng.add(info2, 2);
    auto n3 = ng.add(info2);

    auto c1 = ng.connect(ng.output_sockets(n2)[0], ng.input_sockets(n1)[0]);
    auto c2 = ng.connect(ng.output_sockets(n2)[0], ng.input_sockets(n1)[2]);
    auto c3 = ng.connect(ng.output_sockets(n3)[0], ng.input_sockets(n1)[1]);

    REQUIRE(c1);
    REQUIRE(c2);
    REQUIRE(c3);

    int count = 0;
    ng.depth_first_search(n1, [&](auto&&, auto&&) { ++count; });

    REQUIRE(count == 3);
  }
}

TEST_CASE("node_graph interface")
{
  node_graph ng;

  node_info info1 = {"1", {"0"}, {"0"}};
  node_info info2 = {"2", {"0", "1"}, {"0"}};

  auto n1 = ng.add(info1);
  auto n2 = ng.add(info2);
  REQUIRE(n1);
  REQUIRE(n2);

  auto n1_i0 = ng.input_sockets(n1)[0];
  auto n1_o0 = ng.output_sockets(n1)[0];

  auto n2_i0 = ng.input_sockets(n2)[0];
  auto n2_i1 = ng.input_sockets(n2)[1];
  auto n2_o0 = ng.output_sockets(n2)[0];

  SECTION("init")
  {
    auto interface = ng.add({{"i"}, {}, {}, node_type::interface});

    REQUIRE(interface);
    REQUIRE(ng.exists(interface));
    REQUIRE(ng.get_info(interface));
    REQUIRE(ng.get_info(interface)->name() == "i");
    REQUIRE(ng.get_info(interface)->input_sockets().empty());
    REQUIRE(ng.get_info(interface)->output_sockets().empty());
    REQUIRE(ng.get_info(interface)->type() == node_type::interface);

    REQUIRE(ng.is_interface(interface));
    REQUIRE(!ng.is_primitive(interface));

    REQUIRE(ng.input_connections(interface).empty());
    REQUIRE(ng.output_connections(interface).empty());
    ng.remove(interface);

    REQUIRE(!ng.exists(interface));
  }

  SECTION("attach")
  {
    auto i1 = ng.add({"i1", {}, {}, node_type::interface});

    REQUIRE(ng.attach_interface(i1, n1_i0));
    REQUIRE(ng.attach_interface(i1, n1_i0));
    REQUIRE(ng.get_info(i1)->input_sockets().size() == 1);
    REQUIRE(ng.get_info(i1)->output_sockets().empty());
    REQUIRE(ng.get_interfaces(n1_i0)[0] == i1);
    REQUIRE(ng.get_interfaces(n1_o0).empty());

    REQUIRE(ng.attach_interface(i1, n1_o0));
    REQUIRE(ng.attach_interface(i1, n1_o0));
    REQUIRE(ng.get_info(i1)->input_sockets().size() == 1);
    REQUIRE(ng.get_info(i1)->output_sockets().size() == 1);
    REQUIRE(ng.get_interfaces(n1_o0)[0] == i1);

    REQUIRE(ng.input_sockets(i1).size() == 1);
    REQUIRE(ng.output_sockets(i1).size() == 1);

    REQUIRE(ng.input_connections(i1).empty());
    REQUIRE(ng.output_connections(i1).empty());

    SECTION("detach")
    {
      ng.detach_interface(i1, n1_i0);
      ng.detach_interface(i1, n1_i0);
      ng.detach_interface(i1, n1_o0);
      ng.detach_interface(i1, n1_o0);

      REQUIRE(ng.input_sockets(i1).empty());
      REQUIRE(ng.output_sockets(i1).empty());

      REQUIRE(ng.get_interfaces(n1_i0).empty());
      REQUIRE(ng.get_interfaces(n1_o0).empty());
    }

    SECTION("remove")
    {
      ng.remove(n1);

      // automatically detached
      REQUIRE(ng.exists(i1));
      REQUIRE(ng.input_sockets(i1).empty());
      REQUIRE(ng.output_sockets(i1).empty());
      REQUIRE(ng.get_interfaces(n1_i0).empty());
      REQUIRE(ng.get_interfaces(n1_o0).empty());
    }

    SECTION("conn")
    {
      REQUIRE(ng.connect(n1_o0, n2_i0));
      REQUIRE(ng.output_connections(n1).size() == 1);
      auto ci = ng.get_info(ng.output_connections(n1)[0]);
      REQUIRE(ng.exists(ng.output_connections(n1)[0]));
      REQUIRE(ci);
      REQUIRE(ci->src_node() == n1);
      REQUIRE(ci->dst_node() == n2);
      REQUIRE(ci->src_interfaces()[0] == i1);
      REQUIRE(ci->dst_interfaces().empty());
    }
  }
}