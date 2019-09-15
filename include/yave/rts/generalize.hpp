//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/dynamic_typing.hpp>

namespace yave {

  /// Anti-unification.
  /// \param ts Not-empty list of types
  [[nodiscard]] inline auto generalize(
    const std::vector<object_ptr<const Type>>& ts) -> object_ptr<const Type>
  {
    // generalization table
    struct _table
    {
      object_ptr<const Type> t1;
      object_ptr<const Type> t2;
      object_ptr<const Type> g;
    };

    struct
    {
      auto rec(
        const object_ptr<const Type>& t1,
        const object_ptr<const Type>& t2,
        std::vector<_table>& table) -> object_ptr<const Type>
      {
        // dispatch
        if (is_arrow_type(t1) && is_arrow_type(t2)) {
          auto& a1 = get<arrow_type>(*t1);
          auto& a2 = get<arrow_type>(*t2);
          return object_ptr<const Type>(
            new Type(arrow_type {rec(a1.captured, a2.captured, table),
                                 rec(a1.returns, a2.returns, table)}));
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
    } _impl;

    assert(!ts.empty());
    object_ptr<const Type> tp = ts.front();
    std::vector<_table> table;
    for (size_t i = 1; i < ts.size(); ++i) {
      tp = _impl.rec(tp, ts[i], table);
      table.clear();
    }
    return tp;
  }

  /// specializable
  [[nodiscard]] inline bool specializable(
    const object_ptr<const Type>& t1,
    const object_ptr<const Type>& t2)
  {
    struct
    {
      auto constr(
        const object_ptr<const Type>& t1,
        const object_ptr<const Type>& t2) -> std::vector<Constr>
      {
        if (is_arrow_type(t1) && is_arrow_type(t2)) {
          auto& a1 = get<arrow_type>(*t1);
          auto& a2 = get<arrow_type>(*t2);
          auto l   = constr(a1.captured, a2.captured);
          auto r   = constr(a1.returns, a2.returns);
          // concat
          l.insert(l.end(), r.begin(), r.end());
          return l;
        }
        return {Constr {t1, t2}};
      }

      bool unify(std::vector<Constr>& cs)
      {
        while (!cs.empty()) {
          auto c = cs.back();
          cs.pop_back();
          if (auto arrow1 = get_if<arrow_type>(c.t1.value())) {
            if (auto arrow2 = get_if<arrow_type>(c.t2.value())) {
              cs.push_back({arrow1->captured, arrow2->captured});
              cs.push_back({arrow1->returns, arrow2->returns});
              continue;
            }
            return false;
          }
          if (is_var_type(c.t1)) {
            for (auto& cc : cs) {
              if (same_type(cc.t1, c.t1) && !same_type(cc.t2, c.t2))
                return false;
            }
            continue;
          }
          if (is_value_type(c.t1)) {
            if (!same_type(c.t1, c.t2))
              return false;
          }
        }
        return true;
      }
    } _impl;

    auto cs = _impl.constr(t1, t2);
    return _impl.unify(cs);
  }

} // namespace yave