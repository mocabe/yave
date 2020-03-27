//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <catch2/catch.hpp>
#include <yave/app/node_compiler_thread.hpp>

using namespace yave;
using namespace yave::app;

TEST_CASE("node_compiler_thread")
{
  node_compiler_thread th;

  REQUIRE(!th.is_running());
  REQUIRE(!th.is_compiling());
  REQUIRE(!th.get_last_result()->success);

  SECTION("")
  {
    th.start();
    REQUIRE(th.is_running());
    REQUIRE(!th.is_compiling());
    th.stop();
    REQUIRE(!th.is_running());
    REQUIRE(!th.is_compiling());
  }

  SECTION("")
  {
    th.start();
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
