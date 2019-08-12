//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/data/lib/string.hpp>
#include <catch2/catch.hpp>

using namespace yave;

TEST_CASE("yave::string constructors")
{
  SECTION("default")
  {
    string str;
    REQUIRE(str == "");
    REQUIRE(str == str);
    REQUIRE(str.length() == 0);
  }

  SECTION("char* copy")
  {
    string str = "abc";
    REQUIRE(str == "abc");
    REQUIRE(str == str);
    REQUIRE(str.length() == 3);
  }

  SECTION("string copy")
  {
    string tmp = "abc";
    {
      auto str = tmp;
      REQUIRE(str == "abc");
      REQUIRE(str.length() == 3);
    }
  }

  SECTION("std::string move")
  {
    string str = std::string("abc");
    REQUIRE(str == "abc");
    REQUIRE(str.length() == 3);
  }

  SECTION("std::string copy")
  {
    std::string tmp = "abc";
    {
      string str = tmp;
      REQUIRE(str == "abc");
      REQUIRE(str.length() == 3);
    }
  }

  SECTION("self init")
  {
    string str = str;
    REQUIRE(str == "");
  }
}

TEST_CASE("yave::string assignments")
{
  SECTION("char*")
  {
    string str = "x";
    str        = "abc";
    REQUIRE(str == "abc");
    REQUIRE(str.length() == 3);
  }

  SECTION("copy")
  {
    string tmp = "abc";
    string str = "x";
    str        = tmp;
    REQUIRE(str == tmp);
    REQUIRE(str == "abc");
    REQUIRE(str.length() == 3);
  }

  SECTION("move")
  {
    string tmp = "abc";
    string str = "x";
    str        = std::move(tmp);
    REQUIRE(str == "abc");
    REQUIRE(str.length() == 3);
  }

  SECTION("self assign")
  {
    string str;
    str = str;
    REQUIRE(str == "");
    REQUIRE(str.length() == 0);
  }
}