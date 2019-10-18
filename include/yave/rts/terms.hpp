//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/meta_type.hpp>
#include <yave/rts/meta_tuple.hpp>
#include <yave/rts/specifiers.hpp>

namespace yave {

  // ------------------------------------------
  // Term

  /// tm_apply
  template <class T1, class T2>
  struct tm_apply
  {
  };

  /// tm_closure
  template <class... Ts>
  struct tm_closure
  {
  };

  /// tm_value
  template <class T>
  struct tm_value
  {
  };

  /// tm_var
  template <class Tag>
  struct tm_var
  {
  };

  /// tm_varvalue
  template <class Tag>
  struct tm_varvalue
  {
  };

  /// tm_list
  template <class T>
  struct tm_list
  {
  };

  // ------------------------------------------
  // Term accessors

  template <class T1, class T2>
  struct meta_type<tm_apply<T1, T2>>
  {
    using type = tm_apply<T1, T2>;
    constexpr auto t1() const
    {
      return type_c<T1>;
    }
    constexpr auto t2() const
    {
      return type_c<T2>;
    }
  };

  template <class Tag>
  struct meta_type<tm_value<Tag>>
  {
    using type = tm_value<Tag>;
    constexpr auto tag() const
    {
      return type_c<Tag>;
    }
  };

  template <class Tag>
  struct meta_type<tm_varvalue<Tag>>
  {
    using type = tm_varvalue<Tag>;
    constexpr auto tag() const
    {
      return type_c<Tag>;
    }
  };

  template <class Tag>
  struct meta_type<tm_var<Tag>>
  {
    using type = tm_var<Tag>;
    constexpr auto tag() const
    {
      return type_c<Tag>;
    }
  };

  template <class... Ts>
  struct meta_type<tm_closure<Ts...>>
  {
    using type = tm_closure<Ts...>;
    constexpr size_t size() const
    {
      return sizeof...(Ts);
    }
  };

  template <class T>
  struct meta_type<tm_list<T>>
  {
    using type = tm_list<T>;
    constexpr size_t t() const
    {
      return type_c<T>;
    }
  };

  // ------------------------------------------
  // has_term

  namespace detail {

    template <class T, class = void>
    struct has_term_impl
    {
      static constexpr auto value = false_c;
    };

    template <class T>
    struct has_term_impl<T, std::void_t<decltype(T::term)>>
    {
      static constexpr auto value = true_c;
    };

  } // namespace detail

  template <class T>
  [[nodiscard]] constexpr auto has_term()
  {
    return detail::has_term_impl<T>::value;
  }

  // ------------------------------------------
  // get_term

  template <class T>
  [[nodiscard]] constexpr auto get_term(meta_type<T> = {})
  {
    if constexpr (has_term<T>()) {
      // normal term
      return T::term;
    } else if constexpr (is_specifier(type_c<T>)) {
      // specifier -> term
      return get_term(get_proxy_type(normalize_specifier(type_c<T>)));
    } else
      static_assert(false_v<T>, "T should have either term or specifier");
  }

  /// Special case for value type Undefined.
  [[nodiscard]] constexpr auto get_term(meta_type<struct Undefined> = {})
  {
    return type_c<tm_value<struct Undefined>>;
  }

  // ------------------------------------------
  // is_tm_apply

  namespace detail {

    template <class T>
    struct is_tm_apply_impl
    {
      static constexpr auto value = false_c;
    };

    template <class T1, class T2>
    struct is_tm_apply_impl<tm_apply<T1, T2>>
    {
      static constexpr auto value = true_c;
    };

  } // namespace detail

  /// is_apply_v
  template <class T>
  [[nodiscard]] constexpr auto is_tm_apply(meta_type<T>)
  {
    return detail::is_tm_apply_impl<T>::value;
  }

  /// has_apply_v
  template <class T>
  [[nodiscard]] constexpr auto has_tm_apply()
  {
    return is_tm_apply(get_term<T>());
  }

  // ------------------------------------------
  // is_tm_value

  namespace detail {

    template <class T>
    struct is_tm_value_impl
    {
      static constexpr auto value = false_c;
    };

    template <class Tag>
    struct is_tm_value_impl<tm_value<Tag>>
    {
      static constexpr auto value = true_c;
    };

  } // namespace detail

  /// is_value_v
  template <class T>
  [[nodiscard]] constexpr auto is_tm_value(meta_type<T>)
  {
    return detail::is_tm_value_impl<T>::value;
  }

  /// has_value_v
  template <class T>
  [[nodiscard]] constexpr auto has_tm_value()
  {
    return is_tm_value(get_term<T>());
  }

  // ------------------------------------------
  // is_tm_closure

  namespace detail {

    template <class T>
    struct is_tm_closure_impl
    {
      static constexpr auto value = false_c;
    };

    template <class... Ts>
    struct is_tm_closure_impl<tm_closure<Ts...>>
    {
      static constexpr auto value = true_c;
    };

  } // namespace detail

  /// is_closure_v
  template <class T>
  [[nodiscard]] constexpr auto is_tm_closure(meta_type<T>)
  {
    return detail::is_tm_closure_impl<T>::value;
  }

  /// has_closure_v
  template <class T>
  [[nodiscard]] constexpr auto has_tm_closure()
  {
    return is_tm_closure(get_term<T>());
  }

  // ------------------------------------------
  // is_tm_var

  namespace detail {

    template <class T>
    struct is_tm_var_impl
    {
      static constexpr auto value = false_c;
    };

    template <class Tag>
    struct is_tm_var_impl<tm_var<Tag>>
    {
      static constexpr auto value = true_c;
    };

  } // namespace detail

  /// is_var_v
  template <class T>
  [[nodiscard]] constexpr auto is_tm_var(meta_type<T>)
  {
    return detail::is_tm_var_impl<T>::value;
  }

  /// has_var_v
  template <class T>
  [[nodiscard]] constexpr auto has_tm_var()
  {
    return is_tm_var(get_term<T>());
  }

  // ------------------------------------------
  // is_tm_varvalue

  namespace detail {

    template <class T>
    struct is_tm_varvalue_impl
    {
      static constexpr auto value = false_c;
    };

    template <class Tag>
    struct is_tm_varvalue_impl<tm_varvalue<Tag>>
    {
      static constexpr auto value = true_c;
    };

  } // namespace detail

  /// is_tm_varvalue
  template <class T>
  [[nodiscard]] constexpr auto is_tm_varvalue(meta_type<T>)
  {
    return detail::is_tm_varvalue_impl<T>::value;
  }

  /// has_tm_varvalue_v
  template <class T>
  [[nodiscard]] constexpr auto has_tm_varvalue()
  {
    return is_tm_varvalue(get_term<T>());
  }

  // ------------------------------------------
  // is_tm_list

  namespace detail {

    template <class T>
    struct is_tm_list_impl
    {
      static constexpr auto value = false_c;
    };

    template <class T>
    struct is_tm_list_impl<tm_list<T>>
    {
      static constexpr auto value = true_c;
    };
  }

  /// is_tm_list
  template <class T>
  [[nodiscard]] constexpr auto is_tm_list(meta_type<T>)
  {
    return detail::is_tm_list_impl<T>::value;
  }

  /// has_tm_list
  template <class T>
  [[nodiscard]] constexpr auto has_tm_list()
  {
    return is_tm_list(get_term<T>());
  }

  // ------------------------------------------
  // to_tuple

  template <class... Ts>
  [[nodiscard]] constexpr auto to_tuple(meta_type<tm_closure<Ts...>>)
  {
    return tuple_c<Ts...>;
  }

  // ------------------------------------------
  // to_tm_closure

  template <class... Ts>
  [[nodiscard]] constexpr auto to_tm_closure(meta_tuple<Ts...>)
  {
    return type_c<tm_closure<Ts...>>;
  }

  // ------------------------------------------
  // make_tm_closure

  template <class... Terms>
  [[nodiscard]] constexpr auto make_tm_closure(meta_type<Terms>...)
  {
    return type_c<tm_closure<Terms...>>;
  }

  // ------------------------------------------
  // make_tm_apply

  template <class T1, class T2>
  [[nodiscard]] constexpr auto make_tm_apply(meta_type<T1>, meta_type<T2>)
  {
    return type_c<tm_apply<T1, T2>>;
  }

  // ------------------------------------------
  // make_tm_var

  template <class Tag>
  [[nodiscard]] constexpr auto make_tm_var(meta_type<Tag>)
  {
    return type_c<tm_var<Tag>>;
  }

  // ------------------------------------------
  // make_tm_list

  template <class T>
  [[nodiscard]] constexpr auto make_tm_list(meta_type<T>)
  {
    return type_c<tm_list<T>>;
  }

  // ------------------------------------------
  // head

  template <class T, class... Ts>
  [[nodiscard]] constexpr auto head(meta_type<tm_closure<T, Ts...>>)
  {
    return type_c<T>;
  }

  // ------------------------------------------
  // tail

  template <class T, class... Ts>
  [[nodiscard]] constexpr auto tail(meta_type<tm_closure<T, Ts...>>)
  {
    return type_c<tm_closure<Ts...>>;
  }

  [[nodiscard]] constexpr auto tail(meta_type<tm_closure<>> term)
  {
    return term;
  }

  // ------------------------------------------
  // closure_term_export

  namespace detail {

    template <class Term, class Target>
    constexpr auto closure_term_export_impl(
      meta_type<Term> term,
      meta_type<Target> target)
    {
      if constexpr (is_tm_varvalue(term)) {
        return subst_term(term, make_tm_var(term.tag()), target);
      } else if constexpr (is_tm_closure(term)) {
        if constexpr (term.size() == 1) {
          return closure_term_export_impl(head(term), target);
        } else {
          return closure_term_export_impl(
            tail(term), closure_term_export_impl(head(term), target));
        }
      } else if constexpr (is_tm_apply(term)) {
        return closure_term_export_impl(
          term.t2(), closure_term_export_impl(term.t1(), target));
      } else
        return target;
    }

  } // namespace detail

  /// convert varvalue to var
  template <class Term>
  [[nodiscard]] constexpr auto closure_term_export(meta_type<Term> term)
  {
    return detail::closure_term_export_impl(term, term);
  }

  [[nodiscard]] constexpr auto closure_term_export(meta_type<tm_closure<>> c)
  {
    return c;
  }

} // namespace yave