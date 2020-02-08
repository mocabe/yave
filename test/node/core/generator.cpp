//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <catch2/catch.hpp>

#include <yave/obj/string/string.hpp>
#include <yave/node/core/generator.hpp>

using namespace yave;

TEST_CASE("generators")
{
  SECTION("Unary")
  {
    {
      struct func
      {
        int operator()(int)
        {
          return 42;
        }
      };
      using Func = UnaryFunction<Int, Int, func>;
      auto v     = make_object<Func>();
    }
  }
  SECTION("Binary")
  {
    {
      struct func
      {
        float operator()(int, float)
        {
          return 42;
        }
      };
      using Func              = BinaryFunction<Int, Float, Float, func>;
      [[maybe_unused]] auto v = make_object<Func>();
    }
  }
  SECTION("Ternary")
  {
    {
      struct func
      {
        float operator()(int, float, double)
        {
          return 42;
        }
      };
      using Func = TernaryFunction<Int, Float, Double, Float, func>;
      [[maybe_unused]] auto v = make_object<Func>();
    }
  }
  SECTION("Quaternary")
  {
    {
      struct func
      {
        float operator()(int, float, double, int)
        {
          return 42;
        }
      };
      using Func = QuaternaryFunction<Int, Float, Double, Int, Float, func>;
      [[maybe_unused]] auto v = make_object<Func>();
    }
  }
}