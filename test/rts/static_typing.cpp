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
  using Float = yave::Box<float>;
  using Bool = yave::Box<bool>;
} // namespace yave

YAVE_DECL_TYPE(Int);
YAVE_DECL_TYPE(Double);
YAVE_DECL_TYPE(Float);
YAVE_DECL_TYPE(Bool);

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
    constexpr auto arrow = type_c<tyarrow<var<taggen<0>>, value<int>>>;
    constexpr auto type = type_c<var<taggen<0>>>;
    static_assert(subst(arrow, type) == type_c<value<int>>);
  }
  {
    constexpr auto ar = type_c<tyarrow<var<taggen<0>>, value<int>>>;
    constexpr auto type = type_c<arrow<var<taggen<0>>, value<int>>>;
    static_assert(subst(ar, type) == type_c<arrow<value<int>, value<int>>>);
  }
}

void test_subst_all()
{
  {
    constexpr auto ars = tuple_c<
      tyarrow<var<taggen<0>>, value<int>>,
      tyarrow<var<taggen<1>>, value<float>>>;

    constexpr auto type = type_c<arrow<
      arrow<var<taggen<1>>, var<taggen<0>>>,
      arrow<value<float>, var<taggen<1>>>>>;

    static_assert(
      subst_all(ars, type) == type_c<arrow<
                                arrow<value<float>, value<int>>,
                                arrow<value<float>, value<float>>>>);
  }
}

void test_unify()
{
  {
    // [X -> int]
    constexpr auto c = tuple_c<constr<var<class X>, value<int>>>;
    static_assert(
      set_c<tyarrow<var<class X>, value<int>>> == make_set(unify(c, true_c)));
  }
  {
    // [X = int, Y = X -> X]
    constexpr auto c = tuple_c<
      constr<var<class X>, value<int>>,
      constr<var<class Y>, arrow<var<class X>, var<class X>>>>;

    static_assert(
      set_c<
        tyarrow<var<class X>, value<int>>,
        tyarrow<var<class Y>, arrow<value<int>, value<int>>>> ==
      make_set(unify(c, true_c)));
  }
  {
    // [int->int = X -> Y]
    constexpr auto c = tuple_c<
      constr<arrow<value<int>, value<int>>, arrow<var<class X>, var<class Y>>>>;

    static_assert(
      set_c<
        tyarrow<var<class X>, value<int>>,
        tyarrow<var<class Y>, value<int>>> == make_set(unify(c, true_c)));
  }
  {
    // [int = int -> Y]
    constexpr auto c = tuple_c<constr<value<int>, arrow<value<int>, var<class Y>>>>;
    constexpr auto r = unify(c, false_c);
    static_assert(is_error_type(r));
    // using r = unify_t<c>; // should fail
  }
  {
    // [Y = int -> Y]
    constexpr auto c = tuple_c<constr<var<class Y>, arrow<value<int>, var<class Y>>>>;
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

void test_type_of()
{
  {
    constexpr auto tp = type_c<tm_value<int>>;
    static_assert(type_of(tp) == make_value(type_c<int>));
  }
  {
    constexpr auto tp = type_c<tm_varvalue<int>>;
    static_assert(type_of(tp) == make_varvalue(type_c<int>));
  }
  {
    constexpr auto tp = type_c<tm_closure<tm_value<int>, tm_value<float>>>;
    static_assert(type_of(tp) == type_c<arrow<value<int>, value<float>>>);
  }
  {
    constexpr auto tp = type_c<tm_closure<
      tm_value<int>,
      tm_closure<tm_value<float>, tm_var<taggen<0>>>>>;

    static_assert(
      type_of(tp) ==
      type_c<arrow<value<int>, arrow<value<float>, var<taggen<0>>>>>);
  }
  {
    // apply
    // (Int -> Int) Int = Int
    constexpr auto tm1 =
      type_c<tm_apply<tm_closure<tm_value<int>, tm_value<int>>, tm_value<int>>>;
    static_assert(type_of(tm1) == type_c<value<int>>);
  }
  {
    // higher-order
    // (Double -> Int -> Int) (Double -> Int) = Int
    constexpr auto tm1 = type_c<tm_apply<
      tm_closure<tm_closure<tm_value<double>, tm_value<int>>, tm_value<int>>,
      tm_closure<tm_value<double>, tm_value<int>>>>;
    static_assert(type_c<value<int>> == type_of(tm1));
  }
  {
    // (Double -> X -> X) (Double -> Int) = Int
    constexpr auto tm1 = type_c<tm_apply<
      tm_closure<
        tm_closure<tm_value<double>, tm_var<class X>>,
        tm_var<class X>>,
      tm_closure<tm_value<double>, tm_value<int>>>>;
    static_assert(type_c<value<int>> == type_of(tm1));
  }
  {
    // ((X -> X) -> (X -> X)) (Int -> Int) = Int -> Int
    using doubleapp = tm_closure<
      tm_closure<tm_var<class X>, tm_var<class X>>,
      tm_closure<tm_var<class X>, tm_var<class X>>>;

    constexpr auto tm =
      type_c<tm_apply<doubleapp, tm_closure<tm_value<int>, tm_value<int>>>>;

    static_assert(type_c<arrow<value<int>, value<int>>> == type_of(tm));
  }
  {
    // fix ((int -> bool) -> (int -> bool)) = int -> bool
    constexpr auto ff = type_c<tm_apply<
      tm_closure<tm_closure<tm_var<class X>, tm_var<class X>>, tm_var<class X>>,
      tm_closure<
        tm_closure<tm_value<int>, tm_value<bool>>,
        tm_value<int>,
        tm_value<bool>>>>;

    static_assert(type_of(ff) == type_c<arrow<value<int>, value<bool>>>);
  }
  {
    static_assert(type_of(type_c<tm_var<class X>>) == type_c<var<class X>>);
  }
  {
    // (Int->X) X = X
    constexpr auto term = type_c<
      tm_apply<tm_closure<tm_value<Int>, tm_var<class X>>, tm_value<Int>>>;

    static_assert(type_c<var<taggen<0>>> == type_of(term));
  }
  {
    // (Int -> Double) Int = Double
    constexpr auto term = type_c<
      tm_apply<tm_closure<tm_value<Int>, tm_value<Double>>, tm_value<Int>>>;
    static_assert(type_c<value<Double>> == type_of(term));
  }
  {
    // (X -> Y -> Int) Double -> Z(placeholder)
    constexpr auto term = type_c<tm_apply<
      tm_apply<tm_closure<tm_var<class XX>, tm_var<class YY>>, tm_value<Int>>,
      tm_value<Double>>>;
    static_assert(type_c<var<taggen<3>>> == type_of(term));
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
    // value<T> -> T
    static_assert(guess_object_type(type_c<value<Int>>) == type_c<Int>);
  }
  {
    // varvalue<T> -> VarValurProxy<T>
    static_assert(
      guess_object_type(type_c<varvalue<class Tag>>) ==
      type_c<VarValueProxy<class Tag>>);
  }
  {
    // var<T> -> VarValurProxy<T>
    static_assert(
      guess_object_type(type_c<var<class Tag>>) ==
      type_c<VarValueProxy<class Tag>>);
  }
  {
    // arrow<S, T> -> closure<S, T>
    static_assert(
      guess_object_type(type_c<arrow<value<Double>, value<Int>>>) ==
      type_c<ClosureProxy<Double, Int>>);

    // arrow<S, arrow<T, U>> -> closure<S, T, U>
    static_assert(
      guess_object_type(
        type_c<arrow<value<Int>, arrow<value<Double>, value<Int>>>>) ==
      type_c<ClosureProxy<Int, Double, Int>>);

    // arrow<arrow<Double, Int>, arrow<Double, Int>> -> closure<closure<Double,
    // Int>, Double, Int>
    static_assert(
      guess_object_type(type_c<arrow<
                          arrow<value<Double>, value<Int>>,
                          arrow<value<Double>, value<Int>>>>) ==
      type_c<ClosureProxy<ClosureProxy<Double, Int>, Double, Int>>);
  }
}