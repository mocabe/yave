//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/core/unique_any.hpp>
#include <catch2/catch.hpp>

using namespace yave;

TEST_CASE("unique_any")
{
  unique_any data(42);

  REQUIRE(!data.empty());
  REQUIRE(data.type() == typeid(int));
  REQUIRE(*static_cast<int*>(data.data()) == 42);

  unique_any data2 = std::move(data);
  REQUIRE(data.empty());
  REQUIRE(data.type() == typeid(void));
  REQUIRE(data2.type() == typeid(int));
  REQUIRE(*static_cast<int*>(data2.data()) == 42);
}

TEST_CASE("unique_any_cast")
{
  SECTION("empty")
  {
    unique_any data;

    REQUIRE(data.empty());
    REQUIRE(!data.data());
    REQUIRE(data.type() == typeid(void));
    REQUIRE(!unique_any_cast<void>(&data));
    // REQUIRE_THROWS(unique_any_cast<void>(data)); should not compile
  }

  SECTION("ref")
  {
    unique_any data(42);

    REQUIRE(unique_any_cast<int>(&data));
    REQUIRE(unique_any_cast<const int>(&data));
    REQUIRE(!unique_any_cast<double>(&data));
    REQUIRE(unique_any_cast<int>(data));
    REQUIRE(unique_any_cast<const int>(data));
    REQUIRE_THROWS(unique_any_cast<double>(data));
  }

  SECTION("cref")
  {
    unique_any data(42);

    REQUIRE(unique_any_cast<int>(&data));
    REQUIRE(unique_any_cast<const int>(&data));
    REQUIRE(!unique_any_cast<double>(&data));
    REQUIRE(unique_any_cast<int>(data));
    REQUIRE(unique_any_cast<const int>(data));
    REQUIRE_THROWS(unique_any_cast<double>(data));
  }
}
