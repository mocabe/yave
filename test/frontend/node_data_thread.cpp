//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <catch2/catch.hpp>

#include <yave/frontend/node_data_thread.hpp>

using namespace yave;

TEST_CASE("node_data_thread")
{
  auto ng = std::make_shared<managed_node_graph>();
  node_data_thread th {ng};

  SECTION("start/stop")
  {
    SECTION("")
    {
      th.start();
      th.stop();
    }
    SECTION("")
    {
      th.start();
      // dtor
    }
    SECTION("")
    {
      th.start();
      REQUIRE_THROWS_AS(th.start(), std::runtime_error);
      th.stop();
      th.start();
      REQUIRE_THROWS_AS(th.start(), std::runtime_error);
    }
    SECTION("")
    {
      REQUIRE_THROWS_AS(th.stop(), std::runtime_error);
      th.start();
      th.stop();
      REQUIRE_THROWS_AS(th.stop(), std::runtime_error);
    }
  }

  SECTION("op")
  {
    SECTION("")
    {
      th.send(node_data_thread_op_nop {});
      th.start();
      th.stop();
      th.send(node_data_thread_op_nop {});
      th.start();
      th.stop();
    }
  }

  SECTION("wait")
  {
    th.start();
    auto t = std::chrono::steady_clock::now();
    th.send(node_data_thread_op_create {});
    th.wait_update(t);
    th.stop();
  }
}