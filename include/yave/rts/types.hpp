//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/kinds.hpp>
#include <yave/rts/meta_tuple.hpp>

namespace yave {

  // ------------------------------------------
  // Type primitives

  /// Type variable
  template <class Tag, class Kind>
  struct tvar
  {
  };

  /// Type constructor
  template <class Tag, class Kind>
  struct tcon
  {
  };

  /// Type level application
  template <class T1, class T2>
  struct tap
  {
  };

  template <class Tag, class Kind>
  struct meta_type<tvar<Tag, Kind>>
  {
    [[nodiscard]] constexpr auto tag() const
    {
      return type_c<Tag>;
    }
    [[nodiscard]] constexpr auto kind() const
    {
      return type_c<Kind>;
    }
  };

  template <class Tag, class Kind>
  struct meta_type<tcon<Tag, Kind>>
  {
    [[nodiscard]] constexpr auto tag() const
    {
      return type_c<Tag>;
    }
    [[nodiscard]] constexpr auto kind() const
    {
      return type_c<Kind>;
    }
  };

  template <class T1, class T2>
  struct meta_type<tap<T1, T2>>
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

  template <class Tag, class Kind>
  [[nodiscard]] constexpr auto make_tvar(meta_type<Tag>, meta_type<Kind>)
  {
    return type_c<tvar<Tag, Kind>>;
  }

  template <class Tag, class Kind>
  [[nodiscard]] constexpr auto make_tcon(meta_type<Tag>, meta_type<Kind>)
  {
    return type_c<tcon<Tag, Kind>>;
  }

  template <class T1, class T2>
  [[nodiscard]] constexpr auto make_tap(meta_type<T1>, meta_type<T2>)
  {
    return type_c<tap<T1, T2>>;
  }

  // ------------------------------------------
  // Type constructor tags

  struct arrow_tcon_tag;

  struct list_tcon_tag;

  template <class Tag>
  struct varvalue_tcon_tag;

  template <size_t N>
  struct var_tcon_tag;

  template <class T>
  struct value_tcon_tag;

  // ------------------------------------------
  // Types

  // ty_var
  template <class Tag>
  using ty_var = tvar<Tag, kstar>;

  // ty_varvalue
  template <class Tag>
  using ty_varvalue = tcon<varvalue_tcon_tag<Tag>, kstar>;

  // ty_value
  template <class T>
  using ty_value = tcon<value_tcon_tag<T>, kstar>;

  // ty_arrow
  template <class T1, class T2>
  using ty_arrow =
    tap<tap<tcon<arrow_tcon_tag, kfun<kstar, kfun<kstar, kstar>>>, T1>, T2>;

  // ty_list
  template <class T>
  using ty_list = tap<tcon<list_tcon_tag, kfun<kstar, kstar>>, T>;

  // ------------------------------------------
  // kind_of

  template <class Tag, class Kind>
  [[nodiscard]] constexpr auto kind_of(meta_type<tvar<Tag, Kind>>)
  {
    return type_c<Kind>;
  }

  template <class Tag, class Kind>
  [[nodiscard]] constexpr auto kind_of(meta_type<tcon<Tag, Kind>>)
  {
    return type_c<Kind>;
  }

  template <class T1, class T2>
  [[nodiscard]] constexpr auto kind_of(meta_type<tap<T1, T2>>)
  {
    return kind_of(type_c<T1>).k2();
  }

  // ------------------------------------------
  // tyarrow

  /// Type mapping
  template <class T1, class T2>
  struct tyarrow
  {
  };

  template <class T1, class T2>
  struct meta_type<tyarrow<T1, T2>>
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

  // ------------------------------------------
  // tyerror

  /// Type error
  template <class Tag>
  struct tyerror
  {
  };

  template <class Tag>
  struct meta_type<tyerror<Tag>>
  {
    [[nodiscard]] constexpr auto tag() const
    {
      return type_c<Tag>;
    }
  };

  /// error tag types
  namespace error_tags {

    /// type_missmatch
    template <class T1, class T2, class Other>
    struct type_missmatch
    {
      using left  = T1;
      using right = T2;
      using other = Other;
    };

    template <class K1, class K2, class Other>
    struct kind_missmatch
    {
      using left  = K1;
      using right = K2;
      using other = Other;
    };

    /// unsolvable_constraints
    template <class T1, class T2, class Other>
    struct unsolvable_constraints
    {
      using left  = T1;
      using right = T2;
      using other = Other;
    };

    /// circular_constraints
    template <class Var, class Other>
    struct circular_constraints
    {
      using var   = Var;
      using other = Other;
    };

    /// none
    struct unknown_error
    {
    };

  } // namespace error_tags

  // ------------------------------------------
  // meta_type specializations

  template <class T1, class T2>
  [[nodiscard]] constexpr auto make_ty_arrow(meta_type<T1>, meta_type<T2>)
  {
    return type_c<ty_arrow<T1, T2>>;
  }

  template <class T1, class T2>
  [[nodiscard]] constexpr auto make_tyarrow(meta_type<T1>, meta_type<T2>)
  {
    return type_c<tyarrow<T1, T2>>;
  }

  template <class Tag>
  [[nodiscard]] constexpr auto make_ty_value(meta_type<Tag>)
  {
    return type_c<ty_value<Tag>>;
  }

  template <class T>
  [[nodiscard]] constexpr auto make_ty_var(meta_type<T>)
  {
    return type_c<ty_var<T>>;
  }

  template <class Tag>
  [[nodiscard]] constexpr auto make_ty_varvalue(meta_type<Tag>)
  {
    return type_c<ty_varvalue<Tag>>;
  }

  template <class T>
  [[nodiscard]] constexpr auto make_ty_list(meta_type<T>)
  {
    return type_c<ty_list<T>>;
  }

  template <class T1, class T2, class Other = meta_tuple<>>
  [[nodiscard]] constexpr auto make_unsolvable_constraints(
    meta_type<T1>,
    meta_type<T2>,
    Other = {})
  {
    return type_c<tyerror<error_tags::unsolvable_constraints<T1, T2, Other>>>;
  }

  template <class T1, class T2, class Other = meta_tuple<>>
  [[nodiscard]] constexpr auto make_type_missmatch(
    meta_type<T1>,
    meta_type<T2>,
    Other = {})
  {
    return type_c<tyerror<error_tags::type_missmatch<T1, T2, Other>>>;
  }

  template <class T1, class T2, class Other = meta_tuple<>>
  [[nodiscard]] constexpr auto make_kind_missmatch(
    meta_type<T1>,
    meta_type<T2>,
    Other = {})
  {
    return type_c<tyerror<error_tags::kind_missmatch<T1, T2, Other>>>;
  }

  template <class Var, class Other = meta_tuple<>>
  [[nodiscard]] constexpr auto make_circular_constraints(
    meta_type<Var>,
    Other = {})
  {
    return type_c<tyerror<error_tags::circular_constraints<Var, Other>>>;
  }

  [[nodiscard]] constexpr auto make_unknown_error()
  {
    return type_c<tyerror<error_tags::unknown_error>>;
  }

  // ------------------------------------------
  // is_tyerror

  template <class T>
  [[nodiscard]] constexpr auto is_tyerror(meta_type<tyerror<T>>)
  {
    return true_c;
  }

  template <class T>
  [[nodiscard]] constexpr auto is_tyerror(T)
  {
    return false_c;
  }

  // ------------------------------------------
  // is_type_missmatch

  template <class T1, class T2, class Other>
  [[nodiscard]] constexpr auto is_type_missmatch(
    meta_type<error_tags::type_missmatch<T1, T2, Other>>)
  {
    return true_c;
  }

  template <class T>
  [[nodiscard]] constexpr auto is_type_missmatch(T)
  {
    return false_c;
  }

  // ------------------------------------------
  // is_unsolvable_constraints

  template <class T1, class T2, class Other>
  [[nodiscard]] constexpr auto is_unsolvable_constraints(
    meta_type<error_tags::unsolvable_constraints<T1, T2, Other>>)
  {
    return true_c;
  }

  template <class T>
  [[nodiscard]] constexpr auto is_unsolvable_constraints(T)
  {
    return false_c;
  }

  // ------------------------------------------
  // is_circular_constraints

  template <class Var, class Other>
  [[nodiscard]] constexpr auto is_circular_constraints(
    meta_type<error_tags::circular_constraints<Var, Other>>)
  {
    return true_c;
  }

  template <class T>
  [[nodiscard]] constexpr auto is_circular_constraints(T)
  {
    return false_c;
  }

  // ------------------------------------------
  // is_unknown_error

  [[nodiscard]] constexpr auto is_unknown_error(
    meta_type<error_tags::unknown_error>)
  {
    return true_c;
  }

  template <class T>
  [[nodiscard]] constexpr auto is_unknown_error(T)
  {
    return false_c;
  }

  // ------------------------------------------
  // is_tvar

  template <class Tag, class Kind>
  [[nodiscard]] constexpr auto is_tvar(meta_type<tvar<Tag, Kind>>)
  {
    return true_c;
  }

  template <class T>
  [[nodiscard]] constexpr auto is_tvar(T)
  {
    return false_c;
  }

  // ------------------------------------------
  // is_tcon

  template <class Tag, class Kind>
  [[nodiscard]] constexpr auto is_tcon(meta_type<tcon<Tag, Kind>>)
  {
    return true_c;
  }

  template <class T>
  [[nodiscard]] constexpr auto is_tcon(T)
  {
    return false_c;
  }

  // ------------------------------------------
  // is_tap

  template <class T1, class T2>
  [[nodiscard]] constexpr auto is_tap(meta_type<tap<T1, T2>>)
  {
    return true_c;
  }

  template <class T>
  [[nodiscard]] constexpr auto is_tap(T)
  {
    return false_c;
  }

  // ------------------------------------------
  // gen_c

  template <size_t N>
  constexpr auto gen_c = type_c<var_tcon_tag<N>>;

  // ------------------------------------------
  // nextgen

  template <size_t N>
  [[nodiscard]] constexpr auto nextgen(meta_type<var_tcon_tag<N>>)
  {
    return type_c<var_tcon_tag<N + 1>>;
  }

  // ------------------------------------------
  // genvar

  template <size_t N>
  [[nodiscard]] constexpr auto genvar(meta_type<var_tcon_tag<N>>)
  {
    return type_c<ty_var<var_tcon_tag<N>>>;
  }

} // namespace yave