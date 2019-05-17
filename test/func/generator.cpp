//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/func/generator.hpp>
#include <catch2/catch.hpp>

#include <yave/core/objects/string.hpp>

using namespace yave;

using Int    = Box<int>;
using Float  = Box<float>;
using Double = Box<double>;

YAVE_DECL_TYPE(Int, "7d27665a-c56a-40d1-8e2e-844cb48de9e9");
YAVE_DECL_TYPE(Double, "9cc69b38-8766-44f1-93e9-337cfb3d3bc5");
YAVE_DECL_TYPE(Float, "51b6aa8c-b54a-417f-bfea-5bbff6ef00c2");

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
      [[maybe_unused]] auto v = make_object<Func>;
    }
  }
}