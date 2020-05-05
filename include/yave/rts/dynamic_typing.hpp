//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/type_gen.hpp>
#include <yave/rts/apply.hpp>
#include <yave/rts/lambda.hpp>
#include <yave/rts/exception.hpp>
#include <yave/rts/type_error.hpp>
#include <yave/rts/undefined.hpp>
#include <yave/rts/object_util.hpp>
#include <yave/rts/closure.hpp>
#include <yave/rts/list.hpp>

#include <vector>
#include <algorithm>
#include <map>
#include <optional>

namespace yave {

  // fwd
  template <class T, class U>
  [[nodiscard]] auto value_cast_if(const object_ptr<U>& obj) noexcept
    -> object_ptr<propagate_const_t<T, U>>;
  template <class T, class U>
  [[nodiscard]] auto value_cast_if(object_ptr<U>&& obj) noexcept
    -> object_ptr<propagate_const_t<T, U>>;

  // ------------------------------------------
  // Kind

  inline bool is_kstar(const object_ptr<const Kind>& k)
  {
    // nullptr as kstar
    return !static_cast<bool>(k);
  }

  inline bool is_kfun(const object_ptr<const Kind>& k)
  {
    return !is_kstar(k);
  }

  inline bool same_kind(
    const object_ptr<const Kind>& k1,
    const object_ptr<const Kind>& k2)
  {
    if (is_kstar(k1) && is_kstar(k2))
      return true;

    if (is_kfun(k1) && is_kfun(k2))
      return same_kind(k1->k1, k2->k1) && same_kind(k1->k2, k2->k2);

    return false;
  }

  inline auto kind_of(const object_ptr<const Type>& t) -> object_ptr<const Kind>
  {
    if (auto con = get_if<tcon_type>(t.value()))
      return con->kind;

    if (auto var = get_if<tvar_type>(t.value()))
      return var->kind;

    if (auto ap = get_if<tap_type>(t.value())) {
      auto k1 = kind_of(ap->t1);
      auto k2 = kind_of(ap->t2);

      if (!is_kfun(k1) || !same_kind(k1->k1, k2))
        throw type_error::kind_missmatch(k1, k2, nullptr);

      return k1->k2;
    }
    unreachable();
  }

  // ------------------------------------------
  // same_type

  namespace detail {

    inline bool same_tcon(const tcon_type& lhs, const tcon_type& rhs)
    {
      return tcon_id_eq(lhs.id, rhs.id) && same_kind(lhs.kind, rhs.kind);
    }

    inline bool same_tvar(const tvar_type& lhs, const tvar_type& rhs)
    {
      return tvar_id_eq(lhs.id, rhs.id) && same_kind(lhs.kind, rhs.kind);
    }

    inline bool same_type_impl(
      const object_ptr<const Type>& left,
      const object_ptr<const Type>& right)
    {
      if (left.get() == right.get())
        return true;

      auto* l = left.value();
      auto* r = right.value();

      if (_get_storage(*l).index != _get_storage(*r).index)
        return false;

      if (auto lcon = get_if<tcon_type>(l))
        if (auto rcon = get_if<tcon_type>(r))
          return same_tcon(*lcon, *rcon);

      if (auto lap = get_if<tap_type>(l))
        if (auto rap = get_if<tap_type>(r))
          return same_type_impl(lap->t1, rap->t1)
                 && same_type_impl(lap->t2, rap->t2);

      if (auto lvar = get_if<tvar_type>(l))
        if (auto rvar = get_if<tvar_type>(r))
          return same_tvar(*lvar, *rvar);

      unreachable();
    }
  } // namespace detail

  /// check type equality
  [[nodiscard]] inline bool same_type(
    const object_ptr<const Type>& lhs,
    const object_ptr<const Type>& rhs)
  {
    assert(lhs && rhs);
    return detail::same_type_impl(lhs, rhs);
  }

  // ------------------------------------------
  // Utils

  /// is_tcon_type
  [[nodiscard]] inline bool is_tcon_type(const object_ptr<const Type>& tp)
  {
    return get_if<tcon_type>(tp.value());
  }

  /// is_tcon_type_if
  [[nodiscard]] inline auto is_tcon_type_if(const object_ptr<const Type>& tp)
  {
    return get_if<tcon_type>(tp.value());
  }

  /// is_tap_type
  [[nodiscard]] inline bool is_tap_type(const object_ptr<const Type>& tp)
  {
    return get_if<tap_type>(tp.value());
  }

  /// is_tap_type_if
  [[nodiscard]] inline auto is_tap_type_if(const object_ptr<const Type>& tp)
  {
    return get_if<tap_type>(tp.value());
  }

  /// is_tvar_type
  [[nodiscard]] inline bool is_tvar_type(const object_ptr<const Type>& tp)
  {
    return get_if<tvar_type>(tp.value());
  }

  /// is_tvar_type_if
  [[nodiscard]] inline auto is_tvar_type_if(const object_ptr<const Type>& tp)
  {
    return get_if<tvar_type>(tp.value());
  }

  /// has_tcon_type
  [[nodiscard]] inline bool has_tcon_type(const object_ptr<const Object>& obj)
  {
    return is_tcon_type(get_type(obj));
  }

  /// has_tap_type
  [[nodiscard]] inline bool has_tap_type(const object_ptr<const Object>& obj)
  {
    return is_tap_type(get_type(obj));
  }

  /// has_tvar_type
  [[nodiscard]] inline bool has_tvar_type(const object_ptr<const Object>& obj)
  {
    return is_tvar_type(get_type(obj));
  }

  // ------------------------------------------
  // arrow type

  [[nodiscard]] inline auto make_arrow_type(
    const object_ptr<const Type>& t1,
    const object_ptr<const Type>& t2)
  {
    return make_object<Type>(
      tap_type {make_object<Type>(tap_type {arrow_type_tcon(), t1}), t2});
  }

  [[nodiscard]] inline auto is_arrow_type(const object_ptr<const Type>& t)
  {
    if (auto tap1 = is_tap_type_if(t))
      if (auto tap2 = is_tap_type_if(tap1->t1))
        if (same_type(tap2->t1, arrow_type_tcon()))
          return true;
    return false;
  }

  [[nodiscard]] inline auto has_arrow_type(const object_ptr<const Object>& obj)
  {
    return is_arrow_type(get_type(obj));
  }

  // ------------------------------------------
  // list type

  [[nodiscard]] inline auto make_list_type(const object_ptr<const Type>& t)
  {
    return make_object<Type>(tap_type {list_type_tcon(), t});
  }

  [[nodiscard]] inline auto is_list_type(const object_ptr<const Type>& t)
  {
    if (auto tap = is_tap_type_if(t))
      if (same_type(tap->t1, list_type_tcon()))
        return true;

    return false;
  }

  [[nodiscard]] inline auto has_list_type(const object_ptr<const Object>& obj)
  {
    return is_list_type(get_type(obj));
  }

  // ------------------------------------------
  // var type

  [[nodiscard]] inline auto make_var_type(uint64_t id)
  {
    return make_object<Type>(tvar_type {id /* kstar */});
  }

  // ------------------------------------------
  // genvar

  /// generate new type variable
  [[nodiscard]] inline auto genvar() -> object_ptr<const Type>
  {
    return make_object<Type>(tvar_type::random_generate());
  }

  // ------------------------------------------
  // copy_type

  namespace detail {

    inline auto copy_type_impl(const object_ptr<const Type>& t)
      -> object_ptr<const Type>
    {
      if (auto con = is_tcon_type_if(t))
        return make_object<Type>(tcon_type {*con});

      if (auto var = is_tvar_type_if(t))
        return make_object<Type>(tvar_type {*var});

      if (auto ap = is_tap_type_if(t))
        return make_object<Type>(
          tap_type {copy_type_impl(ap->t1), copy_type_impl(ap->t2)});

      unreachable();
    }
  } // namespace detail

  /// Deep copy type object
  [[nodiscard]] inline auto copy_type(const object_ptr<const Type>& tp)
    -> object_ptr<const Type>
  {
    assert(tp);
    return detail::copy_type_impl(tp);
  }

  // ------------------------------------------
  // type structs

  /// type substitution
  struct type_arrow
  {
    object_ptr<const Type> from;
    object_ptr<const Type> to;
  };

  // ------------------------------------------
  // apply_type_arrow

  namespace detail {

    // returns nullptr for identical subtree
    inline auto apply_type_arrow_impl_rec(
      const type_arrow& ta,
      const object_ptr<const Type>& in) -> object_ptr<const Type>
    {
      if (auto tap = is_tap_type_if(in)) {
        auto t1 = apply_type_arrow_impl_rec(ta, tap->t1);
        auto t2 = apply_type_arrow_impl_rec(ta, tap->t2);
        return (!t1 && !t2) ? nullptr
                            : make_object<Type>(
                              tap_type {t1 ? t1 : tap->t1, t2 ? t2 : tap->t2});
      }

      if (same_type(in, ta.from))
        return ta.to;

      return nullptr;
    }

    inline auto apply_type_arrow_impl(
      const type_arrow& ta,
      const object_ptr<const Type>& in) -> object_ptr<const Type>
    {
      auto r = apply_type_arrow_impl_rec(ta, in);
      return r ? r : in;
    }

  } // namespace detail

  /// type substitution
  [[nodiscard]] inline auto apply_type_arrow(
    const type_arrow& ta,
    const object_ptr<const Type>& in) -> object_ptr<const Type>
  {
    return detail::apply_type_arrow_impl(ta, in);
  }

  // ------------------------------------------
  // type arrow map

  struct var_type_comp
  {
    bool operator()(
      const object_ptr<const Type>& l,
      const object_ptr<const Type>& r) const noexcept
    {
      return (is_tvar_type_if(l)->id < is_tvar_type_if(r)->id);
    }
  };

  /// subst
  class type_arrow_map
  {
  public:
    type_arrow_map()                 = default;
    type_arrow_map(type_arrow_map&&) = default;
    type_arrow_map& operator=(type_arrow_map&&) = default;

    // insert new subst
    void insert(const type_arrow& ta)
    {
      assert(is_tvar_type(ta.from));
      auto p = m_map.try_emplace(ta.from, ta.to);

      if (!p.second)
        std::logic_error("duplicated insertion");
    }

    // find subst
    [[nodiscard]] auto find(const object_ptr<const Type>& from) const
      -> std::optional<type_arrow>
    {
      auto it = m_map.find(from);

      if (it == m_map.end())
        return std::nullopt;

      return type_arrow {it->first, it->second};
    }

    // erase subst
    void erase(const object_ptr<const Type>& from)
    {
      m_map.erase(from);
    }

    // loop
    template <class F>
    void for_each(F&& func) const
    {
      for (auto&& p : m_map) {
        std::forward<F>(func)(p.first, p.second);
      }
    }

    // loop
    template <class F>
    void for_each(F&& func)
    {
      for (auto&& p : m_map) {
        std::forward<F>(func)(p.first, p.second);
      }
    }

    [[nodiscard]] auto size() const -> size_t
    {
      return m_map.size();
    }

    [[nodiscard]] bool empty() const
    {
      return m_map.empty();
    }

  private:
    /// map of (from, to)
    std::map<object_ptr<const Type>, object_ptr<const Type>, var_type_comp>
      m_map;
  };

  // ------------------------------------------
  // apply_subst

  /// apply substitution
  [[nodiscard]] inline auto apply_subst(
    const type_arrow_map& s,
    const object_ptr<const Type>& ty) -> object_ptr<const Type>
  {
    auto tmp = ty;
    s.for_each([&](auto& from, auto& to) {
      tmp = apply_type_arrow({from, to}, tmp);
    });
    return tmp;
  }

  /// apply substitution
  [[nodiscard]] inline auto apply_subst(
    const type_arrow_map& s,
    const type_arrow& a) -> type_arrow
  {
    return {apply_subst(s, a.from), apply_subst(s, a.to)};
  }

  // ------------------------------------------
  // vars

  namespace detail {

    inline void vars_impl(
      const object_ptr<const Type>& tp,
      std::vector<object_ptr<const Type>>& vars)
    {
      if (is_tvar_type(tp))
        vars.push_back(tp);

      if (auto tap = is_tap_type_if(tp)) {
        vars_impl(tap->t1, vars);
        vars_impl(tap->t2, vars);
      }
    }
  } // namespace detail

  /// get list of type variables
  [[nodiscard]] inline auto vars(const object_ptr<const Type>& tp)
    -> std::vector<object_ptr<const Type>>
  {
    auto vars = std::vector<object_ptr<const Type>> {};
    detail::vars_impl(tp, vars);
    return vars;
  }

  /// get list of free variables
  [[nodiscard]] inline auto vars(const type_arrow_map& s)
    -> std::vector<object_ptr<const Type>>
  {
    std::vector<object_ptr<const Type>> ret;
    s.for_each([&](auto&, auto& to) {
      auto v = vars(to);
      ret.insert(ret.end(), v.begin(), v.end());
    });
    return ret;
  }

  // ------------------------------------------
  // compose_subst

  /// compose substitution
  /// \note: s (s1 T), not s1 (s T)!
  inline void compose_subst_over(type_arrow_map& s1, const type_arrow& s)
  {
    s1.for_each([&](auto&, auto& to) { to = apply_type_arrow(s, to); });

    if (!s1.find(s.from))
      s1.insert(s);
  }

  /// compose substitution
  /// \note: s2 (s1 T), not s1 (s2 T)!
  inline void compose_subst_over(type_arrow_map& s1, const type_arrow_map& s2)
  {
    s1.for_each([&](auto&, auto& to) { to = apply_subst(s2, to); });

    s2.for_each([&](auto& from, auto& to) {
      if (!s1.find(from))
        s1.insert({from, to});
    });
  }

  // ------------------------------------------
  // merge_subst

  inline void merge_subst_over(type_arrow_map& s1, const type_arrow_map& s2)
  {
    std::vector<object_ptr<const Type>> ts1;
    s1.for_each([&](auto& from, auto&) { ts1.push_back(from); });

    std::vector<object_ptr<const Type>> ts2;
    s2.for_each([&](auto& from, auto&) { ts2.push_back(from); });

    assert(std::is_sorted(ts1.begin(), ts1.end(), var_type_comp()));
    assert(std::is_sorted(ts2.begin(), ts2.end(), var_type_comp()));

    std::vector<object_ptr<const Type>> isect;

    std::set_intersection(
      ts1.begin(),
      ts1.end(),
      ts2.begin(),
      ts2.end(),
      std::inserter(isect, isect.end()),
      var_type_comp());

    for (auto&& t : isect)
      if (!same_type(apply_subst(s1, t), apply_subst(s2, t)))
        throw type_error::unsolvable_constraints(
          apply_subst(s1, t), apply_subst(s2, t), nullptr);

    s2.for_each([&](auto& from, auto& to) { s1.insert({from, to}); });
  }

  // ------------------------------------------
  // occurs

  /// occurs
  [[nodiscard]] inline bool occurs(
    const object_ptr<const Type>& u,
    const object_ptr<const Type>& t)
  {
    assert(is_tvar_type(u));

    if (is_tcon_type(t))
      return false;

    if (is_tvar_type(t))
      return same_type(u, t);

    if (auto ap = is_tap_type_if(t))
      return occurs(u, ap->t1) || occurs(u, ap->t2);

    unreachable();
  }

  // ------------------------------------------
  // mgu

  [[nodiscard]] inline auto mgu_var(
    const object_ptr<const Type>& u,
    const object_ptr<const Type>& t) -> type_arrow_map
  {
    assert(is_tvar_type(u));

    if (same_type(u, t))
      return {};

    if (occurs(u, t))
      throw type_error::circular_constraint(u, nullptr);

    if (!same_kind(kind_of(u), kind_of(t)))
      throw type_error::kind_missmatch(kind_of(u), kind_of(t), nullptr);

    type_arrow_map ret;
    ret.insert({u, t});
    return ret;
  }

  /// most general uinfier
  [[nodiscard]] inline auto mgu(
    const object_ptr<const Type>& t1,
    const object_ptr<const Type>& t2) -> type_arrow_map
  {
    if (auto tap1 = is_tap_type_if(t1)) {
      if (auto tap2 = is_tap_type_if(t2)) {
        auto s1 = mgu(tap1->t1, tap2->t1);
        auto s2 = mgu(apply_subst(s1, tap1->t2), apply_subst(s1, tap2->t2));
        compose_subst_over(s1, s2);
        return s1;
      }
    }

    if (is_tvar_type(t1))
      return mgu_var(t1, t2);

    if (is_tvar_type(t2))
      return mgu_var(t2, t1);

    if (auto tcon1 = is_tcon_type_if(t1)) {
      if (auto tcon2 = is_tcon_type_if(t2)) {
        if (same_type(t1, t2))
          return {};
        else
          throw type_error::type_missmatch(t1, t2, nullptr);
      }
    }
    throw type_error::unsolvable_constraints(t1, t2, nullptr);
  }

  // ------------------------------------------
  // match

  [[nodiscard]] inline auto match(
    const object_ptr<const Type>& t1,
    const object_ptr<const Type>& t2) -> type_arrow_map
  {
    if (auto tap1 = is_tap_type_if(t1)) {
      if (auto tap2 = is_tap_type_if(t2)) {
        auto s1 = match(tap1->t1, tap2->t1);
        auto s2 = match(tap1->t2, tap2->t2);
        merge_subst_over(s1, s2);
        return s1;
      }
    }

    if (is_tvar_type(t1))
      return mgu_var(t1, t2);

    if (is_tcon_type(t1) && is_tcon_type(t2)) {
      if (same_type(t1, t2))
        return {};
      else
        throw type_error::type_missmatch(t1, t2, nullptr);
    }

    throw type_error::unsolvable_constraints(t1, t2, nullptr);
  }

  // ------------------------------------------
  // unify

  /// unify
  /// \param constr Type constraint
  /// \param src Source node (for error handling)
  [[nodiscard]] inline auto unify(
    const object_ptr<const Type>& t1,
    const object_ptr<const Type>& t2) -> type_arrow_map
  {
    return mgu(t1, t2);
  }

  // ------------------------------------------
  // generalize

  namespace detail {

    struct generalize_table
    {
      object_ptr<const Type> t1;
      object_ptr<const Type> t2;
      object_ptr<const Type> g;
    };

    inline auto generalize_impl_rec(
      const object_ptr<const Type>& t1,
      const object_ptr<const Type>& t2,
      std::vector<generalize_table>& table) -> object_ptr<const Type>
    {
      // (T,T) -> T
      if (same_type(t1, t2))
        return t1;

      // try to get least general form
      if (auto tap1 = is_tap_type_if(t1)) {
        if (auto tap2 = is_tap_type_if(t2)) {
          auto r1 = generalize_impl_rec(tap1->t1, tap2->t1, table);
          auto r2 = generalize_impl_rec(tap1->t2, tap2->t2, table);
          if (r1 && r2)
            return make_object<Type>(tap_type {r1, r2});
        }
      }

      // restrict to kstar
      if (!is_kstar(kind_of(t1)) || !is_kstar(kind_of(t2)))
        return nullptr;

      // (S,T) -> X
      for (auto&& e : table) {
        if (same_type(e.t1, t1) && same_type(e.t2, t2))
          return e.g;
      }

      auto gv = genvar();
      table.push_back({t1, t2, gv});
      return gv;
    }

    inline auto generalize_impl(const std::vector<object_ptr<const Type>>& ts)
      -> object_ptr<const Type>
    {
      object_ptr<const Type> tp = ts.front();

      std::vector<generalize_table> table;
      std::for_each(ts.begin() + 1, ts.end(), [&](auto&& t) {
        tp = generalize_impl_rec(tp, t, table);
        table.clear();
      });

      return tp;
    }

  } // namespace detail

  /// Anti-unification.
  /// This function gives least general generalization of given types.
  /// All type variables which can be generated by this function are limited to
  /// kind kstar(*).
  /// FIXME: I'm not sure this algorithm is correct, maybe it's wrong in some
  /// cases?
  /// \param ts Not-empty list of types
  [[nodiscard]] inline auto generalize(
    const std::vector<object_ptr<const Type>>& ts) -> object_ptr<const Type>
  {
    assert(!ts.empty());

    // should not be type constructor
    for (auto&& t : ts)
      assert(is_kstar(kind_of(t)));

    return detail::generalize_impl(ts);
  }

  // ------------------------------------------
  // specializable

  /// specializable
  [[nodiscard]] inline auto specializable(
    const object_ptr<const Type>& t1,
    const object_ptr<const Type>& t2) -> std::optional<type_arrow_map>
  {
    try {
      return match(t1, t2);
    } catch (const type_error::type_error&) {
      return std::nullopt;
    }
  }

  // ------------------------------------------
  // genpoly

  /// create fresh instance of polymorphic type
  [[nodiscard]] inline auto genpoly(
    const object_ptr<const Type>& tp,
    const type_arrow_map& env) -> object_ptr<const Type>
  {
    auto vs = vars(tp);
    auto t  = tp;

    for (auto v : vs) {

      if (env.find(v))
        continue;

      auto a = type_arrow {v, genvar()};
      t      = apply_type_arrow(a, t);
    }
    return t;
  }

  // ------------------------------------------
  // type_of

  namespace detail {

    inline auto type_of_impl(
      const object_ptr<const Object>& obj,
      type_arrow_map& env) -> object_ptr<const Type>
    {
      // Apply
      if (auto apply = value_cast_if<Apply>(obj)) {

        auto& storage = _get_storage(*apply);

        // cached
        if (storage.is_result())
          return type_of_impl(storage.get_result(), env);

        auto t1 = type_of_impl(storage.app(), env);
        auto t2 = type_of_impl(storage.arg(), env);

        try {

          auto var = genvar();
          auto as  = unify(apply_subst(env, t1), make_arrow_type(t2, var));
          auto ty  = apply_subst(as, var);

          as.erase(var);
          compose_subst_over(env, as);

          return ty;

        } catch (type_error::type_error& e) {
          if (!e.has_source())
            e.source() = obj;
          throw;
        }
      }

      // Lambda
      if (auto lambda = value_cast_if<Lambda>(obj)) {

        auto& storage = _get_storage(*lambda);

        auto var = make_var_type(storage.var->id());
        env.insert(type_arrow {var, var});

        auto t1 = type_of_impl(storage.var, env);
        auto t2 = type_of_impl(storage.body, env);

        auto ty = make_arrow_type(apply_subst(env, t1), t2);

        env.erase(t1);

        return ty;
      }

      // Variable
      if (auto variable = value_cast_if<Variable>(obj)) {

        auto var = make_var_type(variable->id());
        if (auto s = env.find(var))
          return s->to;

        throw type_error::unbounded_variable(var, obj);
      }

      // Partially applied closures
      if (has_arrow_type(obj))
        if (auto c = (const Closure<>*)obj.get(); c->is_pap())
          return type_of_impl(c->vertebrae(c->arity), env);

      // tap
      if (has_tap_type(obj))
        return genpoly(get_type(obj), env);

      // tcon
      if (has_tcon_type(obj))
        return get_type(obj);

      // tvar
      if (has_tvar_type(obj))
        return get_type(obj);

      unreachable();
    }
  } // namespace detail

  /// dynamic type checker.
  [[nodiscard]] inline auto type_of(const object_ptr<const Object>& obj)
    -> object_ptr<const Type>
  {
    type_arrow_map env; // type environment

    auto ty = detail::type_of_impl(obj, env);

    // FIXME: Is subst_type_all(env, ty) necessary?
    return ty;
  }

  // ------------------------------------------
  // has_type

  namespace detail {

    template <class T, class U>
    bool has_type_impl(const object_ptr<U>& obj)
    {
      // general
      if constexpr (is_tm_value(get_term<T>())) {
        // optimize type check on certain types
        if constexpr (detail::has_info_table_tag<T>())
          return likely(obj)
                 && _get_storage(obj).template match_info_table_tag<T>();

        // normal type check
        return same_type(get_type(obj), object_type<T>());
      }
      // List
      else if constexpr (has_tm_list<T>()) {

        if (unlikely(!obj) || !has_list_type(obj))
          return false;

        auto& storage =
          _get_storage(*static_object_cast<const List<Object>>(obj));

        // [] can be any type
        if (storage.is_nil())
          return true;

        using elem_tp = typename decltype(get_term<T>().t().tag())::type;

        return has_type_impl<elem_tp>(storage.car);

      } else
        static_assert(false_v<T>, "T is not value type");
    }

  } // namespace detail

  /// has_type
  template <class T, class U>
  [[nodiscard]] bool has_type(const object_ptr<U>& obj)
  {
    return detail::has_type_impl<T>(obj);
  }

  // ------------------------------------------
  // is_type

  template <class T>
  [[nodiscard]] bool is_type(const object_ptr<const Type>& type)
  {
    return same_type(type, object_type<T>());
  }

  // ------------------------------------------
  // check_type_dynamic

  /// check type at runtime
  template <class T, class U>
  void check_type_dynamic(const object_ptr<U>& obj)
  {
    auto t1 = object_type<T>();
    auto t2 = type_of(obj);
    if (unlikely(!same_type(t1, t2)))
      throw type_error::bad_type_check(t1, t2, obj);
  }
} // namespace yave