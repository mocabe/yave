//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <catch2/catch.hpp>

#include <yave/rts/static_typing.hpp>
#include <yave/rts/type_gen.hpp>

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

void test_tuple_eq()
{
  {
    auto t = tuple_c<>;
    static_assert(equal(t, t));
    static_assert(t == t);
  }
  {
    auto t = tuple_c<int>;
    static_assert(equal(t, t));
    static_assert(t == t);
    static_assert(t != tuple_c<double>);
  }
  {
    auto t = tuple_c<int, double>;
    static_assert(equal(t, t));
    static_assert(t == t);

    static_assert(t != tuple_c<>);
    static_assert(t != tuple_c<double, int>);
    static_assert(t == tuple_c<int, double>);
  }
}

void test_tuple_head()
{
  {
    auto t = tuple_c<int>;
    static_assert(type_c<int> == head(t));
  }
  {
    auto t = tuple_c<int, double>;
    static_assert(type_c<int> == head(t));
  }
}

void test_tuple_tail()
{
  {
    auto t = tuple_c<>;
    static_assert(t == tail(t));
  }
  {
    auto t = tuple_c<int, double>;
    static_assert(tail(t) == tuple_c<double>);
  }
  {
    auto t = tuple_c<int, double, float>;
    static_assert(tail(t) == tuple_c<double, float>);
  }
}

void test_tuple_last()
{
  {
    auto t = tuple_c<int, float, double>;
    static_assert(last(t) == type_c<double>);
  }
  {
    auto t = tuple_c<int>;
    static_assert(last(t) == type_c<int>);
  }
}

void test_tuple_get()
{
  {
    auto t = tuple_c<int, double, float>;
    static_assert(get<0>(t) == type_c<int>);
    static_assert(get<2>(t) == type_c<float>);
  }
}

void test_tuple_append()
{
  {
    static_assert(append(type_c<int>, tuple_c<>) == tuple_c<int>);
  }
  {
    static_assert(append(type_c<int>, tuple_c<double>) == tuple_c<double, int>);
  }
}

void test_tuple_concat()
{
  {
    static_assert(concat(tuple_c<>, tuple_c<>) == tuple_c<>);
  }
  {
    static_assert(concat(tuple_c<int>, tuple_c<>) == tuple_c<int>);
    static_assert(concat(tuple_c<>, tuple_c<int>) == tuple_c<int>);
  }
  {
    static_assert(concat(tuple_c<int>, tuple_c<float>) == tuple_c<int, float>);
  }
  {
    static_assert(
      concat(tuple_c<int, float>, tuple_c<double, short>) ==
      tuple_c<int, float, double, short>);
  }
}

void test_tuple_remove_last()
{
  {
    static_assert(remove_last(tuple_c<>) == tuple_c<>);
  }
  {
    static_assert(remove_last(tuple_c<int>) == tuple_c<>);
  }
  {
    static_assert(remove_last(tuple_c<int, float>) == tuple_c<int>);
  }
}

void test_set_eq()
{
  {
    static_assert(equal(set_c<>, set_c<>));
    static_assert(set_c<> == set_c<>);
  }
  {
    static_assert(equal(set_c<int>, set_c<int>));
    static_assert(set_c<int> == set_c<int>);
    static_assert(set_c<int> != set_c<double>);
  }
  {
    static_assert(equal(set_c<int, int>, set_c<int>));
    static_assert(set_c<int, int> == set_c<int>);
    static_assert(set_c<int, double> != set_c<int>);
  }
}

void test_set_insert()
{
  {
    auto s = set_c<>;
    static_assert(insert(type_c<int>, s) == set_c<int>);
  }
  {
    auto s = set_c<int, double>;
    static_assert(insert(type_c<float>, s) == set_c<int, float, double>);
  }
  {
    auto s = set_c<int, double>;
    static_assert(insert(type_c<int>, s) == s);
  }
}

void test_subst()
{
  {
    constexpr auto arrow = type_c<tyarrow<ty_var<taggen<0>>, ty_value<int>>>;
    constexpr auto type  = type_c<ty_var<taggen<0>>>;
    static_assert(subst(arrow, type) == type_c<ty_value<int>>);
  }
  {
    constexpr auto ar   = type_c<tyarrow<ty_var<taggen<0>>, ty_value<int>>>;
    constexpr auto type = type_c<ty_arrow<ty_var<taggen<0>>, ty_value<int>>>;
    static_assert(
      subst(ar, type) == type_c<ty_arrow<ty_value<int>, ty_value<int>>>);
  }
}

void test_subst_all()
{
  {
    constexpr auto ars = tuple_c<
      tyarrow<ty_var<taggen<0>>, ty_value<int>>,
      tyarrow<ty_var<taggen<1>>, ty_value<float>>>;

    constexpr auto type = type_c<ty_arrow<
      ty_arrow<ty_var<taggen<1>>, ty_var<taggen<0>>>,
      ty_arrow<ty_value<float>, ty_var<taggen<1>>>>>;

    static_assert(
      subst_all(ars, type) == type_c<ty_arrow<
                                ty_arrow<ty_value<float>, ty_value<int>>,
                                ty_arrow<ty_value<float>, ty_value<float>>>>);
  }
}

void test_unify()
{
  {
    // [X -> int]
    constexpr auto c = tuple_c<constr<ty_var<class X>, ty_value<int>>>;
    static_assert(
      set_c<tyarrow<ty_var<class X>, ty_value<int>>> ==
      make_set(unify(c, true_c)));
  }
  {
    // [X = int, Y = X -> X]
    constexpr auto c = tuple_c<
      constr<ty_var<class X>, ty_value<int>>,
      constr<ty_var<class Y>, ty_arrow<ty_var<class X>, ty_var<class X>>>>;

    static_assert(
      set_c<
        tyarrow<ty_var<class X>, ty_value<int>>,
        tyarrow<ty_var<class Y>, ty_arrow<ty_value<int>, ty_value<int>>>> ==
      make_set(unify(c, true_c)));
  }
  {
    // [int->int = X -> Y]
    constexpr auto c = tuple_c<constr<
      ty_arrow<ty_value<int>, ty_value<int>>,
      ty_arrow<ty_var<class X>, ty_var<class Y>>>>;

    static_assert(
      set_c<
        tyarrow<ty_var<class X>, ty_value<int>>,
        tyarrow<ty_var<class Y>, ty_value<int>>> == make_set(unify(c, true_c)));
  }
  {
    // [int = int -> Y]
    constexpr auto c =
      tuple_c<constr<ty_value<int>, ty_arrow<ty_value<int>, ty_var<class Y>>>>;
    constexpr auto r = unify(c, false_c);
    static_assert(is_error_type(r));
    // using r = unify_t<c>; // should fail
  }
  {
    // [Y = int -> Y]
    constexpr auto c = tuple_c<
      constr<ty_var<class Y>, ty_arrow<ty_value<int>, ty_var<class Y>>>>;
    constexpr auto r = unify(c, false_c);
    static_assert(is_error_type(r));
    // using r = unify_t<c>; // should fail
  }
  {
    // []
    constexpr auto c = tuple_c<>;
    static_assert(set_c<> == make_set(unify(c, true_c)));
  }
}

template <class... Ts>
struct F : Object
{
  static constexpr auto term = get_term<closure<Ts...>>();
};

template <class T>
struct L : Object
{
  static constexpr auto term = get_term<list<T>>();
};

void test_get_term()
{
  {
    static_assert(get_term<Int>() == type_c<tm_value<Int>>);
  }
  {
    static_assert(get_term<object<Int>>() == type_c<tm_value<Int>>);
  }
  {
    static_assert(
      get_term<F<Int, Double>>() ==
      type_c<tm_closure<tm_value<Int>, tm_value<Double>>>);
  }
  {
    static_assert(
      get_term<closure<Int, object<Double>>>() ==
      type_c<tm_closure<tm_value<Int>, tm_value<Double>>>);
  }
  {
    static_assert(
      get_term<closure<F<Int, Double>, Int>>() ==
      type_c<tm_closure<
        tm_closure<tm_value<Int>, tm_value<Double>>,
        tm_value<Int>>>);
  }
  {
    static_assert(get_term<L<Int>>() == type_c<tm_list<tm_value<Int>>>);
    static_assert(get_term<list<Int>>() == type_c<tm_list<tm_value<Int>>>);
    static_assert(
      get_term<L<closure<L<Int>, Double>>>() ==
      type_c<tm_list<tm_closure<tm_list<tm_value<Int>>, tm_value<Double>>>>);
  }
  {
    static_assert(
      get_term<list<closure<Int, Double>>>() ==
      type_c<tm_list<tm_closure<tm_value<Int>, tm_value<Double>>>>);
  }
}

void test_type_of()
{
  {
    constexpr auto tp = type_c<tm_value<int>>;
    static_assert(type_of(tp) == make_ty_value(type_c<int>));
  }
  {
    constexpr auto tp = type_c<tm_varvalue<int>>;
    static_assert(type_of(tp) == make_varvalue(type_c<int>));
  }
  {
    constexpr auto tp = type_c<tm_closure<tm_value<int>, tm_value<float>>>;
    static_assert(
      type_of(tp) == type_c<ty_arrow<ty_value<int>, ty_value<float>>>);
  }
  {
    constexpr auto tp = type_c<tm_closure<
      tm_value<int>,
      tm_closure<tm_value<float>, tm_var<taggen<0>>>>>;

    static_assert(
      type_of(tp) ==
      type_c<
        ty_arrow<ty_value<int>, ty_arrow<ty_value<float>, ty_var<taggen<0>>>>>);
  }
  {
    // apply
    // (Int -> Int) Int = Int
    constexpr auto tm1 =
      type_c<tm_apply<tm_closure<tm_value<int>, tm_value<int>>, tm_value<int>>>;
    static_assert(type_of(tm1) == type_c<ty_value<int>>);
  }
  {
    // higher-order
    // (Double -> Int -> Int) (Double -> Int) = Int
    constexpr auto tm1 = type_c<tm_apply<
      tm_closure<tm_closure<tm_value<double>, tm_value<int>>, tm_value<int>>,
      tm_closure<tm_value<double>, tm_value<int>>>>;
    static_assert(type_c<ty_value<int>> == type_of(tm1));
  }
  {
    // (Double -> X -> X) (Double -> Int) = Int
    constexpr auto tm1 = type_c<tm_apply<
      tm_closure<
        tm_closure<tm_value<double>, tm_var<class X>>,
        tm_var<class X>>,
      tm_closure<tm_value<double>, tm_value<int>>>>;
    static_assert(type_c<ty_value<int>> == type_of(tm1));
  }
  {
    // ((X -> X) -> (X -> X)) (Int -> Int) = Int -> Int
    using doubleapp = tm_closure<
      tm_closure<tm_var<class X>, tm_var<class X>>,
      tm_closure<tm_var<class X>, tm_var<class X>>>;

    constexpr auto tm =
      type_c<tm_apply<doubleapp, tm_closure<tm_value<int>, tm_value<int>>>>;

    static_assert(
      type_c<ty_arrow<ty_value<int>, ty_value<int>>> == type_of(tm));
  }
  {
    // fix ((int -> bool) -> (int -> bool)) = int -> bool
    constexpr auto ff = type_c<tm_apply<
      tm_closure<tm_closure<tm_var<class X>, tm_var<class X>>, tm_var<class X>>,
      tm_closure<
        tm_closure<tm_value<int>, tm_value<bool>>,
        tm_value<int>,
        tm_value<bool>>>>;

    static_assert(
      type_of(ff) == type_c<ty_arrow<ty_value<int>, ty_value<bool>>>);
  }
  {
    static_assert(type_of(type_c<tm_var<class X>>) == type_c<ty_var<class X>>);
  }
  {
    // (Int->X) X = X
    constexpr auto term = type_c<
      tm_apply<tm_closure<tm_value<Int>, tm_var<class X>>, tm_value<Int>>>;

    static_assert(type_c<ty_var<taggen<0>>> == type_of(term));
  }
  {
    // (Int -> Double) Int = Double
    constexpr auto term = type_c<
      tm_apply<tm_closure<tm_value<Int>, tm_value<Double>>, tm_value<Int>>>;
    static_assert(type_c<ty_value<Double>> == type_of(term));
  }
  {
    // (X -> Y -> Int) Double -> Z(placeholder)
    constexpr auto term = type_c<tm_apply<
      tm_apply<tm_closure<tm_var<class XX>, tm_var<class YY>>, tm_value<Int>>,
      tm_value<Double>>>;
    static_assert(type_c<ty_var<taggen<3>>> == type_of(term));
  }
  {
    // (X->X) Int = Int
    constexpr auto term = type_c<
      tm_apply<tm_closure<tm_var<class X>, tm_var<class X>>, tm_value<Int>>>;
    static_assert(type_c<ty_value<Int>> == type_of(term));
  }
  {
    // list<T> -> list<T>
    constexpr auto term = type_c<tm_list<tm_value<class Tag>>>;
    static_assert(type_c<ty_list<ty_value<class Tag>>> == type_of(term));
  }
  {
    // (X -> X -> list<X>) Int Int = list<Int>
    constexpr auto term = type_c<tm_apply<
      tm_apply<
        tm_closure<tm_var<class X>, tm_var<class X>, tm_list<tm_var<class X>>>,
        tm_value<Int>>,
      tm_value<Int>>>;
    static_assert(type_c<ty_list<ty_value<Int>>> == type_of(term));
  }
  {
    // (list<X> -> X) list<Int> = Int
    constexpr auto term = type_c<tm_apply<
      tm_closure<tm_list<tm_var<class X>>, tm_var<class X>>,
      tm_list<tm_value<Int>>>>;
    static_assert(type_c<ty_value<Int>> == type_of(term));
  }
}

void test_genpoly()
{
  {
    // Double -> Var<X> -> Var<X>
    constexpr auto term =
      closure_term_export(type_c<tm_closure<
                            tm_closure<tm_value<double>, tm_varvalue<class X>>,
                            tm_varvalue<class X>>>);

    // Double -> Var[0] -> Var[0]
    constexpr auto gterm = type_c<tm_closure<
      tm_closure<tm_value<double>, tm_var<taggen<0>>>,
      tm_var<taggen<0>>>>;

    static_assert(genpoly(term, gen_c<0>).first() == gterm);
  }
}

void test_assume_object_type()
{
  {
    // ty_value<T> -> T
    static_assert(guess_object_type(type_c<ty_value<Int>>) == type_c<Int>);
  }
  {
    // varty_value<T> -> VarValurProxy<T>
    static_assert(
      guess_object_type(type_c<varvalue<class Tag>>) ==
      type_c<VarValueProxy<class Tag>>);
  }
  {
    // ty_var<T> -> VarValurProxy<T>
    static_assert(
      guess_object_type(type_c<ty_var<class Tag>>) ==
      type_c<VarValueProxy<class Tag>>);
  }
  {
    // arrow<S, T> -> closure<S, T>
    static_assert(
      guess_object_type(type_c<ty_arrow<ty_value<Double>, ty_value<Int>>>) ==
      type_c<ClosureProxy<Double, Int>>);

    // arrow<S, arrow<T, U>> -> closure<S, T, U>
    static_assert(
      guess_object_type(type_c<ty_arrow<
                          ty_value<Int>,
                          ty_arrow<ty_value<Double>, ty_value<Int>>>>) ==
      type_c<ClosureProxy<Int, Double, Int>>);

    // arrow<arrow<Double, Int>, arrow<Double, Int>> -> closure<closure<Double,
    // Int>, Double, Int>
    static_assert(
      guess_object_type(type_c<ty_arrow<
                          ty_arrow<ty_value<Double>, ty_value<Int>>,
                          ty_arrow<ty_value<Double>, ty_value<Int>>>>) ==
      type_c<ClosureProxy<ClosureProxy<Double, Int>, Double, Int>>);
  }
}