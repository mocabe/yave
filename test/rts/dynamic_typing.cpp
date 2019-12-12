//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <catch2/catch.hpp>

#include <yave/rts/dynamic_typing.hpp>
#include <yave/rts/function.hpp>
#include <yave/rts/fix.hpp>
#include <yave/rts/identity.hpp>
#include <yave/rts/to_string.hpp>
#include <yave/rts/list.hpp>

#include <fmt/format.h>

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

TEST_CASE("subst_type")
{

  SECTION("[X->Int] X == Int")
  {
    auto var     = genvar();
    auto tyarrow = type_arrow {var, object_type<Int>()};
    REQUIRE(same_type(subst_type(tyarrow, var), object_type<Int>()));
  }

  SECTION("[X->Int] X->Int == Int->Int")
  {
    auto var     = genvar();
    auto tyarrow = type_arrow {var, object_type<Int>()};
    auto type    = make_object<Type>(arrow_type {var, object_type<Int>()});
    REQUIRE(same_type(
      subst_type(tyarrow, type),
      make_object<Type>(arrow_type {object_type<Int>(), object_type<Int>()})));
  }

  SECTION("[X->Int] List<X> == List<Int>")
  {
    auto var     = genvar();
    auto tyarrow = type_arrow {var, object_type<Int>()};
    REQUIRE(same_type(
      subst_type(tyarrow, make_object<Type>(list_type {var})),
      object_type<List<Int>>()));
  }
}

TEST_CASE("subst_type_all")
{
  SECTION(
    "(Y->X) -> (Float->Y) [X->Int, Y->Float] => (Float->Int) -> (Float->Float)")
  {
    auto X               = genvar();
    auto Y               = genvar();
    std::vector tyarrows = {type_arrow {X, object_type<Int>()},
                            type_arrow {Y, object_type<Float>()}};
    auto type            = make_object<Type>(
      arrow_type {new Type(arrow_type {Y, X}),
                  new Type(arrow_type {object_type<Float>(), Y})});

    REQUIRE(same_type(
      subst_type_all(tyarrows, type),
      make_object<Type>(arrow_type {
        new Type(arrow_type {object_type<Float>(), object_type<Int>()}),
        new Type(arrow_type {object_type<Float>(), object_type<Float>()})})));
  }
}

TEST_CASE("unify")
{
  // compare two type_arrow vectors
  auto eq_arrows = [](auto&& result, auto&& ans) {
    if (result.size() != ans.size())
      return false;
    for (auto&& r : result) {
      auto b = [&]() {
        for (auto&& a : ans) {
          if (same_type(r.from, a.from) && same_type(r.to, a.to))
            return true;
        }
        return false;
      }();
      if (!b)
        return false;
    }
    return true;
  };

  auto print_tyarrows = [](auto&& tyarrows) {
    fmt::print("[");
    for (auto&& ta : tyarrows) {
      fmt::print("{}->{}, ", to_string(ta.from), to_string(ta.to));
    }
    fmt::print("]\n");
  };

  auto X = genvar();
  auto Y = genvar();
  auto Z = genvar();
  auto U = genvar();
  auto W = genvar();

  SECTION("[X=int]")
  {
    std::vector cs = {type_constr {X, object_type<Int>()}};
    auto result    = unify(cs);

    SECTION("reunify")
    {
      for (auto&& ta : result) {
        cs = subst_constr_all(ta, cs);
      }
      REQUIRE(unify(cs).empty());
    }

    SECTION("eq")
    {
      std::vector ans = {type_arrow {X, object_type<Int>()}};
      print_tyarrows(result);
      print_tyarrows(ans);
      CHECK(eq_arrows(result, ans));
    }
  }

  SECTION("[X=Int, Y=X->X]")
  {
    std::vector cs     = {type_constr {X, object_type<Int>()},
                      type_constr {Y, new Type(arrow_type {X, X})}};
    std::vector result = unify(cs);

    SECTION("reunify")
    {
      for (auto&& ta : result) {
        cs = subst_constr_all(ta, cs);
      }
      REQUIRE(unify(cs).empty());
    }

    SECTION("eq")
    {
      std::vector ans = {
        type_arrow {X, object_type<Int>()},
        type_arrow {
          Y, new Type(arrow_type {object_type<Int>(), object_type<Int>()})}};

      print_tyarrows(result);
      print_tyarrows(ans);
      CHECK(eq_arrows(result, ans));
    }
  }

  SECTION("[Int-Int=X->Y]")
  {
    std::vector cs     = {type_constr {
      new Type(arrow_type {object_type<Int>(), object_type<Int>()}),
      new Type(arrow_type {X, Y})}};
    std::vector result = unify(cs);

    SECTION("reunify")
    {
      for (auto&& ta : result) {
        cs = subst_constr_all(ta, cs);
      }
      REQUIRE(unify(cs).empty());
    }

    SECTION("eq")
    {
      std::vector ans = {type_arrow {X, object_type<Int>()},
                         type_arrow {Y, object_type<Int>()}};
      print_tyarrows(result);
      print_tyarrows(ans);
      CHECK(eq_arrows(result, ans));
    }
  }

  SECTION("[X->Y=Y->Z, Z=U->W]")
  {
    std::vector cs = {
      type_constr {new Type(arrow_type {X, Y}), new Type(arrow_type {Y, Z})},
      type_constr {Z, new Type(arrow_type {U, W})}};
    auto result = unify(cs);

    SECTION("reunify")
    {
      for (auto&& ta : result) {
        cs = subst_constr_all(ta, cs);
      }
      REQUIRE(unify(cs).empty());
    }

    SECTION("eq")
    {
      std::vector ans = {type_arrow {X, new Type(arrow_type {U, W})},
                         type_arrow {Y, new Type(arrow_type {U, W})},
                         type_arrow {Z, new Type(arrow_type {U, W})}};
      print_tyarrows(result);
      print_tyarrows(ans);
      CHECK(eq_arrows(result, ans));
    }
  }

  SECTION("List<Int> -> List<List<Double>> = List<X> -> List<Y>")
  {
    std::vector cs = {type_constr {
      new Type(arrow_type {new Type(list_type {object_type<Int>()}),
                           new Type(list_type {object_type<List<Double>>()})}),
      new Type(arrow_type {new Type(list_type {X}), new Type(list_type {Y})})}};
    auto result    = unify(cs);

    SECTION("reunify")
    {
      for (auto&& ta : result) {
        cs = subst_constr_all(ta, cs);
      }
      REQUIRE(unify(cs).empty());
    }

    SECTION("eq")
    {
      std::vector ans = {type_arrow {X, object_type<Int>()},
                         type_arrow {Y, object_type<List<Double>>()}};
      print_tyarrows(result);
      print_tyarrows(ans);
      CHECK(eq_arrows(result, ans));
    }
  }

  SECTION("[]")
  {
    std::vector<type_constr> cs;
    auto result = unify(cs);
    REQUIRE(result.empty());
  }
}

TEST_CASE("type_of")
{
  SECTION("type")
  {
    auto t    = make_object<Type>(value_type {});
    auto type = type_of(t);
    REQUIRE(same_type(type, object_type<Type>()));
  }

  SECTION("value")
  {
    auto i    = make_object<Int>();
    auto type = type_of(i);
    REQUIRE(same_type(type, object_type<Int>()));
  }

  SECTION("arrow")
  {
    {
      struct A : Function<A, closure<Int, Int>, Int>
      {
        return_type code() const
        {
          throw;
        }
      };
      auto a    = make_object<A>();
      auto type = type_of(a);
      REQUIRE(same_type(type, object_type<closure<closure<Int, Int>, Int>>()));
    }
  }

  SECTION("List")
  {
    {
      struct F : Function<F, List<Int>, List<Int>>
      {
        return_type code() const
        {
          throw;
        }
      };

      auto f    = make_object<F>();
      auto type = type_of(f);
      REQUIRE(same_type(type, object_type<closure<list<Int>, list<Int>>>()));
    }
  }

  SECTION("apply")
  {
    SECTION("simple")
    {
      struct A : Function<A, Int, Double>
      {
        return_type code() const
        {
          throw;
        }
      };

      auto a = make_object<A>();
      auto i = make_object<Int>();

      SECTION("Apply")
      {
        object_ptr app = new Apply {a, i};
        auto type      = type_of(app);
        REQUIRE(same_type(type, object_type<Double>()));
      }

      SECTION("Apply")
      {
        object_ptr app = make_object<Apply>(a, i);
        auto type      = type_of(app);
        REQUIRE(same_type(type, object_type<Double>()));
      }

      SECTION("operator<<")
      {
        auto app  = a << i;
        auto type = type_of(app);
        REQUIRE(same_type(type, object_type<Double>()));
      }
    }
  }
  SECTION("higher-order")
  {
    struct A : Function<A, closure<Double, Int>, Double, Int>
    {
      return_type code() const
      {
        throw;
      }
    };

    struct B : Function<B, Double, Int>
    {
      return_type code() const
      {
        throw;
      }
    };

    auto a = make_object<A>();
    auto b = make_object<B>();
    auto i = make_object<Int>();
    auto d = make_object<Double>();

    SECTION("Apply")
    {
      object_ptr app = new Apply {new Apply {a, b}, d};
      auto type      = type_of(app);
      REQUIRE(same_type(type, object_type<Int>()));
    }

    SECTION("operator<<")
    {
      auto app  = a << b << d;
      auto type = type_of(app);
      REQUIRE(same_type(type, object_type<Int>()));
    }
  }

  SECTION("polymorphic")
  {

#if defined(__clang__)
    class X;
#endif

    struct A
      : Function<A, Bool, forall<class X>, forall<class X>, forall<class X>>
    {
      return_type code() const
      {
        throw;
      }
    };

    auto a = make_object<A>();
    auto b = make_object<Bool>();
    auto i = make_object<Int>();
    auto d = make_object<Double>();

    SECTION("mono")
    {
      auto app  = a << b << i << i;
      auto type = type_of(app);
      REQUIRE(same_type(type, object_type<Int>()));
    }

    SECTION("poly")
    {
      auto app  = a << b << (a << b << d << d) << d;
      auto type = type_of(app);
      REQUIRE(same_type(type, object_type<Double>()));
    }
  }

  SECTION("fix")
  {

    struct A : Function<A, closure<Int, Double>, closure<Int, Double>>
    {
      return_type code() const
      {
        throw;
      }
    };

    struct B : Function<B, closure<Int, Double>, Int, Double>
    {
      return_type code() const
      {
        throw;
      }
    };

#if defined(__clang__)
    class X;
    class Y;
#endif

    struct C : Function<
                 C,
                 closure<forall<class X>, forall<class Y>>,
                 forall<class X>,
                 forall<class Y>>
    {
      return_type code() const
      {
        throw;
      }
    };

    auto a   = make_object<A>();
    auto b   = make_object<B>();
    auto c   = make_object<C>();
    auto fix = make_object<Fix>();

    SECTION("a")
    {
      auto app  = fix << a;
      auto type = type_of(app);
      REQUIRE(same_type(type, object_type<closure<Int, Double>>()));
    }

    SECTION("b")
    {
      auto app  = fix << b;
      auto type = type_of(app);
      REQUIRE(same_type(type, object_type<closure<Int, Double>>()));
    }

    SECTION("c")
    {
      auto app  = fix << c;
      auto type = type_of(app);
      INFO(to_string(type));
      INFO(to_string(object_type<closure<forall<X>, forall<Y>>>()));
    }
  }
}

TEST_CASE("Undefined")
{
  SECTION("object_type")
  {
    auto t = object_type<Undefined>();
    REQUIRE(t);
  }
  SECTION("get_type")
  {
    auto t = get_type(nullptr);
    REQUIRE(same_type(t, object_type<Undefined>()));
  }
  SECTION("type_of")
  {
    auto t = type_of(nullptr);
    REQUIRE(same_type(t, object_type<Undefined>()));
  }
}

TEST_CASE("copy_type")
{
  SECTION("value")
  {
    auto tp  = object_type<Int>();
    auto cpy = copy_type(tp);
    REQUIRE(same_type(tp, cpy));
  }
  SECTION("var")
  {
    auto tp  = genvar();
    auto cpy = copy_type(tp);
    REQUIRE(same_type(tp, cpy));
  }
  SECTION("arrow")
  {
    struct F : Function<F, Double, Int>
    {
      return_type code() const
      {
        throw;
      }
    };
    auto tp  = object_type<F>();
    auto cpy = copy_type(tp);
    REQUIRE(same_type(tp, cpy));
  }
  SECTION("list")
  {
    struct F : Function<F, List<Int>, List<List<Int>>>
    {
      return_type code() const
      {
        throw;
      }
    };
    auto tp  = object_type<F>();
    auto cpy = copy_type(tp);
    REQUIRE(same_type(tp, cpy));
  }
  SECTION("combined")
  {
    struct F : Function<F, closure<class X, Int>, Double, List<class X>>
    {
      return_type code() const
      {
        throw;
      }
    };

    auto tp  = object_type<F>();
    auto cpy = copy_type(tp);
    REQUIRE(same_type(tp, cpy));
  }
}

TEST_CASE("lambda")
{
  SECTION("lx.x")
  {
    auto var                = make_object<Variable>();
    auto lam                = make_object<Lambda>();
    _get_storage(*lam).var  = var;
    _get_storage(*lam).body = var;

    REQUIRE(_get_storage(lam).is_lambda());
    REQUIRE(_get_storage(var).is_variable());
    REQUIRE(value_cast_if<Lambda>(lam));
    REQUIRE(value_cast_if<Variable>(var));

    auto ty = type_of(lam);
    fmt::print("lx.x : {}\n", to_string(ty));

    auto app = lam << make_object<Int>(1);
    ty       = type_of(app);
    fmt::print("(lx.x) 1 : {}\n", to_string(ty));
    REQUIRE(same_type(ty, object_type<Int>()));
  }

  SECTION("lx.id x")
  {
    auto var                = make_object<Variable>();
    auto lam                = make_object<Lambda>();
    auto id                 = make_object<Identity>();
    _get_storage(*lam).var  = var;
    _get_storage(*lam).body = (id << var);

    auto ty = type_of(lam);
    fmt::print("lx.id x : {}\n", to_string(ty));

    auto app = lam << make_object<Int>(1);
    ty       = type_of(app);
    fmt::print("(lx.id x) 1 : {}\n", to_string(ty));
    REQUIRE(same_type(ty, object_type<Int>()));
  }

  SECTION("lx.id (id x)")
  {
    auto var                = make_object<Variable>();
    auto lam                = make_object<Lambda>();
    auto id                 = make_object<Identity>();
    _get_storage(*lam).var  = var;
    _get_storage(*lam).body = (id << (id << var));

    auto ty = type_of(lam);
    fmt::print("lx.id (id x) : {}\n", to_string(ty));

    auto app = lam << make_object<Int>(1);
    ty       = type_of(app);
    fmt::print("(lx.id (id x)) 1 : {}\n", to_string(ty));
    REQUIRE(same_type(ty, object_type<Int>()));
  }

  SECTION("lx.ly. id ((id y) (id x))")
  {
    auto var1 = make_object<Variable>();
    auto var2 = make_object<Variable>();
    auto lam1 = make_object<Lambda>();
    auto lam2 = make_object<Lambda>();
    auto id   = make_object<Identity>();

    _get_storage(*lam1).var = var1;
    _get_storage(*lam2).var = var2;

    _get_storage(*lam1).body = (id << var1) << (id << var2);
    _get_storage(*lam2).body = id << lam1;

    auto ty = type_of(lam2);
    fmt::print("lx.ly.id ((id y) (id x)): {}\n", to_string(ty));

    auto app = lam2 << make_object<Int>(1) << (id << id);
    ty       = type_of(app);
    fmt::print("(lx.ly.id ((id y) (id x)) 1 (id id): {}\n", to_string(ty));
    REQUIRE(same_type(ty, object_type<Int>()));
  }

  SECTION("lf.lx. f (f x)")
  {
    auto var1 = make_object<Variable>();
    auto var2 = make_object<Variable>();
    auto lam  = make_object<Lambda>(
      var1, make_object<Lambda>(var2, var1 << (var1 << var2)));

    // (x->x) -> (x->x)
    auto ty = type_of(lam);
    fmt::print("lf.lx.f (f x): {}\n", to_string(ty));

    struct F : Function<F, Int, Int>
    {
      return_type code() const
      {
        throw;
      }
    };

    auto app = lam << make_object<F>();
    REQUIRE(same_type(type_of(app), object_type<closure<Int, Int>>()));
  }

  SECTION("lx.((ly.x) 1)")
  {
    auto var1 = make_object<Variable>();
    auto var2 = make_object<Variable>();
    auto lam1 = make_object<Lambda>();
    auto lam2 = make_object<Lambda>();
    auto id   = make_object<Identity>();

    _get_storage(*lam1).var = var1;
    _get_storage(*lam2).var = var2;

    _get_storage(*lam1).body = var2;
    _get_storage(*lam2).body = (lam1 << make_object<Int>());

    auto ty = type_of(lam2);
    fmt::print("lx.((ly.x) 1): {}\n", to_string(ty));

    auto app = lam2 << make_object<Int>(1);
    ty       = type_of(app);
    fmt::print("(lx.((ly.x) 1)) 1: {}\n", to_string(ty));
    REQUIRE(same_type(ty, object_type<Int>()));
  }

  SECTION("lx.h (f x) (g x)")
  {
    using namespace yave;

    struct H : Function<H, Int, Double, Float>
    {
      auto code() const -> return_type
      {
        throw;
      }
    };

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

    auto h   = make_object<H>();
    auto f   = make_object<F>();
    auto g   = make_object<G>();
    auto var = make_object<Variable>();
    auto lam = make_object<Lambda>();
    auto id  = make_object<Identity>();

    _get_storage(*lam).var  = var;
    _get_storage(*lam).body = (id << h) << (id << (f << var)) << (g << var);

    try {
      auto ty = type_of(lam);
      REQUIRE(false);
    } catch (type_error::type_missmatch& e) {
      REQUIRE(same_type(e.expected(), object_type<Double>()));
      REQUIRE(same_type(e.provided(), object_type<Int>()));
    }
  }
}