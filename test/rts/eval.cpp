//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/rts/rts.hpp>
#include <catch2/catch.hpp>

using namespace yave;

namespace yave {
  using Int    = yave::Box<int>;
  using Double = yave::Box<double>;
  using Float  = yave::Box<float>;
  using Bool   = yave::Box<bool>;
} // namespace yave

YAVE_DECL_TYPE(Int, "7d27665a-c56a-40d1-8e2e-844cb48de9e9");
YAVE_DECL_TYPE(Double, "9cc69b38-8766-44f1-93e9-337cfb3d3bc5");
YAVE_DECL_TYPE(Float, "51b6aa8c-b54a-417f-bfea-5bbff6ef00c2");
YAVE_DECL_TYPE(Bool, "7ba340e7-8c41-41bc-a1f9-bea2a2db077d");

TEST_CASE("eval")
{
  SECTION("smpl")
  {
    struct F : Function<F, Int, Int, Int>
    {
      return_type code() const
      {
        return make_object<Int>(*eval_arg<0>() + *eval_arg<1>());
      }
    };

    auto f = make_object<F>();

    SECTION("1")
    {
      auto term = f << new Int(42);
      REQUIRE(type_of(term));
    }

    SECTION("2")
    {
      auto term = f << new Int(42) << new Int(24);
      REQUIRE(type_of(term));
      REQUIRE(*value_cast<Int>(eval(term)) == 66);
    }

    SECTION("3")
    {
      auto term = f << new Int(42) << (f << new Int(24) << new Int(24));
      REQUIRE(type_of(term));
      REQUIRE(*value_cast<Int>(eval(term)) == 90);
    }

    SECTION("4")
    {
      auto term = f << (f << new Int(42) << new Int(24))
                    << (f << new Int(42) << new Int(24));
      REQUIRE(type_of(term));
      REQUIRE(*value_cast<Int>(eval(term)) == 132);
    }
  }

  SECTION("ho")
  {
    struct F : Function<F, closure<Int, Int>, Int, Int>
    {
      return_type code() const
      {
        return arg<0>() << arg<1>();
      }
    };

    struct G : Function<G, Int, Int>
    {
      return_type code() const
      {
        return new Int(*eval_arg<0>() * 2);
      }
    };

    auto f = make_object<F>();
    auto g = make_object<G>();

    SECTION("0")
    {
      auto term = f << g << new Int(42);
      REQUIRE(type_of(term));
      REQUIRE(*value_cast<Int>(eval(term)) == 84);
    }

    SECTION("1")
    {
      auto term = f << (f << g) << new Int(42);
      REQUIRE(type_of(term));
      REQUIRE(*value_cast<Int>(eval(term)) == 84);
    }
  }

  SECTION("rec")
  {
    struct Fib : Function<Fib, Int, Int>
    {
      struct Impl : Function<Impl, closure<Int, Int>, Int, Int>
      {
        return_type code() const
        {
          auto fib = arg<0>();
          auto n   = eval_arg<1>();

          if (*n < 2)
            return n;

          auto l = eval(fib << new Int(*n - 1));
          auto r = eval(fib << new Int(*n - 2));
          return new Int(*value_cast<Int>(l) + *value_cast<Int>(r));
        }
      };

      return_type code() const
      {
        static const auto fix  = make_object<Fix>();
        static const auto impl = make_object<Impl>();
        return fix << impl << arg<0>();
      }
    };

    auto fib = make_object<Fib>();

    SECTION("0")
    {
      auto term = fib << new Int(10);
      REQUIRE(type_of(term));
      REQUIRE(*value_cast<Int>(eval(term)) == 55);
    }
  }
}