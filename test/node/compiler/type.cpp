//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <catch2/catch.hpp>

#include <yave/node/compiler/type.hpp>
#include <yave/node/compiler/compile_result.hpp>
#include <yave/rts/rts.hpp>

using namespace yave;

TEST_CASE("overloading")
{
  using Float  = Float32;
  using Double = Float64;

  // empty loc map
  location_map loc;

  SECTION("f: X->X with Int->Int")
  {
    struct F : Function<F, Int, Int>
    {
      auto code() const -> return_type
      {
        throw;
      }
    };

    auto f  = make_object<F>();
    auto i  = make_object<Int>(42);
    auto id = make_object<Identity>();

    class_env env;
    auto o = env.add_overloading(uid::random_generate(), {f});

    SECTION("f 42")
    {
      auto app        = o << i;
      auto [ty, app2] = type_of_overloaded(app, std::move(env), std::move(loc));

      REQUIRE(same_type(ty, object_type<Int>()));

      auto ty2 = type_of(app2);
      REQUIRE(same_type(ty, ty2));
    }

    SECTION("f (f 42)")
    {
      auto app        = o << (o << i);
      auto [ty, app2] = type_of_overloaded(app, std::move(env), std::move(loc));

      REQUIRE(same_type(ty, object_type<Int>()));

      auto ty2 = type_of(app2);
      REQUIRE(same_type(ty, ty2));
    }

    SECTION("(id f) 42")
    {
      auto app        = (id << o) << i;
      auto [ty, app2] = type_of_overloaded(app, std::move(env), std::move(loc));

      REQUIRE(same_type(ty, object_type<Int>()));

      auto ty2 = type_of(app2);
      REQUIRE(same_type(ty, ty2));
    }

    SECTION("(lx.o x) 42")
    {
      auto var = make_object<Variable>();
      auto lam = make_object<Lambda>();

      _get_storage(*lam).var  = var;
      _get_storage(*lam).body = o << var;

      auto app        = lam << i;
      auto [ty, app2] = type_of_overloaded(app, std::move(env), std::move(loc));

      REQUIRE(same_type(ty, object_type<Int>()));

      auto ty2 = type_of(app2);
      REQUIRE(same_type(ty, ty2));
    }
  }

  SECTION("f: X->X with Double->Double, Int->Int")
  {
    class X;
    struct F : Function<F, Double, Double>
    {
      auto code() const -> return_type
      {
        throw;
      }
    };

    struct G : Function<G, Int, Int>
    {
      auto code() const -> return_type
      {
        throw;
      }
    };

    auto f  = make_object<F>();
    auto g  = make_object<G>();
    auto i  = make_object<Int>(42);
    auto b  = make_object<Bool>(true);
    auto id = make_object<Identity>();

    class_env env;
    auto o = env.add_overloading(uid::random_generate(), {f, g});

    SECTION("f true")
    {
      auto app = o << b;
      REQUIRE_THROWS_AS(
        type_of_overloaded(app, std::move(env), std::move(loc)),
        compile_results::no_valid_overloading);
    }

    SECTION("(id f) (f 42)")
    {
      auto app        = (id << o) << (o << i);
      auto [ty, app2] = type_of_overloaded(app, std::move(env), std::move(loc));

      REQUIRE(same_type(ty, object_type<Int>()));

      auto ty2 = type_of(app2);
      REQUIRE(same_type(ty, ty2));
    }

    SECTION("(id f) 42")
    {
      auto app        = (id << o) << i;
      auto [ty, app2] = type_of_overloaded(app, std::move(env), std::move(loc));

      REQUIRE(same_type(ty, object_type<Int>()));

      auto ty2 = type_of(app2);
      REQUIRE(same_type(ty, ty2));
    }

    SECTION("(id (lx.(id o) x)) 42")
    {
      auto var = make_object<Variable>();
      auto lam = make_object<Lambda>();

      _get_storage(*lam).var  = var;
      _get_storage(*lam).body = (id << o) << var;

      auto app        = (id << lam) << i;
      auto [ty, app2] = type_of_overloaded(app, std::move(env), std::move(loc));

      REQUIRE(same_type(ty, object_type<Int>()));

      auto ty2 = type_of(app2);
      REQUIRE(same_type(ty, ty2));
    }
  }

  SECTION("f 42 [f: X->X with {...}]")
  {
    struct F : Function<F, Int, Int>
    {
      auto code() const -> return_type
      {
        throw;
      }
    };

    struct G : Function<G, Double, Double>
    {
      auto code() const -> return_type
      {
        throw;
      }
    };

    struct H : Function<H, Float, Float>
    {
      auto code() const -> return_type
      {
        throw;
      }
    };

    struct I : Function<I, List<Int>, List<Int>>
    {
      auto code() const -> return_type
      {
        throw;
      }
    };

    struct J : Function<J, List<Double>, List<Double>>
    {
      auto code() const -> return_type
      {
        throw;
      }
    };

    auto f  = make_object<F>();
    auto g  = make_object<G>();
    auto h  = make_object<H>();
    auto i  = make_object<I>();
    auto j  = make_object<J>();
    auto id = make_object<Identity>();

    class_env env;
    auto ovl = env.add_overloading(uid::random_generate(), {j, i, h, g, f});

    SECTION("f Int")
    {
      auto app        = (id << ovl) << make_object<Int>();
      auto [ty, app2] = type_of_overloaded(app, std::move(env), std::move(loc));

      REQUIRE(same_type(ty, object_type<Int>()));

      auto ty2 = type_of(app2);
      REQUIRE(same_type(ty, ty2));
    }

    SECTION("f Float")
    {
      auto app        = (id << ovl) << make_object<Float>();
      auto [ty, app2] = type_of_overloaded(app, std::move(env), std::move(loc));

      REQUIRE(same_type(ty, object_type<Float>()));

      auto ty2 = type_of(app2);
      REQUIRE(same_type(ty, ty2));
    }

    SECTION("f List<Int>")
    {
      auto app        = (id << ovl) << make_object<List<Int>>();
      auto [ty, app2] = type_of_overloaded(app, std::move(env), std::move(loc));

      REQUIRE(same_type(ty, object_type<List<Int>>()));

      auto ty2 = type_of(app2);
      REQUIRE(same_type(ty, ty2));
    }

    SECTION("f List<Double>")
    {
      auto app        = (id << ovl) << make_object<List<Double>>();
      auto [ty, app2] = type_of_overloaded(app, std::move(env), std::move(loc));

      REQUIRE(same_type(ty, object_type<List<Double>>()));

      auto ty2 = type_of(app2);
      REQUIRE(same_type(ty, ty2));
    }

    SECTION("(lx. o x) Int")
    {
      auto var = make_object<Variable>();
      auto lam = make_object<Lambda>();

      _get_storage(*lam).var  = var;
      _get_storage(*lam).body = ovl << var;

      auto app        = lam << make_object<Int>();
      auto [ty, app2] = type_of_overloaded(app, std::move(env), std::move(loc));

      REQUIRE(same_type(ty, object_type<Int>()));

      auto ty2 = type_of(app2);
      REQUIRE(same_type(ty, ty2));
    }

    SECTION("(lx. (id o) x) List<Double>")
    {
      auto var = make_object<Variable>();
      auto lam = make_object<Lambda>();

      _get_storage(*lam).var  = var;
      _get_storage(*lam).body = id << ovl << var;

      auto app        = lam << make_object<List<Double>>();
      auto [ty, app2] = type_of_overloaded(app, std::move(env), std::move(loc));

      REQUIRE(same_type(ty, object_type<List<Double>>()));

      auto ty2 = type_of(app2);
      REQUIRE(same_type(ty, ty2));
    }
  }

  SECTION("f : X->X with [a->a, Int->Int]")
  {
    struct F : Function<F, Int, Int>
    {
      return_type code() const
      {
        throw;
      }
    };

    class a;
    struct G : Function<G, List<a>, List<a>>
    {
      return_type code() const
      {
        throw;
      }
    };

    auto f = make_object<F>();
    auto g = make_object<G>();

    class_env env;
    auto ovl = env.add_overloading(uid::random_generate(), {g, f});

    SECTION("f Int")
    {
      auto app        = ovl << make_object<Int>();
      auto [ty, app2] = type_of_overloaded(app, std::move(env), std::move(loc));

      REQUIRE(same_type(ty, object_type<Int>()));

      auto ty2 = type_of(app2);
      REQUIRE(same_type(ty, ty2));
    }

    SECTION("f List<Int>")
    {
      auto app        = ovl << make_object<List<Int>>();
      auto [ty, app2] = type_of_overloaded(app, std::move(env), std::move(loc));

      REQUIRE(same_type(ty, object_type<List<Int>>()));

      auto ty2 = type_of(app2);
      REQUIRE(same_type(ty, ty2));
    }
  }

  // SECTION("f: X->Y")
  // {
  //   struct F : Function<F, Int, Double>
  //   {
  //     auto code() const -> return_type
  //     {
  //       throw;
  //     }
  //   };

  //   struct G : Function<G, Float, Bool>
  //   {
  //     auto code() const -> return_type
  //     {
  //       throw;
  //     }
  //   };

  //   auto f  = make_object<F>();
  //   auto g  = make_object<G>();
  //   auto id = make_object<Identity>();

  //   class_env env;
  //   auto o = env.add_overloading(uid::random_generate(), {f, g});

  //   SECTION("f Int")
  //   {
  //     auto app        = o << make_object<Int>();
  //     auto [ty, app2] = type_of_overloaded(app, std::move(env),
  //     std::move(loc));

  //     REQUIRE(same_type(ty, object_type<Double>()));

  //     auto ty2 = type_of(app2);
  //     REQUIRE(same_type(ty, ty2));
  //   }

  //   SECTION("f Float")
  //   {
  //     auto app        = o << make_object<Float>();
  //     auto [ty, app2] = type_of_overloaded(app, std::move(env),
  //     std::move(loc));

  //     REQUIRE(same_type(ty, object_type<Bool>()));

  //     auto ty2 = type_of(app2);
  //     REQUIRE(same_type(ty, ty2));
  //   }

  //   SECTION("id ((id f) Int)")
  //   {
  //     auto app        = (id << o) << make_object<Int>();
  //     auto [ty, app2] = type_of_overloaded(app, std::move(env),
  //     std::move(loc));

  //     REQUIRE(same_type(ty, object_type<Double>()));

  //     auto ty2 = type_of(app2);
  //     REQUIRE(same_type(ty, ty2));
  //   }
  // }

  SECTION("ho")
  {
    struct F : Function<F, closure<Int, Int>, Int, Double>
    {
      return_type code() const
      {
        throw;
      }
    };

    struct G : Function<G, closure<Int, Double>, Int, Double>
    {
      return_type code() const
      {
        throw;
      }
    };

    struct H : Function<H, Int, Double>
    {
      return_type code() const
      {
        throw;
      }
    };

    class X;
    struct I : Function<I, closure<X, Double>, Int, Double>
    {
      return_type code() const
      {
        throw;
      }
    };

    auto f = make_object<F>();
    auto g = make_object<G>();
    auto h = make_object<H>();
    auto i = make_object<I>();

    SECTION("1")
    {
      auto app = i << (i << h);
      auto ty  = type_of(app);
      REQUIRE(same_type(ty, object_type<closure<Int, Double>>()));
    }

    SECTION("2")
    {
      class_env env;
      auto overloaded = env.add_overloading(uid::random_generate(), {f, g});

      auto app = overloaded << (overloaded << h);

      auto [ty, app2] = type_of_overloaded(app, std::move(env), std::move(loc));

      REQUIRE(same_type(ty, object_type<closure<Int, Double>>()));

      auto ty2 = type_of(app2);

      REQUIRE(same_type(ty, ty2));
    }
  }
}