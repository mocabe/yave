//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/meta_type.hpp>

namespace yave {

  // ------------------------------------------
  // Types

  /// Arrow type
  template <class T1, class T2>
  struct ty_arrow
  {
  };

  /// Type variable
  template <class Tag>
  struct ty_var
  {
  };

  /// Value type
  template <class Tag>
  struct ty_value
  {
  };

  /// Var value
  template <class Tag>
  struct ty_varvalue
  {
  };

  /// List type
  template <class T>
  struct ty_list
  {
  };

  /// Type mapping
  template <class T1, class T2>
  struct tyarrow
  {
  };

  /// error_type
  template <class Tag>
  struct error_type
  {
    [[nodiscard]] constexpr auto tag() const
    {
      return type_c<Tag>;
    }
  };

  /// error tag types
  namespace error_tags {

    /// type_missmatch
    template <class T1, class T2, class Other = std::tuple<>>
    struct type_missmatch
    {
      using left  = T1;
      using right = T2;
      using other = Other;
    };

    /// unsolvable_constraints
    template <class T1, class T2, class Other = std::tuple<>>
    struct unsolvable_constraints
    {
      using left  = T1;
      using right = T2;
      using other = Other;
    };

    /// circular_constraints
    template <class Var, class Other = std::tuple<>>
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
  struct meta_type<ty_arrow<T1, T2>>
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
  [[nodiscard]] constexpr auto make_ty_arrow(meta_type<T1>, meta_type<T2>)
  {
    return type_c<ty_arrow<T1, T2>>;
  }

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

  template <class T1, class T2>
  [[nodiscard]] constexpr auto make_tyarrow(meta_type<T1>, meta_type<T2>)
  {
    return type_c<tyarrow<T1, T2>>;
  }

  template <class Tag>
  struct meta_type<ty_value<Tag>>
  {
    [[nodiscard]] constexpr auto tag() const
    {
      return type_c<Tag>;
    }
  };

  template <class Tag>
  [[nodiscard]] constexpr auto make_ty_value(meta_type<Tag>)
  {
    return type_c<ty_value<Tag>>;
  }

  template <class Tag>
  struct meta_type<ty_var<Tag>>
  {
    [[nodiscard]] constexpr auto tag() const
    {
      return type_c<Tag>;
    }
  };

  template <class Tag>
  [[nodiscard]] constexpr auto make_ty_var(meta_type<Tag>)
  {
    return type_c<ty_var<Tag>>;
  }

  template <class Tag>
  struct meta_type<ty_varvalue<Tag>>
  {
    [[nodiscard]] constexpr auto tag() const
    {
      return type_c<Tag>;
    }
  };

  template <class Tag>
  [[nodiscard]] constexpr auto make_ty_varvalue(meta_type<Tag>)
  {
    return type_c<ty_varvalue<Tag>>;
  }

  template <class Tag>
  struct meta_type<error_type<Tag>>
  {
    [[nodiscard]] constexpr auto tag() const
    {
      return type_c<Tag>;
    }
  };

  template <class T>
  [[nodiscard]] constexpr auto make_ty_list(meta_type<T>)
  {
    return type_c<ty_list<T>>;
  }

  template <class T>
  struct meta_type<ty_list<T>>
  {
    [[nodiscard]] constexpr auto t() const
    {
      return type_c<T>;
    }
  };

  template <class T1, class T2, class Other>
  [[nodiscard]] constexpr auto make_unsolvable_constraints(
    meta_type<T1>,
    meta_type<T2>,
    Other)
  {
    return type_c<
      error_type<error_tags::unsolvable_constraints<T1, T2, Other>>>;
  }

  template <class T1, class T2, class Other>
  [[nodiscard]] constexpr auto make_type_missmatch(
    meta_type<T1>,
    meta_type<T2>,
    Other)
  {
    return type_c<error_type<error_tags::type_missmatch<T1, T2, Other>>>;
  }

  template <class Var, class Other>
  [[nodiscard]] constexpr auto make_circular_constraints(meta_type<Var>, Other)
  {
    return type_c<error_type<error_tags::circular_constraints<Var, Other>>>;
  }

  // ------------------------------------------
  // is_error_type

  template <class T>
  struct is_error_type_impl
  {
    static constexpr std::false_type value {};
  };

  template <class Tag>
  struct is_error_type_impl<error_type<Tag>>
  {
    static constexpr std::true_type value {};
  };

  template <class T>
  [[nodiscard]] constexpr auto is_error_type(meta_type<T>)
  {
    return is_error_type_impl<T>::value;
  }

  template <class T>
  [[nodiscard]] constexpr auto is_error_type(T)
  {
    return std::false_type {};
  }

  // ------------------------------------------
  // is_type_missmatch

  template <class T>
  struct is_type_missmatch_impl
  {
    static constexpr std::false_type value {};
  };

  template <class T1, class T2, class Other>
  struct is_type_missmatch_impl<error_tags::type_missmatch<T1, T2, Other>>
  {
    static constexpr std::true_type value {};
  };

  template <class T>
  [[nodiscard]] constexpr auto is_type_missmatch(meta_type<T>)
  {
    return is_type_missmatch_impl<T>::value;
  }

  // ------------------------------------------
  // is_unsolvable_constraints

  template <class T>
  struct is_unsolvable_constraints_impl
  {
    static constexpr std::false_type value {};
  };

  template <class T1, class T2, class Other>
  struct is_unsolvable_constraints_impl<
    error_tags::unsolvable_constraints<T1, T2, Other>>
  {
    static constexpr std::true_type value {};
  };

  template <class T>
  [[nodiscard]] constexpr auto is_unsolvable_constraints(meta_type<T>)
  {
    return is_unsolvable_constraints_impl<T>::value;
  }

  // ------------------------------------------
  // is_circular_constraints

  template <class T>
  struct is_circular_constraints_impl
  {
    static constexpr std::false_type value {};
  };

  template <class Var, class Other>
  struct is_circular_constraints_impl<
    error_tags::circular_constraints<Var, Other>>
  {
    static constexpr std::true_type value {};
  };

  template <class T>
  [[nodiscard]] constexpr auto is_circular_constraints(meta_type<T>)
  {
    return is_circular_constraints_impl<T>::value;
  }

  // ------------------------------------------
  // is_unknown_error

  template <class T>
  struct is_unknown_error_impl
  {
    static constexpr std::false_type value {};
  };

  template <>
  struct is_unknown_error_impl<error_tags::unknown_error>
  {
    static constexpr std::true_type value {};
  };

  template <class T>
  [[nodiscard]] constexpr auto is_unknown_error(meta_type<T>)
  {
    return is_unknown_error_impl<T>::value;
  }

  // ------------------------------------------
  // is_ty_value

  template <class T>
  struct is_ty_value_impl
  {
    static constexpr std::false_type value {};
  };

  template <class Tag>
  struct is_ty_value_impl<ty_value<Tag>>
  {
    static constexpr std::true_type value {};
  };

  template <class T>
  [[nodiscard]] constexpr auto is_ty_value(meta_type<T>)
  {
    return is_ty_value_impl<T>::value;
  }

  // ------------------------------------------
  // is_ty_arrow

  template <class T>
  struct is_ty_arrow_impl
  {
    static constexpr std::false_type value {};
  };

  template <class T1, class T2>
  struct is_ty_arrow_impl<ty_arrow<T1, T2>>
  {
    static constexpr std::true_type value {};
  };

  template <class T>
  [[nodiscard]] constexpr auto is_ty_arrow(meta_type<T>)
  {
    return is_ty_arrow_impl<T>::value;
  }

  // ------------------------------------------
  // is_ty_var

  template <class T>
  struct is_ty_var_impl
  {
    static constexpr std::false_type value {};
  };

  template <class Tag>
  struct is_ty_var_impl<ty_var<Tag>>
  {
    static constexpr std::true_type value {};
  };

  template <class T>
  [[nodiscard]] constexpr auto is_ty_var(meta_type<T>)
  {
    return is_ty_var_impl<T>::value;
  }

  // ------------------------------------------
  // is_ty_varvalue

  template <class T>
  struct is_ty_varvalue_impl
  {
    static constexpr std::false_type value {};
  };

  template <class Tag>
  struct is_ty_varvalue_impl<ty_varvalue<Tag>>
  {
    static constexpr std::true_type value {};
  };

  template <class T>
  [[nodiscard]] constexpr auto is_ty_varvalue(meta_type<T>)
  {
    return is_ty_varvalue_impl<T>::value;
  }

  // ------------------------------------------
  // is_ty_list

  template <class T>
  struct is_ty_list_impl
  {
    static constexpr std::false_type value {};
  };

  template <class T>
  struct is_ty_list_impl<ty_list<T>>
  {
    static constexpr std::true_type value {};
  };

  template <class T>
  [[nodiscard]] constexpr auto is_ty_list(meta_type<T>)
  {
    return is_ty_list_impl<T>::value;
  }

  // ------------------------------------------
  // taggen

  template <size_t N>
  struct taggen
  {
  };

  template <size_t N>
  constexpr auto gen_c = type_c<taggen<N>>;

  // ------------------------------------------
  // nextgen

  template <size_t N>
  [[nodiscard]] constexpr auto nextgen(meta_type<taggen<N>>)
  {
    return type_c<taggen<N + 1>>;
  }

  // ------------------------------------------
  // genvar

  template <size_t N>
  [[nodiscard]] constexpr auto gen_ty_var(meta_type<taggen<N>>)
  {
    return type_c<ty_var<taggen<N>>>;
  }

  template <size_t N>
  [[nodiscard]] constexpr auto gen_tm_var(meta_type<taggen<N>>)
  {
    return type_c<tm_var<taggen<N>>>;
  }

} // namespace yave