//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <catch2/catch.hpp>

#include <yave/node/core/function.hpp>
#include <yave/obj/primitive/primitive.hpp>

using namespace yave;

TEST_CASE("NodeFunction")
{
  using Float  = Float32;
  using Double = Float64;

  SECTION("simple")
  {
    struct F : NodeFunction<F, Int, Float, Double>
    {
      return_type code() const
      {
        return make_object<Double>(*eval_arg<0>() + *eval(arg<1>()));
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

  SECTION("poly")
  {
    class X;
    struct F : NodeFunction<F, Bool, forall<X>, forall<X>, forall<X>>
    {
      return_type code() const
      {
        if (*eval_arg<0>())
          return arg<1>();
        else
          return arg<2>();
      }
    };

    auto f = make_object<F>();
  }
}