//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/rts/generalize.hpp>
#include <yave/rts/dynamic_typing.hpp>

namespace yave {

  namespace {
    struct generalize_table
    {
      object_ptr<const Type> t1;
      object_ptr<const Type> t2;
      object_ptr<const Type> g;
    };

    object_ptr<const Type> generalize_impl(
      const object_ptr<const Type>& t1,
      const object_ptr<const Type>& t2,
      std::vector<generalize_table>& table)
    {
      // dispatch
      if (is_arrow_type(t1) && is_arrow_type(t2)) {
        auto& a1 = get<arrow_type>(*t1);
        auto& a2 = get<arrow_type>(*t2);
        return object_ptr<const Type>(new Type(
          arrow_type {generalize_impl(a1.captured, a2.captured, table),
                      generalize_impl(a1.returns, a2.returns, table)}));
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
  } // namespace

  object_ptr<const Type>
    generalize(const std::vector<object_ptr<const Type>>& ts)
  {
    assert(!ts.empty());
    object_ptr<const Type> tp = ts.front();
    std::vector<generalize_table> table;
    for (size_t i = 1; i < ts.size(); ++i) {
      tp = generalize_impl(tp, ts[i], table);
      table.clear();
    }
    return tp;
  }

  namespace {
    std::vector<Constr> specializable_constr(
      const object_ptr<const Type>& t1,
      const object_ptr<const Type>& t2)
    {
      if (is_arrow_type(t1) && is_arrow_type(t2)) {
        auto& a1 = get<arrow_type>(*t1);
        auto& a2 = get<arrow_type>(*t2);
        auto l   = specializable_constr(a1.captured, a2.captured);
        auto r   = specializable_constr(a1.returns, a2.returns);
        // concat
        l.insert(l.end(), r.begin(), r.end());
        return l;
      }
      return {Constr {t1, t2}};
    }

    bool specializable_unify(std::vector<Constr>& cs)
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
  } // namespace

  bool specializable(
    const object_ptr<const Type>& t1,
    const object_ptr<const Type>& t2)
  {
    auto cs = specializable_constr(t1, t2);
    return specializable_unify(cs);
  }

} // namespace yave