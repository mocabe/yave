//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/meta_type.hpp>
#include <yave/rts/meta_tuple.hpp>
#include <yave/rts/meta_set.hpp>
#include <yave/rts/meta_pair.hpp>
#include <yave/rts/terms.hpp>
#include <yave/rts/types.hpp>

namespace yave {

  // ------------------------------------------
  // subst

  /// Type substitution
  /// \param ar type map of substitution
  /// \param type type to apply substitution
  template <class TyT1Tag, class TyT2, class T>
  [[nodiscard]] constexpr auto subst(
    meta_type<tyarrow<ty_var<TyT1Tag>, TyT2>> ar,
    meta_type<T> type)
  {
    (void)ar;
    (void)type;

    if constexpr (type == ar.t1()) {
      return ar.t2();
    } else if constexpr (is_ty_arrow(type)) {
      auto lt = subst(ar, type.t1());
      auto rt = subst(ar, type.t2());
      return make_ty_arrow(lt, rt);
    } else if constexpr (is_ty_value(type)) {
      return type;
    } else if constexpr (is_ty_var(type)) {
      return type;
    } else if constexpr (is_ty_varvalue(type)) {
      return type;
    } else if constexpr (is_ty_list(type)) {
      auto t = subst(ar, type.t());
      return make_ty_list(t);
    } else
      static_assert(false_v<T>, "Invalid type");
  }

  // ------------------------------------------
  // subst_all

  /// Process list of type substitution
  template <class... TyArrow, class Ty>
  [[nodiscard]] constexpr auto subst_all(
    meta_tuple<TyArrow...> tyarrows,
    meta_type<Ty> type)
  {
    if constexpr (empty(tyarrows))
      return type;
    else
      return subst_all(tail(tyarrows), subst(head(tyarrows), type));
  }

  // ------------------------------------------
  // Constr

  /// Type constraint
  template <class T1, class T2>
  struct constr
  {
    using type = constr<T1, T2>;
    using t1   = T1;
    using t2   = T2;
  };

  template <class T1, class T2>
  struct meta_type<constr<T1, T2>>
  {
    [[nodiscard]] constexpr auto t1() const
    {
      return type_c<T1>;
    }
    [[nodiscard]] constexpr auto t2() const
    {
      return type_c<T2>;
    }
  };

  template <class T1, class T2>
  [[nodiscard]] constexpr auto make_constr(meta_type<T1>, meta_type<T2>)
  {
    return type_c<constr<T1, T2>>;
  }

  // ------------------------------------------
  // subst_constr

  /// Type substitution on type constraint
  template <class TyT1, class TyT2, class T1, class T2>
  [[nodiscard]] constexpr auto subst_constr(
    meta_type<tyarrow<TyT1, TyT2>> a,
    meta_type<constr<T1, T2>>)
  {
    auto t1 = subst(a, type_c<T1>);
    auto t2 = subst(a, type_c<T2>);
    return make_constr(t1, t2);
  }

  // ------------------------------------------
  // subst_constr_all

  /// Process list of type substitution on a type constraint
  template <class TyT1, class TyT2, class... Cs>
  [[nodiscard]] constexpr auto subst_constr_all(
    meta_type<tyarrow<TyT1, TyT2>> a,
    meta_tuple<Cs...>)
  {
    (void)a;

    return make_tuple(subst_constr(a, type_c<Cs>)...);
  }

  // ------------------------------------------
  // compose_subst

  template <class... TyArrows, class TyT1, class TyT2, class... Results>
  constexpr auto compose_subst_impl(
    meta_tuple<TyArrows...> tyarrows,
    meta_type<tyarrow<TyT1, TyT2>> a,
    meta_tuple<Results...> result)
  {
    auto h = head(tyarrows);
    auto t = tail(tyarrows);

    auto r = make_tyarrow(h.t1(), subst(a, h.t2()));

    if constexpr (empty(t))
      return append(r, result);
    else
      return compose_subst_impl(t, a, append(r, result));
  }

  /// compose substitution
  template <class... TyArrows, class TyT1, class TyT2>
  [[nodiscard]] constexpr auto compose_subst(
    meta_tuple<TyArrows...> tyarrows,
    meta_type<tyarrow<TyT1, TyT2>> a)
  {
    // g(f(S)):
    // | X->g(T) when (X->T) belongs f
    // | X->T    when (X->T) belongs g && X not belongs dom(f)
    if constexpr (empty(tyarrows))
      return make_tuple(a);
    else {
      auto r = compose_subst_impl(tyarrows, a, tuple_c<>);
      // FIXME: add domain check
      return append(a, r);
    }
  }

  // ------------------------------------------
  // occurs

  /// "Occurs check" algorithm
  template <class X, class T>
  [[nodiscard]] constexpr auto occurs(meta_type<X> x, meta_type<T> t)
  {
    (void)x;
    (void)t;

    if constexpr (is_ty_arrow(t)) {
      constexpr bool b = occurs(x, t.t1()) || occurs(x, t.t2());
      return std::bool_constant<b> {};
    } else if constexpr (is_ty_value(t)) {
      return false_c;
    } else if constexpr (is_ty_var(t)) {
      return x == t;
    } else if constexpr (is_ty_varvalue(t)) {
      return false_c;
    } else if constexpr (is_ty_list(t)) {
      return occurs(x, t.t());
    } else
      static_assert(false_v<T>, "Invalid type");
  }

  // ------------------------------------------
  // unify

  template <class... Cs>
  constexpr auto unify_impl(meta_tuple<Cs...> cs);

  template <class Var, class X, class... Cs>
  constexpr auto unify_impl_vars(
    meta_type<Var> var,
    meta_type<X> x,
    meta_tuple<Cs...> cs)
  {
    // occurs check
    if constexpr (occurs(var, x)) {
      return make_circular_constraints(var, cs);
    } else {
      auto arr = make_tyarrow(var, x);
      auto t   = unify_impl(subst_constr_all(arr, tail(cs)));
      // error handling
      if constexpr (is_tyerror(t))
        return t;
      else
        return compose_subst(t, arr);
    }
  }

  template <class... Cs>
  constexpr auto unify_impl(meta_tuple<Cs...> cs)
  {
    if constexpr (empty(cs)) {
      return tuple_c<>;
    } else {
      auto c  = head(cs);
      auto tl = tail(cs);

      (void)tl;

      if constexpr (c.t1() == c.t2()) {
        return unify_impl(tl);
      }
      // constr(arrow(S1, S2), arrow(T1, T2))
      else if constexpr (is_ty_arrow(c.t1()) && is_ty_arrow(c.t2())) {
        auto a1 = c.t1();
        auto a2 = c.t2();
        return unify_impl(concat(
          make_tuple(
            make_constr(a1.t1(), a2.t1()), make_constr(a1.t2(), a2.t2())),
          tl));
      }
      // constr(x , var)
      else if constexpr (is_ty_var(c.t2())) {
        auto x   = c.t1();
        auto var = c.t2();
        return unify_impl_vars(var, x, cs);
      }
      // constr(var, x)
      else if constexpr (is_ty_var(c.t1())) {
        auto var = c.t1();
        auto x   = c.t2();
        return unify_impl_vars(var, x, cs);
      }
      // constr(list(T1), list(T2))
      else if constexpr (is_ty_list(c.t1()) && is_ty_list(c.t2())) {
        auto l = c.t1();
        auto r = c.t2();
        return unify_impl(concat(make_tuple(make_constr(l.t(), r.t())), tl));
      } else
        return make_type_missmatch(c.t1(), c.t2(), tl);
    }
  }

  /// emit static_assert for static type check errors
  template <class T>
  [[nodiscard]] constexpr auto unify_assert(T type)
  {
    constexpr auto tag = type.tag();
    if constexpr (is_type_missmatch(tag)) {
      static_assert(false_v<T>, "Unification error: Type missmatch");
      // print error info
      using left  = typename decltype(tag)::type::left::_print_lhs;
      using right = typename decltype(tag)::type::right::_print_rhs;
      using other = typename decltype(tag)::type::other::_print_other;
      static_assert(false_v<left, right, other>, "Unification failed.");
    } else if constexpr (is_unsolvable_constraints(tag)) {
      static_assert(false_v<T>, "Unification error: Unsolvable constraints");
      // print error info
      using left  = typename decltype(tag)::type::left::_print_lhs;
      using right = typename decltype(tag)::type::right::_print_rhs;
      using other = typename decltype(tag)::type::other::_print_other;
      static_assert(false_v<left, right, other>, "Unification failed.");
    } else if constexpr (is_circular_constraints(tag)) {
      static_assert(false_v<T>, "Unification error: Circular constraints");
      // print error info
      using var   = typename decltype(tag)::type::var::_print_var;
      using other = typename decltype(tag)::type::other::_print_other;
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

  /// Compile time unification.
  /// \param cs a list of constraints
  /// \param enable_assert if `true_type` then emit `static_assert` otherwise
  /// return `tyerror`.
  /// \return `meta_set` of `constr` or `tyerror` on failure(when
  /// `enable_assert` is `false_type`).
  template <bool B, class... Cs>
  [[nodiscard]] constexpr auto unify(
    meta_tuple<Cs...> cs,
    std::bool_constant<B> enable_assert)
  {
    (void)cs;
    (void)enable_assert;

    constexpr auto t = unify_impl(cs);

    if constexpr (enable_assert && is_tyerror(t))
      return unify_assert(t);
    else
      return t;
  }

  // ------------------------------------------
  // subst_term

  template <class From, class To, class... Ts>
  constexpr auto subst_term_impl(
    meta_type<From> from,
    meta_type<To> to,
    meta_type<tm_closure<Ts...>>)
  {
    return make_tm_closure(subst_term(from, to, type_c<Ts>)...);
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
    (void)from;
    (void)to;
    (void)term;

    if constexpr (from == term) {
      return to;
    } else if constexpr (is_tm_closure(term)) {
      return subst_term_impl(from, to, term);
    } else if constexpr (is_tm_apply(term)) {
      return make_tm_apply(
        subst_term(from, to, term.t1()), subst_term(from, to, term.t2()));
    } else if constexpr (is_tm_value(term)) {
      return term;
    } else if constexpr (is_tm_varvalue(term)) {
      return term;
    } else if constexpr (is_tm_var(term)) {
      return term;
    } else if constexpr (is_tm_list(term)) {
      return make_tm_list(subst_term(from, to, term.t()));
    } else
      static_assert(false_v<Term>, "Invalid type");
  }

  // ------------------------------------------
  // genpoly

  /// Recursively traverse term tree and replace tm_var with new one.
  /// \param term a point on term tree currently being processed
  /// \param gen generator
  /// \param target current result of procedure
  template <class Term, class Gen, class Target>
  constexpr auto genpoly_impl(
    meta_type<Term> term,
    meta_type<Gen> gen,
    meta_type<Target> target)
  {
    if constexpr (is_tm_closure(term)) {
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
    } else if constexpr (is_tm_apply(term)) {
      auto p1 = genpoly_impl(term.t1(), gen, target);
      auto t1 = p1.first();
      auto g1 = p1.second();

      auto p2 = genpoly_impl(term.t2(), g1, t1);
      auto t2 = p2.first();
      auto g2 = p2.second();
      return make_pair(make_tm_apply(t1, t2), g2);
    } else if constexpr (is_tm_var(term)) {
      return make_pair(subst_term(term, gen_tm_var(gen), target), nextgen(gen));
    } else
      return make_pair(target, gen);
  }

  /// create fresh polymorphoc closure
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
      if constexpr (term.size() <= 1) {
        return type_of_impl_closure(head(term), gen, enable_assert);
      } else {
        auto h = head(term);
        auto t = tail(term);

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
      }
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

    if constexpr (is_tm_apply(term)) {
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
          auto var = gen_ty_var(g2);
          auto g3  = nextgen(g2);
          auto c   = make_tuple(make_constr(t1, make_ty_arrow(t2, var)));
          auto s   = unify(c, enable_assert);
          if constexpr (is_tyerror(s))
            return make_pair(s, g3);
          else
            return make_pair(subst_all(s, var), g3);
        }
      }
    } else if constexpr (is_tm_closure(term)) {
      return type_of_impl_closure(term, gen, enable_assert);
    } else if constexpr (is_tm_value(term)) {
      return make_pair(make_ty_value(term.tag()), gen);
    } else if constexpr (is_tm_varvalue(term)) {
      return make_pair(make_ty_varvalue(term.tag()), gen);
    } else if constexpr (is_tm_var(term)) {
      return make_pair(make_ty_var(term.tag()), gen);
    } else if constexpr (is_tm_list(term)) {
      auto p = type_of_impl(term.t(), gen, enable_assert);
      auto t = p.first();
      auto g = p.second();
      return make_pair(make_ty_list(t), g);
    } else
      static_assert(false_v<Term>, "Invalid term");
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

} // namespace yave