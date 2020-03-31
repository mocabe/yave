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
  // Utils

  /// type substitution
  struct type_arrow
  {
    object_ptr<const Type> from;
    object_ptr<const Type> to;
  };

  /// type constraint
  struct type_constr
  {
    object_ptr<const Type> t1;
    object_ptr<const Type> t2;
  };

  /// is_value_type
  [[nodiscard]] inline bool is_value_type(const object_ptr<const Type>& tp)
  {
    return get_if<value_type>(tp.value());
  }

  /// is_arrow_type
  [[nodiscard]] inline bool is_arrow_type(const object_ptr<const Type>& tp)
  {
    return get_if<arrow_type>(tp.value());
  }

  /// is_var_type
  [[nodiscard]] inline bool is_var_type(const object_ptr<const Type>& tp)
  {
    return get_if<var_type>(tp.value());
  }

  // is_list_type
  [[nodiscard]] inline bool is_list_type(const object_ptr<const Type>& tp)
  {
    return get_if<list_type>(tp.value());
  }

  /// has_value_type
  [[nodiscard]] inline bool has_value_type(const object_ptr<const Object>& obj)
  {
    return is_value_type(get_type(obj));
  }

  /// has_arrow_type
  [[nodiscard]] inline bool has_arrow_type(const object_ptr<const Object>& obj)
  {
    return is_arrow_type(get_type(obj));
  }

  /// has_var_type
  [[nodiscard]] inline bool has_var_type(const object_ptr<const Object>& obj)
  {
    return is_var_type(get_type(obj));
  }

  /// has_list_type
  [[nodiscard]] inline bool has_list_type(const object_ptr<const Object>& obj)
  {
    return is_list_type(get_type(obj));
  }

  // ------------------------------------------
  // genvar

  /// generate new type variable
  [[nodiscard]] inline auto genvar() -> object_ptr<const Type>
  {
    return make_object<Type>(var_type::random_generate());
  }

  // ------------------------------------------
  // flatten

  namespace detail {

    inline void flatten_impl(
      const object_ptr<const Type>& t,
      std::vector<object_ptr<const Type>>& v)
    {
      if (auto arr = get_if<arrow_type>(&*t)) {
        v.push_back(arr->captured);
        flatten_impl(arr->returns, v);
      } else {
        v.push_back(t);
      }
    }
  } // namespace detail

  /// flatten arrow type
  [[nodiscard]] inline auto flatten(const object_ptr<const Type>& tp)
    -> std::vector<object_ptr<const Type>>
  {
    assert(tp);
    std::vector<object_ptr<const Type>> ret;
    detail::flatten_impl(tp, ret);
    return ret;
  }

  // ------------------------------------------
  // copy_type

  namespace detail {

    inline auto copy_type_impl(const object_ptr<const Type>& t)
      -> object_ptr<const Type>
    {
      if (auto value = get_if<value_type>(t.value()))
        return make_object<Type>(value_type {*value});

      if (auto var = get_if<var_type>(t.value()))
        return make_object<Type>(var_type {var->id});

      if (auto arrow = get_if<arrow_type>(t.value()))
        return make_object<Type>(arrow_type {copy_type_impl(arrow->captured),
                                             copy_type_impl(arrow->returns)});

      if (auto list = get_if<list_type>(t.value()))
        return make_object<Type>(list_type {copy_type_impl(list->t)});

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
  // same_type

  namespace detail {

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

      if (auto lvar = get_if<value_type>(l))
        if (auto rvar = get_if<value_type>(r))
          return value_type::equal(*lvar, *rvar);

      if (auto larr = get_if<arrow_type>(l))
        if (auto rarr = get_if<arrow_type>(r))
          return same_type_impl(larr->captured, rarr->captured) &&
                 same_type_impl(larr->returns, rarr->returns);

      if (auto lany = get_if<var_type>(l))
        if (auto rany = get_if<var_type>(r))
          return lany->id == rany->id;

      if (auto llist = get_if<list_type>(l))
        if (auto rlist = get_if<list_type>(r))
          return same_type_impl(llist->t, rlist->t);

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
  // type arrow map

  struct var_type_comp
  {
    bool operator()(
      const object_ptr<const Type>& l,
      const object_ptr<const Type>& r) const noexcept
    {
      return (
        get_if<var_type>(l.value())->id < get_if<var_type>(r.value())->id);
    }
  };

  /// type_arrow map
  class type_arrow_map
  {
  public:
    type_arrow_map()                 = default;
    type_arrow_map(type_arrow_map&&) = default;
    type_arrow_map& operator=(type_arrow_map&&) = default;

    void insert(const type_arrow& ta)
    {
      assert(is_var_type(ta.from));
      auto p = m_map.try_emplace(ta.from, ta.to);

      if (!p.second)
        std::logic_error("duplicated insertion");
    }

    [[nodiscard]] auto find(const object_ptr<const Type>& from) const
      -> std::optional<type_arrow>
    {
      auto it = m_map.find(from);

      if (it == m_map.end())
        return std::nullopt;

      return type_arrow {it->first, it->second};
    }

    void erase(const object_ptr<const Type>& from)
    {
      m_map.erase(from);
    }

    template <class F>
    void for_each(F&& func) const
    {
      for (auto&& p : m_map) {
        std::forward<F>(func)(p.first, p.second);
      }
    }

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
  // subst_type

  namespace detail {

    // returns nullptr for identical subtree
    inline auto subst_impl_rec(
      const type_arrow& ta,
      const object_ptr<const Type>& in) -> object_ptr<const Type>
    {
      if (auto arrow = get_if<arrow_type>(in.value())) {
        auto cap = subst_impl_rec(ta, arrow->captured);
        auto ret = subst_impl_rec(ta, arrow->returns);
        return (!cap && !ret)
                 ? nullptr
                 : make_object<Type>(arrow_type {cap ? cap : arrow->captured,
                                                 ret ? ret : arrow->returns});
      }

      if (auto list = get_if<list_type>(in.value())) {
        auto t = subst_impl_rec(ta, list->t);
        return !t ? nullptr : make_object<Type>(list_type {t});
      }

      if (same_type(in, ta.from))
        return ta.to;

      return nullptr;
    }

    inline auto subst_impl(
      const type_arrow& ta,
      const object_ptr<const Type>& in) -> object_ptr<const Type>
    {
      auto r = subst_impl_rec(ta, in);
      return r ? r : in;
    }

  } // namespace detail

  /// emulate type-substitution
  [[nodiscard]] inline auto subst_type(
    const type_arrow& ta,
    const object_ptr<const Type>& in) -> object_ptr<const Type>
  {
    return detail::subst_impl(ta, in);
  }

  // ------------------------------------------
  // subst_all

  /// apply all substitution
  [[nodiscard]] inline auto subst_type_all(
    const type_arrow_map& tyarrows,
    const object_ptr<const Type>& ty) -> object_ptr<const Type>
  {
    auto tmp = ty;
    tyarrows.for_each([&](auto& from, auto& to) {
      tmp = subst_type({from, to}, tmp);
    });
    return tmp;
  }

  // ------------------------------------------
  // apply_subst

  /// apply substitution (without adding to context)
  inline void apply_subst(type_arrow_map& tyarrows, const type_arrow& a)
  {
    tyarrows.for_each([&](auto&, auto& to) { to = subst_type(a, to); });
  }

  // ------------------------------------------
  // compose_subst

  /// compose substitution
  inline void compose_subst(type_arrow_map& tyarrows, const type_arrow& a)
  {
    apply_subst(tyarrows, a);

    if (tyarrows.find(a.from))
      return;

    tyarrows.insert(a);
  }

  // ------------------------------------------
  // subst_constr

  /// subst_constr
  [[nodiscard]] inline auto subst_constr(
    const type_arrow& ta,
    const type_constr& constr) -> type_constr
  {
    return {subst_type(ta, constr.t1), subst_type(ta, constr.t2)};
  }

  // ------------------------------------------
  // subst_constr_all

  /// subst_constr_all
  [[nodiscard]] inline auto subst_constr_all(
    const type_arrow& ta,
    const std::vector<type_constr>& cs) -> std::vector<type_constr>
  {
    auto ret = std::vector<type_constr> {};
    ret.reserve(cs.size());

    for (auto&& c : cs)
      ret.push_back(subst_constr(ta, c));

    return ret;
  }

  // ------------------------------------------
  // occurs

  /// occurs
  [[nodiscard]] inline bool occurs(
    const object_ptr<const Type>& x,
    const object_ptr<const Type>& t)
  {
    if (get_if<value_type>(t.value()))
      return false;

    if (get_if<var_type>(t.value()))
      return same_type(x, t);

    if (auto arrow = get_if<arrow_type>(t.value()))
      return occurs(x, arrow->captured) || occurs(x, arrow->returns);

    if (auto list = get_if<list_type>(t.value()))
      return occurs(x, list->t);

    unreachable();
  }

  // ------------------------------------------
  // unify

  /// unify
  /// \param cs Type constraints
  /// \param src Source node (for error handling)
  [[nodiscard]] inline auto unify(
    const std::vector<type_constr>& constrs,
    const object_ptr<const Object>& src) -> type_arrow_map
  {
    auto cs = constrs;
    auto ta = type_arrow_map {};

    while (!cs.empty()) {
      auto c = cs.back();
      cs.pop_back();

      if (same_type(c.t1, c.t2))
        continue;

      if (is_var_type(c.t2)) {
        if (likely(!occurs(c.t2, c.t1))) {
          auto arr = type_arrow {c.t2, c.t1};
          cs       = subst_constr_all(arr, cs);
          compose_subst(ta, arr);
          continue;
        }
        throw type_error::circular_constraint(src, c.t1);
      }

      if (is_var_type(c.t1)) {
        if (likely(!occurs(c.t1, c.t2))) {
          auto arr = type_arrow {c.t1, c.t2};
          cs       = subst_constr_all(arr, cs);
          compose_subst(ta, arr);
          continue;
        }
        throw type_error::circular_constraint(src, c.t1);
      }

      if (is_arrow_type(c.t1) && is_arrow_type(c.t2)) {
        auto* arrow1 = get_if<arrow_type>(c.t1.value());
        auto* arrow2 = get_if<arrow_type>(c.t2.value());
        cs.push_back({arrow1->captured, arrow2->captured});
        cs.push_back({arrow1->returns, arrow2->returns});
        continue;
      }

      if (is_list_type(c.t1) && is_list_type(c.t2)) {
        auto list1 = get_if<list_type>(c.t1.value());
        auto list2 = get_if<list_type>(c.t2.value());
        cs.push_back({list1->t, list2->t});
        continue;
      }

      throw type_error::type_missmatch(src, c.t1, c.t2);
    }
    return ta;
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
      // arrow
      if (is_arrow_type(t1) && is_arrow_type(t2)) {
        auto* a1 = get_if<arrow_type>(t1.value());
        auto* a2 = get_if<arrow_type>(t2.value());
        return make_object<Type>(
          arrow_type {generalize_impl_rec(a1->captured, a2->captured, table),
                      generalize_impl_rec(a1->returns, a2->returns, table)});
      }

      // list
      if (is_list_type(t1) && is_list_type(t2)) {
        auto* l1 = get_if<list_type>(t1.value());
        auto* l2 = get_if<list_type>(t2.value());
        return make_object<Type>(
          list_type {generalize_impl_rec(l1->t, l2->t, table)});
      }

      // (T,T) -> T
      if (same_type(t1, t2))
        return t1;

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
  /// \param ts Not-empty list of types
  [[nodiscard]] inline auto generalize(
    const std::vector<object_ptr<const Type>>& ts) -> object_ptr<const Type>
  {
    assert(!ts.empty());
    return detail::generalize_impl(ts);
  }

  // ------------------------------------------
  // specializable

  namespace detail {

    inline bool specializable_impl(
      std::vector<type_constr>& cs,
      type_arrow_map& ta)
    {
      while (!cs.empty()) {
        auto c = cs.back();
        cs.pop_back();

        if (same_type(c.t1, c.t2))
          continue;

        if (is_var_type(c.t1)) {
          if (likely(!occurs(c.t1, c.t2))) {
            auto arr = type_arrow {c.t1, c.t2};
            cs       = subst_constr_all(arr, cs);
            compose_subst(ta, arr);
            continue;
          }
          return false;
        }

        // arrow
        if (auto arrow1 = get_if<arrow_type>(c.t1.value())) {
          if (auto arrow2 = get_if<arrow_type>(c.t2.value())) {
            cs.push_back({arrow1->captured, arrow2->captured});
            cs.push_back({arrow1->returns, arrow2->returns});
            continue;
          }
          return false;
        }

        // list
        if (auto list1 = get_if<list_type>(c.t1.value())) {
          if (auto list2 = get_if<list_type>(c.t2.value())) {
            cs.push_back({list1->t, list2->t});
            continue;
          }
          return false;
        }

        // value
        if (is_value_type(c.t1)) {
          if (!same_type(c.t1, c.t2))
            return false;
          else
            continue;
        }

        unreachable();
      }
      return true;
    }

  } // namespace detail

  /// specializable
  [[nodiscard]] inline auto specializable(
    const object_ptr<const Type>& t1,
    const object_ptr<const Type>& t2) -> std::optional<type_arrow_map>
  {
    assert(t1 && t2);

    std::vector<type_constr> constrs = {{t1, t2}};
    type_arrow_map arr;

    if (detail::specializable_impl(constrs, arr))
      return arr;
    else
      return std::nullopt;
  }

  // ------------------------------------------
  // vars

  namespace detail {

    inline void vars_impl(
      const object_ptr<const Type>& tp,
      std::vector<object_ptr<const Type>>& vars)
    {
      if (get_if<value_type>(tp.value()))
        return;

      if (get_if<var_type>(tp.value())) {
        for (auto&& v : vars) {
          if (same_type(v, tp))
            return;
        }
        vars.push_back(tp);
        return;
      }

      if (auto arrow = get_if<arrow_type>(tp.value())) {
        vars_impl(arrow->captured, vars);
        vars_impl(arrow->returns, vars);
        return;
      }

      if (auto list = get_if<list_type>(tp.value())) {
        vars_impl(list->t, vars);
        return;
      }

      unreachable();
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
  [[nodiscard]] inline auto vars(const type_arrow_map& map)
    -> std::vector<object_ptr<const Type>>
  {
    std::vector<object_ptr<const Type>> ret;
    map.for_each([&](auto&, auto& to) {
      auto v = vars(to);
      ret.insert(ret.end(), v.begin(), v.end());
    });
    return ret;
  }

  // ------------------------------------------
  // genpoly

  /// create fresh polymorphic closure type
  [[nodiscard]] inline auto genpoly(
    const object_ptr<const Type>& tp,
    const type_arrow_map& env) -> object_ptr<const Type>
  {
    if (!is_arrow_type(tp))
      return tp;

    auto vs = vars(tp);
    auto t  = tp;

    for (auto v : vs) {

      if (env.find(v))
        continue;

      auto a = type_arrow {v, genvar()};
      t      = subst_type(a, t);
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

        auto var = genvar();
        auto cs  = std::vector {type_constr {
          subst_type_all(env, t1), make_object<Type>(arrow_type {t2, var})}};
        auto as  = unify(std::move(cs), obj);
        auto ty  = subst_type_all(as, var);

        as.erase(var);
        as.for_each([&](auto& from, auto& to) {
          compose_subst(env, {from, to});
        });

        return ty;
      }

      // Lambda
      if (auto lambda = value_cast_if<Lambda>(obj)) {

        auto& storage = _get_storage(*lambda);

        auto var = make_object<Type>(var_type {storage.var->id()});
        env.insert(type_arrow {var, var});

        auto t1 = type_of_impl(storage.var, env);
        auto t2 = type_of_impl(storage.body, env);

        auto ty = make_object<Type>(arrow_type {subst_type_all(env, t1), t2});

        env.erase(t1);

        return ty;
      }

      // Variable
      if (auto variable = value_cast_if<Variable>(obj)) {

        auto var = make_object<Type>(var_type {variable->id()});
        if (auto s = env.find(var))
          return s->to;

        throw type_error::unbounded_variable(obj, var);
      }

      // arrow -> arrow or PAP
      if (has_arrow_type(obj)) {
        auto c = reinterpret_cast<const Closure<>*>(obj.get());
        // pap: return root apply node
        // app: get_type
        return c->is_pap() ? type_of_impl(c->vertebrae(c->arity), env)
                           : genpoly(get_type(obj), env);
      }

      // list:
      //  | []   : [] a
      //  | x:xs : [type_of(x)]
      if (has_list_type(obj)) {

        auto list = static_object_cast<const List<Object>>(obj);

        auto& storage = _get_storage(*list);

        if (storage.is_nil())
          return get_type(obj);
        else
          return make_object<Type>(list_type {type_of_impl(storage.car, env)});
      }

      // value -> value
      if (has_value_type(obj))
        return get_type(obj);

      // var -> var
      if (has_var_type(obj))
        return get_type(obj);

      unreachable();
    }
  } // namespace detail

  /// dynamic type checker.
  [[nodiscard]] inline auto type_of(const object_ptr<const Object>& obj)
    -> object_ptr<const Type>
  {
    type_arrow_map env;  // type environment

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
      throw type_error::bad_type_check(obj, t1, t2);
  }
} // namespace yave