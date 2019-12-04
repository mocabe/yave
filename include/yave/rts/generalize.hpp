//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/dynamic_typing.hpp>

#include <algorithm>

namespace yave {

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
        return object_ptr<Type>(new Type(
          arrow_type {generalize_impl_rec(a1->captured, a2->captured, table),
                      generalize_impl_rec(a1->returns, a2->returns, table)}));
      }

      // list
      if (is_list_type(t1) && is_list_type(t2)) {
        auto* l1 = get_if<list_type>(t1.value());
        auto* l2 = get_if<list_type>(t2.value());
        return object_ptr<Type>(
          new Type(list_type {generalize_impl_rec(l1->t, l2->t, table)}));
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

    inline auto specializable_impl_constr(
      const object_ptr<const Type>& t1,
      const object_ptr<const Type>& t2) -> std::vector<type_constr>
    {
      // arrow
      if (is_arrow_type(t1) && is_arrow_type(t2)) {
        auto* a1 = get_if<arrow_type>(t1.value());
        auto* a2 = get_if<arrow_type>(t2.value());
        auto l   = specializable_impl_constr(a1->captured, a2->captured);
        auto r   = specializable_impl_constr(a1->returns, a2->returns);
        // concat
        l.insert(l.end(), r.begin(), r.end());
        return l;
      }

      // list
      if (is_list_type(t1) && is_list_type(t2)) {
        auto* l1 = get_if<list_type>(t1.value());
        auto* l2 = get_if<list_type>(t2.value());
        return specializable_impl_constr(l1->t, l2->t);
      }

      return {type_constr {t1, t2}};
    }

  } // namespace detail

  /// specializable
  [[nodiscard]] inline bool specializable(
    const object_ptr<const Type>& t1,
    const object_ptr<const Type>& t2)
  {
    assert(t1 && t2);

    try {
      (void)unify(detail::specializable_impl_constr(t1, t2));
      return true;
    } catch (type_error::type_error&) {
      return false;
    }
  }

} // namespace yave