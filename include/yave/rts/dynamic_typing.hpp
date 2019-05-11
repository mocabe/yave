//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/type_gen.hpp>
#include <yave/rts/apply.hpp>
#include <yave/rts/exception.hpp>
#include <yave/rts/type_error.hpp>
#include <yave/rts/utility.hpp>
#include <yave/rts/undefined.hpp>
#include <yave/rts/obj_util.hpp>

#include <vector>

namespace yave {

  // fwd
  template <class T, class U>
  [[nodiscard]] object_ptr<propagate_const_t<T, U>>
    value_cast_if(const object_ptr<U>& obj) noexcept;
  template <class T, class U>
  [[nodiscard]] object_ptr<propagate_const_t<T, U>>
    value_cast_if(object_ptr<U>&& obj) noexcept;

  // ------------------------------------------
  // Utils

  /// is_value_type
  [[nodiscard]] inline bool is_value_type(const object_ptr<const Type>& tp)
  {
    if (get_if<value_type>(tp.value()))
      return true;
    else
      return false;
  }

  /// is_arrow_type
  [[nodiscard]] inline bool is_arrow_type(const object_ptr<const Type>& tp)
  {
    if (get_if<arrow_type>(tp.value()))
      return true;
    else
      return false;
  }

  /// is_var_type
  [[nodiscard]] inline bool is_var_type(const object_ptr<const Type>& tp)
  {
    if (get_if<var_type>(tp.value()))
      return true;
    else
      return false;
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

  struct TyArrow
  {
    object_ptr<const Type> from;
    object_ptr<const Type> to;
  };

  struct Constr
  {
    object_ptr<const Type> t1;
    object_ptr<const Type> t2;
  };

  /// Deep copy type object
  [[nodiscard]] object_ptr<const Type>
    copy_type(const object_ptr<const Type>& tp);

  /// check type equality
  [[nodiscard]] bool same_type(
    const object_ptr<const Type>& lhs,
    const object_ptr<const Type>& rhs);

  /// emulate type-substitution
  [[nodiscard]] object_ptr<const Type>
    subst_type(const TyArrow& ta, const object_ptr<const Type>& in);

  /// apply all substitution
  [[nodiscard]] object_ptr<const Type> subst_type_all(
    const std::vector<TyArrow>& tyarrows,
    const object_ptr<const Type>& ty);

  /// compose substitution
  void compose_subst(std::vector<TyArrow>& tyarrows, const TyArrow& a);

  /// subst_constr
  [[nodiscard]] Constr subst_constr(const TyArrow& ta, const Constr& constr);

  /// subst_constr_all
  [[nodiscard]] std::vector<Constr>
    subst_constr_all(const TyArrow& ta, const std::vector<Constr>& cs);

  /// occurs
  [[nodiscard]] bool
    occurs(const object_ptr<const Type>& x, const object_ptr<const Type>& t);

  /// unify
  /// \param cs Type constraints
  /// \param src Source node (for error handling)
  [[nodiscard]] std::vector<TyArrow>
    unify(const std::vector<Constr>& cs, const object_ptr<const Object>& src);

  /// generate new type variable
  [[nodiscard]] object_ptr<const Type> genvar();

  /// get list of type variables
  [[nodiscard]] std::vector<object_ptr<const Type>>
    vars(const object_ptr<const Type>& tp);

  /// create fresh polymorphic closure type
  [[nodiscard]] object_ptr<const Type>
    genpoly(const object_ptr<const Type>& tp);

  /// type_of
  [[nodiscard]] object_ptr<const Type>
    type_of(const object_ptr<const Object>& obj);

  /// has_type
  template <class T, class U>
  [[nodiscard]] bool has_type(const object_ptr<U>& obj)
  {
    if (same_type(get_type(obj), object_type<T>()))
      return true;
    else
      return false;
  }

} // namespace yave