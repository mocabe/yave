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
    REQUIRE(str == u8"");
    REQUIRE(str == str);
    REQUIRE(str.length() == 0);
  }

  SECTION("char* copy")
  {
    auto str = string("abc");
    REQUIRE(str == u8"abc");
    REQUIRE(str == str);
    REQUIRE(str.length() == 3);
  }

  SECTION("u8 init")
  {
    auto str = string(u8"abc");
    REQUIRE(str == u8"abc");
  }

  SECTION("string copy")
  {
    string tmp = u8"abc";
    {
      auto str = tmp;
      REQUIRE(str == u8"abc");
      REQUIRE(str.length() == 3);
    }
  }

  SECTION("string move")
  {
    auto tmp = string("abc");
    {
      string str = std::move(tmp);
      REQUIRE(str == u8"abc");
      REQUIRE(str.length() == 3);
    }

    SECTION("after move")
    {
      REQUIRE(tmp.length() == 0);
      REQUIRE(tmp == u8"");
      REQUIRE(tmp == string(tmp.c_str()));
    }
  }

  SECTION("std::u8string move")
  {
    auto str = std::u8string(u8"abc");
    REQUIRE(str == u8"abc");
    REQUIRE(str.length() == 3);
  }

  SECTION("std::u8string copy")
  {
    auto tmp = std::u8string(u8"abc");
    {
      string str = tmp;
      REQUIRE(str == u8"abc");
      REQUIRE(str.length() == 3);
    }
  }

  SECTION("std::string copy")
  {
    auto tmp = std::string("abc");
    auto str = string(tmp);
    REQUIRE(str == u8"abc");
  }

  SECTION("std::string move")
  {
    auto str = string(std::string("abc"));
    REQUIRE(str == u8"abc");
  }

  SECTION("self init")
  {
    string str = str;
    REQUIRE(str == u8"");
  }

  SECTION("self move init")
  {
    string str = std::move(str);
    REQUIRE(str == u8"");
  }
}

TEST_CASE("yave::string assignments")
{
  SECTION("char*")
  {
    auto str = string("x");
    str      = u8"abc";
    REQUIRE(str == u8"abc");
    REQUIRE(str.length() == 3);
  }

  SECTION("copy")
  {
    string tmp = u8"abc";
    string str = u8"x";
    str        = tmp;
    REQUIRE(str == tmp);
    REQUIRE(str == u8"abc");
    REQUIRE(str.length() == 3);
  }

  SECTION("move")
  {
    string tmp = u8"abc";
    string str = u8"x";
    str        = std::move(tmp);
    REQUIRE(str == u8"abc");
    REQUIRE(str.length() == 3);
  }

  SECTION("self assign")
  {
    string str;
    str = str;
    REQUIRE(str == u8"");
    REQUIRE(str.length() == 0);
  }

  SECTION("self move assign")
  {
    string str = u8"abc";
    str        = std::move(str);
    REQUIRE(str == u8"abc");
  }
}

TEST_CASE("yave::string util")
{
  SECTION("swap")
  {
    string str1 = u8"abc";
    string str2 = u8"xyz";
    str1.swap(str2);
    REQUIRE(str1 == u8"xyz");
    REQUIRE(str2 == u8"abc");
  }

  SECTION("range based for")
  {
    string str = u8"aaa";
    for (auto&& c : str) {
      c = 'x';
    }
    REQUIRE(str == u8"xxx");
  }

  SECTION("range based for const")
  {
    const string str = u8"xxx";
    for (auto&& c : str) {
      REQUIRE(c == 'x');
    }
  }

  SECTION("eq")
  {
    auto str = string("abc");
    REQUIRE(u8"abc" == str);
    REQUIRE(str == u8"abc");
    REQUIRE(str == str);
  }
}