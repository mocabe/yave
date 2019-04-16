//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/value_cast.hpp>
#include <yave/rts/function.hpp>
#include <yave/rts/eval_error.hpp>
#include <yave/rts/result_error.hpp>

namespace yave {

  /// copy apply graph
  [[nodiscard]] object_ptr<const Object>
    copy_apply_graph(const object_ptr<const Object>& obj);

  namespace detail {

    /// eval implementation
    [[nodiscard]] object_ptr<const Object>
      eval_obj(const object_ptr<const Object>& obj);

  } // namespace detail

  /// evaluate each apply node and replace with result
  template <class T>
  [[nodiscard]] auto eval(object_ptr<T> obj)
  {
    auto result = detail::eval_obj(std::move(obj));
    assert(result);

    // for gcc 7
    constexpr auto type = type_of(get_term<T>(), false_c);

    // run compile time type check
    if constexpr (!is_error_type(type)) {
      // Currently object_ptr<T> MUST have type T which has compatible memory
      // layout with actual object pointing to.
      // Since it's impossible to decide memory layout of closure types,
      // we convert it to closure<...> which is essentially equal to to
      // Object. Type variables are also undecidable so we just convert
      // them to Object.
      using To =
        std::add_const_t<typename decltype(guess_object_type(type))::type>;
      // cast to resutn type
      return static_object_cast<To>(result);
    } else {
      // fallback to object_ptr<>
      return result;
    }
  }

} // namespace yave