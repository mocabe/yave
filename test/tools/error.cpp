//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/tools/error.hpp>
#include <catch2/catch.hpp>
#include <fmt/format.h>

using namespace yave;

TEST_CASE("Error")
{
  SECTION("Success")
  {
    SECTION("init")
    {
      auto succ  = Success();
      auto succ2 = succ;
      auto succ3 = std::move(succ2);
      succ2      = succ3;
      succ3      = std::move(succ);

      REQUIRE(succ.is_success());
      REQUIRE(succ2.is_success());
      REQUIRE(succ3.is_success());
    }
    SECTION("convert")
    {
      auto succ = Success();
      Error err = Success();
      err       = succ;
      err       = std::move(succ);

      REQUIRE(succ.is_success());
      REQUIRE(err.is_success());
    }
  }
}

TEST_CASE("ErrorInfo")
{
  SECTION("default")
  {
    struct MyError : ErrorInfo<MyError>
    {
    };

    auto err = make_error<MyError>();
    REQUIRE(!err.is_success());
    REQUIRE(!err);
    REQUIRE(err.message() == "ErrorInfo");

    auto err2 = std::move(err);
    REQUIRE(!err2.is_success());
    REQUIRE(!err2);
    REQUIRE(err2.message() == "ErrorInfo");
  }

  SECTION("custom")
  {
    struct MyError : ErrorInfo<MyError>
    {
      MyError(const std::string& str)
        : msg {str}
      {
      }
      std::string message() const
      {
        return msg;
      }

    private:
      std::string msg;
    };

    auto err = make_error<MyError>("TestString");

    REQUIRE(err.message() == "TestString");
    REQUIRE(err.type() == typeid(MyError));
  }
}