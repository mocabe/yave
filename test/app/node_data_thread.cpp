//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <catch2/catch.hpp>

#include <yave/app/node_data_thread.hpp>

using namespace yave;
using namespace yave::app;

TEST_CASE("node_data_thread")
{
  auto ng = managed_node_graph();
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
      th.send([](managed_node_graph&) {});
      th.start();
      th.stop();
      th.send([](managed_node_graph&) {});
      th.start();
      th.stop();
    }
  }

  SECTION("wait")
  {
    th.start();
    auto t = std::chrono::steady_clock::now();
    th.send([](managed_node_graph& g) {
      (void)g.create(g.root_group(), node_declaration("", {}, {}, {}, {}));
    });
    th.wait_update(t);
    th.stop();
  }
}