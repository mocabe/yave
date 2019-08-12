//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/lib/string/string.hpp>
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

  SECTION("u8 init")
  {
    string str = u8"abc";
    REQUIRE(str == "abc");
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

  SECTION("string move")
  {
    string tmp = "abc";
    {
      string str = std::move(tmp);
      REQUIRE(str == "abc");
      REQUIRE(str.length() == 3);
    }

    SECTION("after move")
    {
      REQUIRE(tmp.length() == 0);
      REQUIRE(tmp == "");
      REQUIRE(tmp == string(tmp.c_str()));
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

TEST_CASE("yave::string util")
{
  SECTION("swap")
  {
    string str1 = "abc";
    string str2 = "xyz";
    str1.swap(str2);
    REQUIRE(str1 == "xyz");
    REQUIRE(str2 == "abc");
  }

  SECTION("range based for")
  {
    string str = "aaa";
    for (auto&& c : str) {
      c = 'x';
    }
    REQUIRE(str == "xxx");
  }

  SECTION("range based for const")
  {
    const string str = "xxx";
    for (auto&& c : str) {
      REQUIRE(c == 'x');
    }
  }

  SECTION("eq")
  {
    std::string str = "abc";
    REQUIRE("abc" == str);
    REQUIRE(str == "abc");
    REQUIRE(str == str);
  }
}