//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <catch2/catch.hpp>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_sinks.h>

#include <yave/node/node_graph.hpp>

using namespace yave;

TEST_CASE("Init")
  {
  // Runtime log levels
  spdlog::set_level(spdlog::level::info);
  // create logger
  static auto console = spdlog::stderr_logger_mt("stderr");
}

TEST_CASE("NodeGraph control")
{
  NodeGraph ng;


  SECTION("add")
  {
    NodeInfo info {
      "test node", {"input1", "input2"}, {"output1", "output2"}, false};

    auto n1 = ng.add_node(info);
    REQUIRE(n1);
    REQUIRE(ng.exists(n1));
    REQUIRE(ng.nodes().size() == 1);
    auto n2 = ng.add_node(info);
    REQUIRE(ng.exists(n2));
    REQUIRE(ng.nodes().size() == 2);

    NodeInfo prim_info {"prim test node", {}, {"value"}, true};
    auto n3 = ng.add_node(prim_info);
    REQUIRE(n3);
    REQUIRE(ng.exists(n3));
    REQUIRE(ng.nodes().size() == 3);

    auto i1 = ng.node_info(n1);
    auto i2 = ng.node_info(n2);
    auto i3 = ng.node_info(n3);
    REQUIRE(i1);
    REQUIRE(i2);
    REQUIRE(i3);

    REQUIRE(i1->name() == info.name());
    REQUIRE(i2->name() == info.name());
    REQUIRE(i3->name() == prim_info.name());

    REQUIRE(i1->input_sockets().size() == 2);
    REQUIRE(i1->output_sockets().size() == 2);

    REQUIRE(i3->is_prim());
    REQUIRE(i3->input_sockets().empty());
    REQUIRE(i3->output_sockets().size() == 1);
  }

  SECTION("remove")
  {
    NodeInfo info {
      "test node", {"input1", "input2"}, {"output1", "output2"}, false};

    auto n1 = ng.add_node(info);
    auto n2 = ng.add_node(info);
    auto n3 = ng.add_node(info);

    REQUIRE(ng.connect(n1, "output1", n2, "input1"));
    REQUIRE(ng.connect(n2, "output1", n3, "input1"));
    REQUIRE(ng.connect(n3, "output1", n1, "input1"));

    REQUIRE(ng.nodes("test node").size() == 3);

    ng.remove_node(n2);

    REQUIRE(ng.nodes("test node").size() == 2);
    REQUIRE(ng.input_connections(n3, "intput1").empty());
    REQUIRE_FALSE(ng.output_connections(n3, "output1").empty());
    REQUIRE(ng.output_connections(n1, "output1").empty());
    REQUIRE_FALSE(ng.input_connections(n1, "input1").empty());

    ng.remove_node(n1);
    REQUIRE(ng.nodes("test node").size() == 1);
    REQUIRE(ng.input_connections(n3, "intput1").empty());
    REQUIRE(ng.input_connections(n3, "output1").empty());

    ng.remove_node(n3);
    REQUIRE(ng.nodes("test node").empty());
  }

  SECTION("connect")
  {
    NodeInfo info {"test node", {"input"}, {"output"}, false};
    auto n1 = ng.add_node(info);
    auto n2 = ng.add_node(info);
    REQUIRE(n1);
    REQUIRE(n2);
    ConnectionInfo ci {n1, "output", n2, "input"};
    REQUIRE(ng.connect(ci));
    REQUIRE(ng.connect(ci));
    REQUIRE(ng.connect(ci) == ng.connect(ci));

    REQUIRE(!ng.is_connected(n1, "input"));
    REQUIRE(ng.is_connected(n1, "output"));
    REQUIRE(!ng.is_connected(n2, "output"));
    REQUIRE(ng.is_connected(n2, "input"));

    // multiple input
    auto n3 = ng.add_node(info);
    REQUIRE(n3);
    ConnectionInfo connectInfo2 {n3, "output", n2, "input"};
    REQUIRE(!ng.connect(connectInfo2));

    // self connect
    ConnectionInfo connectInfo3 {n1, "output", n1, "input"};
    REQUIRE(!ng.connect(connectInfo3));
  }

  SECTION("disconnect")
  {
    NodeInfo info {"test node", {"input"}, {"output"}, false};
    auto n1 = ng.add_node(info);
    auto n2 = ng.add_node(info);

    ConnectionInfo cinfo {n1, "output", n2, "input"};
    auto c = ng.connect(cinfo);
    REQUIRE(c);

    ng.disconnect(c);
    REQUIRE(!ng.is_connected(n1, "output"));
    REQUIRE(!ng.is_connected(n2, "input"));

    REQUIRE(ng.connect(cinfo));
  }

  SECTION("find node")
  {
    NodeInfo info1 {"test node", {"input"}, {"output"}, false};
    NodeInfo info2 {"test node2", {"input", "input2"}, {"output"}, false};
    auto n1 = ng.add_node(info1);
    auto n2 = ng.add_node(info1);
    auto n3 = ng.add_node(info2);
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
      NodeInfo info {"test node", {"input"}, {"output"}, false};
      auto n1 = ng.add_node(info);
      auto n2 = ng.add_node(info);
      REQUIRE(ng.connect(n1, "output", n2, "input"));

      REQUIRE(ng.is_connected(n1, "output"));
      REQUIRE(ng.is_connected(n2, "input"));
    }
    SECTION("2")
    {
      NodeInfo info1 {"test node", {}, {"output"}, true};
      NodeInfo info2 {"test node", {"input"}, {"output"}, false};
      auto n1 = ng.add_node(info1);
      auto n2 = ng.add_node(info2);

      REQUIRE(ng.connect(n1, "output", n2, "input"));

      REQUIRE(ng.is_connected(n1, "output"));
      REQUIRE(ng.is_connected(n2, "input"));
      REQUIRE(!ng.is_connected(n2, "output"));
      REQUIRE(!ng.is_connected(n1, "nonexist"));
    }
  }

  SECTION("io sockets")
  {
    NodeInfo info1 {"test node", {}, {"output"}, true};
    NodeInfo info2 {"test node", {"input"}, {"output", "2"}, false};
    auto n1 = ng.add_node(info1);
    auto n2 = ng.add_node(info2);

    REQUIRE(ng.input_sockets(n1).size() == 0);
    REQUIRE(ng.output_sockets(n1).size() == 1);

    REQUIRE(ng.input_sockets(n2).size() == 1);
    REQUIRE(ng.output_sockets(n2).size() == 2);
  }

  SECTION("connections")
  {
    NodeInfo info1 {"test node", {"input"}, {"output"}, false};
    NodeInfo info2 {
      "test node test node", {"input", "input2"}, {"output", "input2"}, false};

    auto n1 = ng.add_node(info1);
    auto n2 = ng.add_node(info2);

    auto c1 = ng.connect(n1, "output", n2, "input");
    auto c2 = ng.connect(n2, "output", n1, "input");
    auto c3 = ng.connect(n1, "output", n2, "input2");

    REQUIRE(c1);
    REQUIRE(c2);
    REQUIRE(c3);

    REQUIRE(ng.connections(n1, "output").size() == 2);
    REQUIRE(ng.connections(n1, "input").size() == 1);
    REQUIRE(ng.connections(n2, "output").size() == 1);
    REQUIRE(ng.connections(n2, "input").size() == 1);
    REQUIRE(ng.connections(n2, "output2").empty());
  }
}