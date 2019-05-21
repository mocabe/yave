//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <catch2/catch.hpp>

#include <yave/node/objects/function.hpp>
#include <yave/core/objects/primitive.hpp>

using namespace yave;

TEST_CASE("NodeFunction")
{
  SECTION("simple")
  {
    struct F : NodeFunction<F, Int, Float, Double>
    {
      return_type code() const
      {
        return make_object<Double>(*eval_arg<0>() + *eval(arg<1>()));
        return new Double();
      }
    };

    auto f = make_object<F>();
  }

  SECTION("Higher-order")
  {
    struct F : NodeFunction<F, closure<Int, Double>, Int, Double>
    {
      return_type code() const
      {
        return arg<0>() << arg<1>();
        return eval_arg<0>() << arg<1>();
        return eval_arg<0>() << eval_arg<1>();
        return eval(arg<0>() << arg<1>());
        return eval(eval_arg<0>() << arg<1>());
        return eval(eval_arg<0>() << eval_arg<1>());
      }
    };

    auto f = make_object<F>();
  }
}