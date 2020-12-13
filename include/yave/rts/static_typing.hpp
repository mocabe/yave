//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/meta_type.hpp>
#include <yave/rts/meta_tuple.hpp>
#include <yave/rts/meta_set.hpp>
#include <yave/rts/meta_pair.hpp>
#include <yave/rts/meta_result.hpp>
#include <yave/rts/terms.hpp>
#include <yave/rts/types.hpp>
#include <yave/rts/object.hpp>

namespace yave {

  // ------------------------------------------
  // apply_subst

  template <class TyT1Tag, class TyT1Kind, class TyT2, class T>
  [[nodiscard]] constexpr auto apply_tyarrow(
    meta_type<tyarrow<tvar<TyT1Tag, TyT1Kind>, TyT2>> s,
    meta_type<T> t)
  {
    if constexpr (t == s.t1()) {
      return s.t2();
    } else if constexpr (is_tap(t)) {
      return make_tap(apply_tyarrow(s, t.t1()), apply_tyarrow(s, t.t2()));
    } else
      return t;
  }

  template <class... TyArrows, class T>
  [[nodiscard]] constexpr auto apply_subst(
    meta_tuple<TyArrows...> s,
    meta_type<T> t)
  {
    if constexpr (empty(s))
      return t;
    else
      return apply_subst(tail(s), apply_tyarrow(head(s), t));
  }

  template <class... TyArrows, class... Ts>
  [[nodiscard]] constexpr auto apply_subst(
    meta_tuple<TyArrows...> s,
    meta_tuple<Ts...> ts)
  {
    return map(
      ts, [=](auto t) constexpr { return apply_subst(s, t); });
  }

  // ------------------------------------------
  // vars

  template <class T>
  [[nodiscard]] constexpr auto vars(meta_type<T> t)
  {
    if constexpr (is_tvar(t)) {
      return make_set(t);
    } else if constexpr (is_tap(t)) {
      return merge(vars(t.t1()), vars(t.t2()));
    } else
      return set_c<>;
  }

  template <class... Ts>
  [[nodiscard]] constexpr auto vars(meta_tuple<Ts...> ts)
  {
    return make_set(flatten(map(ts, [=](auto t) {
      // lift to meta_type<meta_tuple<...>>
      return type_c<decltype(make_tuple(vars(t)))>;
    })));
  }

  // ------------------------------------------
  // compose_subst

  template <class... TyArrows1, class... TyArrows2>
  [[nodiscard]] constexpr auto compose_subst(
    meta_tuple<TyArrows1...> s1,
    meta_tuple<TyArrows2...> s2)
  {
    return concat(
      map(
        s2,
        [=](auto s) { return make_tyarrow(s.t1(), apply_subst(s1, s.t2())); }),
      s1);
  }

  // ------------------------------------------
  // merge_subst

  template <class... TyArrows1, class... TyArrows2>
  [[nodiscard]] constexpr auto merge_subst(
    meta_tuple<TyArrows1...> s1,
    meta_tuple<TyArrows2...> s2)
  {
    constexpr auto fst = [=](auto s) { return s.t1(); };

    constexpr auto agree = [=](auto v) {
      return apply_subst(s1, v) == apply_subst(s2, v);
    };

    if constexpr (all(intersect(map(s1, fst), map(s2, fst)), agree))
      return make_succ(concat(s1, s2));
    else
      return make_error(make_unknown_error());
  }

  // ------------------------------------------
  // occurs

  template <class Tag, class Kind, class T>
  [[nodiscard]] constexpr auto occurs(
    meta_type<tvar<Tag, Kind>> u,
    meta_type<T> t)
  {
    (void)u;

    if constexpr (is_tap(t))
      return occurs(u, t.t1()) || occurs(u, t.t2());
    else if constexpr (is_tvar(t))
      return u == t;
    else if constexpr (is_tcon(t))
      return false_c;
    else
      static_assert(false_v<T>, "Invalid type");
  }

  // ------------------------------------------
  // mgu

  template <class Tag, class Kind, class T>
  [[nodiscard]] constexpr auto mgu_var(
    meta_type<tvar<Tag, Kind>> u,
    meta_type<T> t)
  {
    if constexpr (u == t)
      return make_succ(tuple_c<>);
    else if constexpr (occurs(u, t))
      return make_error(make_circular_constraints(u, t));
    else if constexpr (kind_of(u) != kind_of(t))
      return make_error(make_kind_missmatch(u, t));
    else
      return make_succ(tuple_c<tyarrow<tvar<Tag, Kind>, T>>);
  }

  /// most general unifier
  /// \returns succ of meta_tuple<tyarrow<T1, T2>...>, or err of tyerror
  template <class T1, class T2>
  [[nodiscard]] constexpr auto mgu(meta_type<T1> t1, meta_type<T2> t2)
  {
    // clang-format off
    if constexpr (is_tap(t1) && is_tap(t2))
      return mgu(t1.t1(), t2.t1())
        .and_then([=](auto s1) { return mgu(apply_subst(s1, t1.t2()), apply_subst(s1, t2.t2()))
        .and_then([=](auto s2) { return make_succ(compose_subst(s2, s1)); }); });
    // clang-format on
    else if constexpr (is_tvar(t1))
      return mgu_var(t1, t2);
    else if constexpr (is_tvar(t2))
      return mgu_var(t2, t1);
    else if constexpr (is_tcon(t1) && is_tcon(t2)) {
      if constexpr (t1 == t2)
        return make_succ(tuple_c<>);
      else
        return make_error(make_type_missmatch(t1, t2));
    } else
      return make_error(make_unsolvable_constraints(t1, t2));
  }

  // ------------------------------------------
  // match

  template <class T1, class T2>
  [[nodiscard]] constexpr auto match(meta_type<T1> t1, meta_type<T2> t2)
  {
    // clang-format off
    if constexpr (is_tap(t1) && is_tap(t2))
      return match(t1.t1(), t2.t1())
        .and_then([=](auto s1) { return match(t1.t2(), t2.t2())
        .and_then([=](auto s2) { return merge_subst(s1, s2); }); });
    // clang-format on
    else if constexpr (is_tvar(t1))
      return mgu_var(t1, t2);
    else if constexpr (is_tcon(t1) && is_tcon(t2))
      if constexpr (t1 == t2)
        return make_succ(tuple_c<>);
      else
        return make_error(make_type_missmatch(t1, t2));
    else
      return make_error(make_unsolvable_constraints(t1, t2));
  }

  // ------------------------------------------
  // unify

  /// emit static_assert for static type check errors
  template <class T>
  [[nodiscard]] constexpr auto unify_assert(T type)
  {
    constexpr auto tag = type.tag();
    if constexpr (is_type_missmatch(tag)) {
      static_assert(false_v<T>, "Unification error: Type missmatch");
      // print error info
      using left  = typename decltype(tag)::type::left::_show;
      using right = typename decltype(tag)::type::right::_show;
      using other = typename decltype(tag)::type::other::_show;
      static_assert(false_v<left, right, other>, "Unification failed.");
    } else if constexpr (is_unsolvable_constraints(tag)) {
      static_assert(false_v<T>, "Unification error: Unsolvable constraints");
      // print error info
      using left  = typename decltype(tag)::type::left::_show;
      using right = typename decltype(tag)::type::right::_show;
      using other = typename decltype(tag)::type::other::_show;
      static_assert(false_v<left, right, other>, "Unification failed.");
    } else if constexpr (is_circular_constraints(tag)) {
      static_assert(false_v<T>, "Unification error: Circular constraints");
      // print error info
      using var   = typename decltype(tag)::type::var::_show;
      using other = typename decltype(tag)::type::other::_show;
      static_assert(false_v<var, other>, "Unification failed.");
    } else if constexpr (is_unknown_error(tag)) {
      static_assert(
        false_v<T>,
        "Unification error: Unknown error(probably invalid constraints)");
    } else {
      static_assert(false_v<T>, "Invalid error tag for tyerror");
    }
    return type;
  }

  /// unification
  /// \returns meta_tuple<tyarrow<T1,T2>...> or tyerror<E>
  template <class T1, class T2, bool B>
  [[nodiscard]] constexpr auto unify(
    meta_type<T1> t1,
    meta_type<T2> t2,
    std::bool_constant<B> enable_assert)
  {
    (void)enable_assert;

    auto result = mgu(t1, t2);

    if constexpr (result.is_succ())
      return result.value();

    if constexpr (result.is_error()) {
      if constexpr (enable_assert)
        return unify_assert(result.error());
      else
        return result.error();
    }
  }

  // ------------------------------------------
  // subst_term

  template <class From, class To, class Term>
  constexpr auto subst_term_impl(
    meta_type<From>,
    meta_type<To>,
    meta_type<Term>)
  {
    static_assert(false_v<From, To, Term>, "Invalid type");
  }

  /// Replace type `From` with `To` in a `Term` tree.
  /// \param from a type going to be replaced
  /// \param to a type going to replace
  /// \param term term tree
  template <class From, class To, class Term>
  [[nodiscard]] constexpr auto subst_term(
    meta_type<From> from,
    meta_type<To> to,
    meta_type<Term> term)
  {
    if constexpr (from == term)
      return to;
    else
      return subst_term_impl(from, to, term);
  }

  template <class From, class To, class Tag>
  constexpr auto subst_term_impl(
    meta_type<From>,
    meta_type<To>,
    meta_type<tm_value<Tag>> term)
  {
    return term;
  }

  template <class From, class To, class Tag>
  constexpr auto subst_term_impl(
    meta_type<From>,
    meta_type<To>,
    meta_type<tm_varvalue<Tag>> term)
  {
    return term;
  }

  template <class From, class To, class Tag>
  constexpr auto subst_term_impl(
    meta_type<From>,
    meta_type<To>,
    meta_type<tm_var<Tag>> term)
  {
    return term;
  }

  template <class From, class To, class... Ts>
  constexpr auto subst_term_impl(
    meta_type<From> from,
    meta_type<To> to,
    meta_type<tm_closure<Ts...>>)
  {
    return make_tm_closure(subst_term(from, to, type_c<Ts>)...);
  }

  template <class From, class To, class T1, class T2>
  constexpr auto subst_term_impl(
    meta_type<From> from,
    meta_type<To> to,
    meta_type<tm_apply<T1, T2>> term)
  {
    return make_tm_apply(
      subst_term(from, to, term.t1()), subst_term(from, to, term.t2()));
  }

  // ------------------------------------------
  // genpoly

  template <class Term, class Gen, class Target>
  constexpr auto genpoly_impl(
    meta_type<Term> term,
    meta_type<Gen> gen,
    meta_type<Target> target)
  {
    (void)term;
    (void)gen;
    (void)target;
    static_assert(false_v<Term, Gen, Target>);
  }

  template <class... Ts, class Gen, class Target>
  constexpr auto genpoly_impl(
    meta_type<tm_closure<Ts...>> term,
    meta_type<Gen> gen,
    meta_type<Target> target)
  {
    if constexpr (empty(to_tuple(term))) {
      return make_pair(target, gen);
    } else {
      auto h  = head(term);
      auto t  = tail(term);
      auto p1 = genpoly_impl(h, gen, target);
      auto t1 = p1.first();
      auto g1 = p1.second();
      return genpoly_impl(t, g1, t1);
    }
  }

  template <class T1, class T2, class Gen, class Target>
  constexpr auto genpoly_impl(
    meta_type<tm_apply<T1, T2>> term,
    meta_type<Gen> gen,
    meta_type<Target> target)
  {
    auto p1 = genpoly_impl(term.t1(), gen, target);
    auto t1 = p1.first();
    auto g1 = p1.second();

    auto p2 = genpoly_impl(term.t2(), g1, t1);
    auto t2 = p2.first();
    auto g2 = p2.second();
    return make_pair(make_tm_apply(t1, t2), g2);
  }

  template <class Tag, class Gen, class Target>
  constexpr auto genpoly_impl(
    meta_type<tm_value<Tag>>,
    meta_type<Gen> gen,
    meta_type<Target> target)
  {
    return make_pair(target, gen);
  }

  template <class Tag, class Gen, class Target>
  constexpr auto genpoly_impl(
    meta_type<tm_varvalue<Tag>>,
    meta_type<Gen> gen,
    meta_type<Target> target)
  {
    return make_pair(target, gen);
  }

  template <class Tag, class Gen, class Target>
  constexpr auto genpoly_impl(
    meta_type<tm_var<Tag>> term,
    meta_type<Gen> gen,
    meta_type<Target> target)
  {
    return make_pair(subst_term(term, make_tm_var(gen), target), nextgen(gen));
  }

  /// Recursively traverse term tree and replace tm_var with new one.
  /// \param term a point on term tree currently being processed
  /// \param gen generator
  template <class Term, class Gen>
  [[nodiscard]] constexpr auto genpoly(meta_type<Term> term, meta_type<Gen> gen)
  {
    return genpoly_impl(term, gen, term);
  }

  // ------------------------------------------
  // type_of

  // fwd
  template <class Term, class Gen, bool Assert>
  constexpr auto type_of_impl(
    meta_type<Term> term,
    meta_type<Gen> gen,
    std::bool_constant<Assert> enable_assert);

  /// Convert `tm_closure<T1, T2...>` to `arrow<T1, arrow<T2, ...>>`.
  /// \param term term
  /// \param gen generator
  /// \param enable_assert an option to control static_assert
  template <class T, class Gen, bool Assert>
  constexpr auto type_of_impl_closure(
    meta_type<T> term,
    meta_type<Gen> gen,
    std::bool_constant<Assert> enable_assert)
  {
    if constexpr (is_tm_closure(term)) {

      auto h = head(term);
      auto t = tail(term);

      if constexpr (term.size() > 1) {

        // head
        auto p1 = type_of_impl_closure(h, gen, enable_assert);
        auto t1 = p1.first();
        auto g1 = p1.second();
        // tail
        auto p2 = type_of_impl_closure(t, g1, enable_assert);
        auto t2 = p2.first();
        auto g2 = p2.second();

        // No need for error check (as far as tm_closure is used just for
        // type declaration).
        return make_pair(make_ty_arrow(t1, t2), g2);

      } else
        return type_of_impl_closure(h, gen, enable_assert);

    } else
      return type_of_impl(term, gen, enable_assert);
  }

  /// Create fresh polymorphic type on tm_closure, otherwise fallback to
  /// type_of_impl.
  template <class Term, class Gen, bool Assert>
  constexpr auto type_of_impl_app(
    meta_type<Term> term,
    meta_type<Gen> gen,
    std::bool_constant<Assert> enable_assert)
  {
    if constexpr (is_tm_closure(term)) {
      // generate fresh polymorphoc type
      auto p1 = genpoly(term, gen);
      auto t1 = p1.first();
      auto g1 = p1.second();
      // create arrow type from tm_closure
      return type_of_impl_closure(t1, g1, enable_assert);
    } else {
      return type_of_impl(term, gen, enable_assert);
    }
  }

  /// Infer a type of term tree.
  /// \param term term
  /// \param gen generator
  /// \param enable_assert an option to control static_assert
  template <class Term, class Gen, bool Assert>
  constexpr auto type_of_impl(
    meta_type<Term> term,
    meta_type<Gen> gen,
    std::bool_constant<Assert> enable_assert)
  {
    (void)term;
    (void)gen;
    (void)enable_assert;
    static_assert(false_v<Term, Gen, Assert>, "Invalid term");
  }

  template <class T1, class T2, class Gen, bool Assert>
  constexpr auto type_of_impl(
    meta_type<tm_apply<T1, T2>> term,
    meta_type<Gen> gen,
    std::bool_constant<Assert> enable_assert)
  {
    // app
    auto p1 = type_of_impl_app(term.t1(), gen, enable_assert);
    auto t1 = p1.first();
    auto g1 = p1.second();
    if constexpr (is_tyerror(t1)) {
      return make_pair(t1, g1);
    } else {
      // arg
      auto p2 = type_of_impl(term.t2(), g1, enable_assert);
      auto t2 = p2.first();
      auto g2 = p2.second();
      if constexpr (is_tyerror(t2)) {
        return make_pair(t2, g2);
      } else {
        // type check subtree
        auto var = make_ty_var(g2);
        auto g3  = nextgen(g2);
        auto s   = unify(t1, make_ty_arrow(t2, var), enable_assert);
        if constexpr (is_tyerror(s))
          return make_pair(s, g3);
        else
          return make_pair(apply_subst(s, var), g3);
      }
    }
  }

  template <class Tag, class Gen, bool Assert>
  constexpr auto type_of_impl(
    meta_type<tm_value<Tag>> term,
    meta_type<Gen> gen,
    std::bool_constant<Assert>)
  {
    return make_pair(make_ty_value(term.tag()), gen);
  }

  template <class Tag, class Gen, bool Assert>
  constexpr auto type_of_impl(
    meta_type<tm_varvalue<Tag>> term,
    meta_type<Gen> gen,
    std::bool_constant<Assert>)
  {
    return make_pair(make_ty_varvalue(term.tag()), gen);
  }

  template <class Tag, class Gen, bool Assert>
  constexpr auto type_of_impl(
    meta_type<tm_var<Tag>> term,
    meta_type<Gen> gen,
    std::bool_constant<Assert>)
  {
    return make_pair(make_ty_var(term.tag()), gen);
  }

  template <class... Ts, class Gen, bool Assert>
  constexpr auto type_of_impl(
    meta_type<tm_closure<Ts...>> term,
    meta_type<Gen> gen,
    std::bool_constant<Assert> enable_assert)
  {
    return type_of_impl_closure(term, gen, enable_assert);
  }

  /// Infer type of term tree
  /// \param term term
  /// \param enable_assert option to control `static_assert`
  template <class Term, bool Assert = true>
  [[nodiscard]] constexpr auto type_of(
    meta_type<Term> term,
    std::bool_constant<Assert> enable_assert = {})
  {
    auto p = type_of_impl(term, gen_c<0>, enable_assert);
    auto t = p.first(); // type
    // return result type
    return t;
  }

  // ------------------------------------------
  // check_type_static

  /// check type at compile time
  template <class T, class U>
  void check_type_static(const object_ptr<U>&)
  {
    auto t1 = type_of(get_term<T>(), true_c);
    auto t2 = type_of(get_term<U>(), true_c);

    if constexpr (t1 != t2) {
      static_assert(false_v<T>, "Compile time type check failed!");
      using expected = typename decltype(t1)::_show;
      using provided = typename decltype(t2)::_show;
      static_assert(false_v<expected, provided>);
    }
  }

} // namespace yave