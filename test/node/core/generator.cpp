//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <catch2/catch.hpp>

#include <yave/obj/string/string.hpp>
#include <yave/signal/generator.hpp>
#include <yave/obj/primitive/primitive.hpp>

using namespace yave;

TEST_CASE("generators")
{
  using Float  = Float32;
  using Double = Float64;

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
      using Func = UnarySignalFunction<Int, Int, func>;
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
      using Func              = BinarySignalFunction<Int, Float, Float, func>;
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
      using Func = TernarySignalFunction<Int, Float, Double, Float, func>;
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
      using Func = QuaternarySignalFunction<Int, Float, Double, Int, Float, func>;
      [[maybe_unused]] auto v = make_object<Func>();
    }
  }
}