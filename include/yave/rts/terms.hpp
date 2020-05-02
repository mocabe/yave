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
    constexpr auto t() const
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
    if constexpr (has_term<T>())
      // normal term
      return T::term;
    else if constexpr (std::is_same_v<T, struct Undefined>)
      // case for Undefined
      return type_c<tm_value<struct Undefined>>;
    else
      // specifier -> term
      return get_term(get_proxy_type(normalize_specifier(type_c<T>)));
  }

  // ------------------------------------------
  // is_tm_apply

  template <class T1, class T2>
  [[nodiscard]] constexpr auto is_tm_apply(meta_type<tm_apply<T1, T2>>)
  {
    return true_c;
  }

  template <class T>
  [[nodiscard]] constexpr auto is_tm_apply(meta_type<T>)
  {
    return false_c;
  }

  template <class T>
  [[nodiscard]] constexpr auto has_tm_apply()
  {
    return is_tm_apply(get_term<T>());
  }

  // ------------------------------------------
  // is_tm_value

  template <class Tag>
  [[nodiscard]] constexpr auto is_tm_value(meta_type<tm_value<Tag>>)
  {
    return true_c;
  }

  template <class T>
  [[nodiscard]] constexpr auto is_tm_value(meta_type<T>)
  {
    return false_c;
  }

  template <class T>
  [[nodiscard]] constexpr auto has_tm_value()
  {
    return is_tm_value(get_term<T>());
  }

  // ------------------------------------------
  // is_tm_closure

  template <class... Ts>
  [[nodiscard]] constexpr auto is_tm_closure(meta_type<tm_closure<Ts...>>)
  {
    return true_c;
  }

  template <class T>
  [[nodiscard]] constexpr auto is_tm_closure(meta_type<T>)
  {
    return false_c;
  }

  template <class T>
  [[nodiscard]] constexpr auto has_tm_closure()
  {
    return is_tm_closure(get_term<T>());
  }

  // ------------------------------------------
  // is_tm_var

  template <class Tag>
  [[nodiscard]] constexpr auto is_tm_var(meta_type<tm_var<Tag>>)
  {
    return true_c;
  }

  template <class T>
  [[nodiscard]] constexpr auto is_tm_var(meta_type<T>)
  {
    return false_c;
  }

  template <class T>
  [[nodiscard]] constexpr auto has_tm_var()
  {
    return is_tm_var(get_term<T>());
  }

  // ------------------------------------------
  // is_tm_varvalue

  template <class Tag>
  [[nodiscard]] constexpr auto is_tm_varvalue(meta_type<tm_varvalue<Tag>>)
  {
    return true_c;
  }

  template <class T>
  [[nodiscard]] constexpr auto is_tm_varvalue(meta_type<T>)
  {
    return false_c;
  }

  template <class T>
  [[nodiscard]] constexpr auto has_tm_varvalue()
  {
    return is_tm_varvalue(get_term<T>());
  }

  // ------------------------------------------
  // is_tm_list

  template <class T>
  [[nodiscard]] constexpr auto is_tm_list(meta_type<tm_list<T>>)
  {
    return true_c;
  }

  template <class T>
  [[nodiscard]] constexpr auto is_tm_list(meta_type<T>)
  {
    return false_c;
  }

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
  // polymorphic_term_export

  namespace detail {

    template <class Term, class Target>
    constexpr auto polymorphic_term_export_impl(
      meta_type<Term> term,
      meta_type<Target> target)
    {
      if constexpr (is_tm_varvalue(term)) {
        return subst_term(term, make_tm_var(term.tag()), target);
      } else if constexpr (is_tm_closure(term)) {
        if constexpr (term.size() == 0) {
          return target;
        } else if constexpr (term.size() == 1) {
          return polymorphic_term_export_impl(head(term), target);
        } else {
          return polymorphic_term_export_impl(
            tail(term), polymorphic_term_export_impl(head(term), target));
        }
      } else if constexpr (is_tm_apply(term)) {
        return polymorphic_term_export_impl(
          term.t2(), polymorphic_term_export_impl(term.t1(), target));
      } else if constexpr (is_tm_list(term)) {
        return polymorphic_term_export_impl(term.t(), target);
      } else
        return target;
    }

  } // namespace detail

  /// convert varvalue to var
  template <class Term>
  [[nodiscard]] constexpr auto polymorphic_term_export(meta_type<Term> term)
  {
    return detail::polymorphic_term_export_impl(term, term);
  }

} // namespace yave