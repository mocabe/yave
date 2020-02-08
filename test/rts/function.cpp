//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <catch2/catch.hpp>

#include <yave/rts/function.hpp>
#include <yave/rts/eval.hpp>
#include <yave/rts/unit.hpp>
#include <yave/rts/list.hpp>

#include <iostream>

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

TEST_CASE("simple function test")
{
  SECTION("Int->Int")
  {
    struct F : Function<F, Int, Int>
    {
      return_type code() const
      {
        static_assert(
          type_c<argument_proxy_t<0>> == type_c<const ObjectProxy<Int>>);
        return arg<0>();
        return eval_arg<0>();
        return eval(arg<0>());
        return eval(eval_arg<0>());
        return make_object<Int>();
        return make_object<const Int>();
        return new (std::pmr::new_delete_resource()) Int();
      }
    };
    auto f = make_object<F>();
  }
}

TEST_CASE("higher order function test")
{
  SECTION("(Int->Int) -> (Int->Int)")
  {
    struct F : Function<F, closure<Int, Int>, closure<Int, Int>>
    {
      return_type code() const
      {
        static_assert(
          type_c<argument_proxy_t<0>> ==
          type_c<
            const ClosureArgumentProxy<ObjectProxy<Int>, ObjectProxy<Int>>>);
        return arg<0>();
        return eval_arg<0>();
        return eval(arg<0>());
        return eval(eval_arg<0>());
      }
    };
    auto f = make_object<F>();
  }
}

TEST_CASE("static apply test")
{
  SECTION("")
  {
    struct F : Function<F, closure<Int, Int>, Int, Int>
    {
      return_type code() const
      {
        // (Int->Int)Int
        return arg<0>() << arg<1>();
        return eval_arg<0>() << arg<1>();
        return arg<0>() << eval_arg<1>();
        return eval_arg<0>() << eval_arg<1>();
        return eval(arg<0>() << arg<1>());

        // (Int->Int)((Int->Int)Int)
        return arg<0>() << (arg<0>() << arg<1>());
        return arg<0>() << eval(arg<0>() << arg<1>());
        return arg<0>() << (eval_arg<0>() << arg<1>());
        return eval_arg<0>() << (arg<0>() << arg<1>());
        return eval_arg<0>() << eval(arg<0>() << arg<1>());
        return eval_arg<0>() << (eval_arg<0>() << arg<1>());
      }
    };
    auto f = make_object<F>();
  }
}

TEST_CASE("polymorphic function test")
{
  SECTION("polymorphic closure declaration")
  {
    class X;
    struct F : Function<F, Int, closure<Int, forall<X>>, forall<X>>
    {
      return_type code() const
      {
        static_assert(
          type_c<argument_proxy_t<1>> ==
          type_c<
            const ClosureArgumentProxy<ObjectProxy<Int>, VarValueProxy<X>>>);
        return arg<1>() << arg<0>();
      }
    };
    auto f = make_object<F>();
  }

  SECTION("polymorphic closure declaration")
  {
    class X;
    struct F : Function<F, Int, closure<Int, X>, X>
    {
      return_type code() const
      {
        static_assert(
          type_c<argument_proxy_t<1>> ==
          type_c<
            const ClosureArgumentProxy<ObjectProxy<Int>, VarValueProxy<X>>>);
        return arg<1>() << arg<0>();
      }
    };
    auto f = make_object<F>();
  }

  SECTION("polymorphic return type")
  {
    class X;
    struct If : Function<If, Bool, forall<X>, forall<X>, forall<X>>
    {
      return_type code() const
      {
        static_assert(
          type_c<argument_proxy_t<1>> == type_c<argument_proxy_t<2>>);
        static_assert(
          type_c<argument_proxy_t<1>> == type_c<const VarValueProxy<X>>);

        if (*eval_arg<0>())
          return arg<1>();
        else
          return arg<2>();
      }
    };
    auto _if = make_object<If>();

    SECTION("static polymorphic apply")
    {
      struct F : Function<F, Unit, Int>
      {
        return_type code() const
        {
          auto _if = make_object<If>();
          auto b   = make_object<Bool>();
          auto i   = make_object<Int>();
          return _if << b << i << i; // Bool->X->X->X Bool Int Int
        }
      };
      auto f = make_object<F>();
    }

    SECTION("static polymorphic apply 2")
    {
      // static polymorphic apply
      struct F : Function<F, closure<Double, Int, Int>, Int>
      {
        return_type code() const
        {
          auto _if = make_object<If>();
          auto b   = make_object<Bool>();
          auto i   = make_object<Int>();
          auto d   = make_object<Double>();
          // (Double->Int->Int) ((Bool->X->X->X) Bool Double Double)
          // ((Bool->X->X->X) Bool Int Int)
          return arg<0>() << (_if << b << d << d) << (_if << b << i << i);
        }
      };
      auto f = make_object<F>();
    }
  }
}

TEST_CASE("List<T>")
{
  SECTION("List head")
  {
    struct F : Function<F, List<Int>, Int>
    {
      return_type code() const
      {
        return eval_arg<0>()->head();
      }
    };

    auto f = make_object<F>();
  }

  SECTION("List tail")
  {
    struct F : Function<F, List<Int>, List<Int>>
    {
      return_type code() const
      {
        return eval_arg<0>()->tail();
        return eval(arg<0>())->tail();
      }
    };

    auto f = make_object<F>();
  }

  SECTION("List cons")
  {
    struct F : Function<F, Int, List<Int>, List<Int>>
    {
      return_type code() const
      {
        return make_object<List<Int>>(arg<0>(), arg<1>());
        return make_object<List<Int>>(eval_arg<0>(), arg<1>());
        return make_object<List<Int>>(arg<0>(), eval_arg<1>());
        return make_object<List<Int>>(eval_arg<0>(), eval_arg<1>());
        return make_object<List<Int>>(eval_arg<0>(), eval_arg<1>()->tail());
        return make_object<List<Int>>(eval_arg<1>()->head(), eval_arg<1>());
        return make_object<List<Int>>(
          eval_arg<1>()->head(), eval_arg<1>()->tail());
        return make_list<Int>(arg<0>(), eval_arg<0>(), eval_arg<1>()->head());
      }
    };

    auto f = make_object<F>();
  }

  SECTION("List misc")
  {
    struct F : Function<F, closure<Int, List<Int>>, Int, Int>
    {
      return_type code() const
      {
        return eval(arg<0>() << arg<1>())->head();
        return eval(eval_arg<0>() << arg<1>())->head();
        return eval(eval_arg<0>() << eval_arg<1>())->head();
        return eval(arg<0>() << eval_arg<1>())->head();
      }
    };

    auto f = make_object<F>();
  }

  SECTION("List poly")
  {
    class X;
    struct F
      : Function<F, closure<class X, List<class X>>, class X, List<class X>>
    {
      return_type code() const
      {
        return arg<0>() << arg<1>();
        return eval(arg<0>() << arg<1>());
        return eval(arg<0>() << arg<1>())->tail();
      }
    };

    struct G : Function<G, class X, List<class X>>
    {
      return_type code() const
      {
        return make_list<class X>(arg<0>());
        return make_list<class X>(eval_arg<0>());
      }
    };

    struct H : Function<H, F, G, List<Double>>
    {
      return_type code() const
      {
        return arg<0>() << arg<1>() << make_object<Double>();
        return eval_arg<0>() << arg<1>() << make_object<Double>();
        return eval_arg<0>() << eval_arg<1>() << make_object<Double>();
        return make_object<F>() << arg<1>() << make_object<Double>();
        return make_object<F>() << make_object<G>() << make_object<Double>();
      }
    };

    auto f = make_object<F>();
    auto g = make_object<G>();
    auto h = make_object<H>();
  }
}
