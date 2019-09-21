//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/lib/vector/vector.hpp>
#include <catch2/catch.hpp>

#include <yave/lib/string/string.hpp>

using namespace yave;

TEST_CASE("init", "[lib][vector]")
{
  vector<int>();
  vector<int>(1);
  vector<int>(1, 42);
  vector<int>(std::vector<int>());

  vector<string>();
  vector<string>(1);
  vector<string>(1, "Hello, World");
  vector<string>(std::vector<string>(1, "Hello, World"));

  vector<vector<string>>();
  vector<vector<string>>(1);
  vector<vector<string>>(1, vector<string>(1, "Hello, World"));
}

TEST_CASE("assign", "[lib][vector]")
{
  vector<int> v0(42);
  vector<int> v1(24);

  SECTION("0")
  {
    v0 = v1;
    REQUIRE(v0.size() == 24);
    REQUIRE(v0.size() == 24);
  }

  SECTION("1")
  {
    v0 = std::move(v1);
    REQUIRE(v0.size() == 24);
    REQUIRE(v1.size() == 0);
  }
}

TEST_CASE("self assign", "[lib][vector]")
{
  SECTION("cp")
  {
    vector<int> v = v;
    REQUIRE(v.empty());
  }

  SECTION("mv")
  {
    vector<int> v = std::move(v);
    REQUIRE(v.empty());
  }
}

TEST_CASE("empty", "[lib][vector]")
{
  auto v0 = vector<int>();
  REQUIRE(v0.empty());
  REQUIRE(std::vector<int>(v0.begin(), v0.end()).empty());
  REQUIRE(v0.size() == 0);
  REQUIRE(v0.begin() == v0.end());
  REQUIRE(v0.cend()  == v0.cend());

  auto v1 = v0;
  REQUIRE(v1.empty());
  REQUIRE(v1.size() == 0);
  REQUIRE(v1.begin() == v1.end());
  REQUIRE(v1.cend()  == v1.cend());

  auto v2 = std::move(v1);
  REQUIRE(v2.empty());
  REQUIRE(v2.size() == 0);
  REQUIRE(v2.begin() == v2.end());
  REQUIRE(v2.cend()  == v2.cend());
}

TEST_CASE("42", "[lib][vector]")
{
  auto v0 = vector<int>(42);
  REQUIRE(!v0.empty());
  REQUIRE(v0.size() == 42);
  REQUIRE(std::vector<int>(v0.begin(), v0.end()).size() == 42);

  auto v1 = v0;
  REQUIRE(!v0.empty());
  REQUIRE(!v1.empty());
  REQUIRE(v0.size() == 42);
  REQUIRE(v1.size() == 42);

  auto v2 = std::move(v1);
  REQUIRE(v1.empty());
  REQUIRE(!v2.empty());
  REQUIRE(v1.size() == 0);
  REQUIRE(v2.size() == 42);
}

TEST_CASE("access", "[lib][vector]")
{
  auto v = vector<int>(3);

  REQUIRE(v[0] == 0);
  REQUIRE(v[1] == 0);
  REQUIRE(v[2] == 0);

  for (auto& e : v) {
    REQUIRE(e == 0);
    e = 0;
  }

  const auto& ref = v;

  for (auto& e : v) {
    REQUIRE(e == 0);
  }

  v[0] = 1;
  v[1] = 2;
  v[2] = 3;

  REQUIRE(v[0] == 1);
  REQUIRE(v[1] == 2);
  REQUIRE(v[2] == 3);

  REQUIRE(v.at(0) == 1);
  REQUIRE(v.at(1) == 2);
  REQUIRE(v.at(2) == 3);

  REQUIRE(v.front() == 1);
  REQUIRE(v.back() == 3);

  REQUIRE_NOTHROW(v.at(0));
  REQUIRE_NOTHROW(v.at(2));
  REQUIRE_THROWS_AS(v.at(3), std::out_of_range);
}

TEST_CASE("conv", "[lib][vector]")
{
  std::vector<string> str = {"Hello", "World", "!"};
  vector<string> v        = str;
  REQUIRE(v[0] == "Hello");
  REQUIRE(v[1] == "World");
  REQUIRE(v[2] == "!");

  v[2] = "?";

  str = static_cast<std::vector<string>>(v);

  REQUIRE(v[0] == "Hello");
  REQUIRE(v[1] == "World");
  REQUIRE(v[2] == "?");

  REQUIRE(str[0] == "Hello");
  REQUIRE(str[1] == "World");
  REQUIRE(str[2] == "?");
}