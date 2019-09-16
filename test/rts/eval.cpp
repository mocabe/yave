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
  SECTION("value")
  {
    SECTION("i")
    {
      auto i = make_object<Int>();
      REQUIRE(i == eval(i));
    }
  }

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

    SECTION("0")
    {
      REQUIRE(object_ptr<const Object>(f) == object_ptr<const Object>(eval(f)));
    }

    SECTION("1")
    {
      auto term = f << new Int(42);
      REQUIRE(type_of(term));
      (void)eval(term);
      REQUIRE(type_of(term));
    }

    SECTION("2")
    {
      auto term = f << new Int(42) << new Int(24);
      REQUIRE(type_of(term));
      REQUIRE(*value_cast<Int>(eval(term)) == 66);
      REQUIRE(*value_cast<Int>(eval(term)) == 66);
    }

    SECTION("3")
    {
      auto term = f << new Int(42) << (f << new Int(24) << new Int(24));
      REQUIRE(type_of(term));
      REQUIRE(*value_cast<Int>(eval(term)) == 90);
      REQUIRE(*value_cast<Int>(eval(term)) == 90);
    }

    SECTION("4")
    {
      auto term = f << (f << new Int(42) << new Int(24))
                    << (f << new Int(42) << new Int(24));
      REQUIRE(type_of(term));
      REQUIRE(*value_cast<Int>(eval(term)) == 132);
      REQUIRE(*value_cast<Int>(eval(term)) == 132);
    }

    SECTION("5")
    {
      auto part = eval(f << new Int(42));
      auto term = part << new Int(24);
      REQUIRE(type_of(term));
      REQUIRE(*value_cast<Int>(eval(term)) == 66);
      REQUIRE(*value_cast<Int>(eval(term)) == 66);
    }

    SECTION("6")
    {
      auto app = f << new Int(42) << new Int(24);
      (void)eval(app); // result will be cached
      auto term = f << app << app;
      REQUIRE(type_of(app));
      REQUIRE(type_of(term));
      REQUIRE(*value_cast<Int>(eval(term)) == 132);
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

  SECTION("ho2")
  {
    struct F : Function<F, closure<Int, Int, Int>, Int, closure<Int, Int>>
    {
      return_type code() const
      {
        return arg<0>() << arg<1>();
      }
    };

    struct G : Function<G, Int, Int, Int>
    {
      return_type code() const
      {
        return arg<1>();
      }
    };

    auto f = make_object<F>();
    auto g = make_object<G>();

    SECTION("0")
    {
      auto part = f << g << new Int(2019);
      (void)eval(part);

      auto t1 = part << new Int(42);
      auto t2 = part << new Int(24);

      REQUIRE(type_of(t1));
      REQUIRE(type_of(t2));

      REQUIRE(*value_cast<Int>(eval(t1)) == 42);
      REQUIRE(*value_cast<Int>(eval(t2)) == 24);
    }

    SECTION("1")
    {
      struct H : Function<H, closure<Int, Int, Int>, Int, closure<Int, Int>>
      {
        return_type code() const
        {
          // direct PAP return
          return eval(arg<0>() << arg<1>());
        }
      };

      auto h    = make_object<H>();
      auto term = h << g << new Int(0) << new Int(42);

      REQUIRE(type_of(term));
      REQUIRE(*value_cast<Int>(eval(term)) == 42);
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

          auto l = eval(fib << make_object<Int>(*n - 1));
          auto r = eval(fib << make_object<Int>(*n - 2));
          return make_object<Int>(*value_cast<Int>(l) + *value_cast<Int>(r));
        }
      };

      return_type code() const
      {
        return make_object<Fix>() << make_object<Impl>() << arg<0>();
      }
    };

    auto fib = make_object<Fib>();

    SECTION("0")
    {
      auto term = fib << new Int(10);
      REQUIRE(type_of(term));
      REQUIRE(*value_cast<Int>(eval(term)) == 55);
      REQUIRE(type_of(term));
      REQUIRE(*value_cast<Int>(eval(term)) == 55);
    }

    SECTION("1")
    {
      struct Add : Function<Add, Int, Int, Int>
      {
        return_type code() const
        {
          return new Int(*eval_arg<0>() + *eval_arg<1>());
        }
      };

      auto add    = make_object<Add>();
      auto shared = fib << new Int(10);
      auto term   = add << shared << shared;

      REQUIRE(type_of(term));
      REQUIRE(*value_cast<Int>(eval(term)) == 55 * 2);
    }
  }
}

TEST_CASE("copy_apply_graph", "[rts][eval]")
{
  static int count = 0;

  struct F : Function<F, Int, Int, Int>
  {
    return_type code() const
    {
      (void)eval_arg<0>();
      (void)eval_arg<1>();
      ++count;
      return new Int(42);
    }
  };

  auto f1 = make_object<F>();
  auto f2 = make_object<F>();
  auto i  = make_object<Int>();

  auto sub = f2 << i << i;
  auto app = f1 << (f2 << sub << i) << (f2 << sub << i);

  SECTION("")
  {
    count = 0;

    SECTION("eval")
    {
      REQUIRE_NOTHROW(type_of(app));
      REQUIRE_NOTHROW(eval(app));
      REQUIRE(count == 4);
    }

    SECTION("copy before eval")
    {
      auto copy = copy_apply_graph(app);
      REQUIRE_NOTHROW(type_of(copy));
      REQUIRE_NOTHROW(eval(app));
      REQUIRE_NOTHROW(eval(copy));
      REQUIRE(count == 8);
      REQUIRE_NOTHROW(eval(copy));
      REQUIRE(count == 8);
    }

    SECTION("copy after eval")
    {
      REQUIRE_NOTHROW(eval(app));
      auto copy = copy_apply_graph(app);
      REQUIRE_NOTHROW(copy);
      REQUIRE(count == 4);
    }
  }
}