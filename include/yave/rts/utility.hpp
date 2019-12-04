//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/object_ptr.hpp>
#include <yave/rts/object_cast.hpp>
#include <yave/rts/type_error.hpp>

namespace yave {

  /// Clone
  /// \effects Call copy constructor of the object from vtable.
  /// \returns `object_ptr<remove_const_t<T>>` pointing new object.
  /// \throws `std::bad_alloc` when `clone` returned nullptr.
  /// \notes Reference count of new object will be set to 1.
  /// \requires not null.
  template <class T>
  [[nodiscard]] auto clone(const object_ptr<T>& obj)
  {
    assert(obj);

    object_ptr tmp = _get_storage(obj).this_info_table()->clone(obj.get());

    if (unlikely(!tmp))
      throw std::bad_alloc();

    using To = std::remove_const_t<T>;

    return static_object_cast<To>(std::move(tmp));
  }

  /// check type at runtime
  template <class T, class U, class = std::enable_if_t<is_object_pointer_v<U>>>
  void check_type_dynamic(U&& obj)
  {
    auto tmp = object_ptr(std::forward<U>(obj));
    auto t1  = object_type<T>();
    auto t2  = type_of(tmp);
    if (unlikely(!same_type(t1, t2)))
      throw type_error::bad_type_check(t1, t2);
  }

  /// check type at compile time
  template <class T, class U, class = std::enable_if_t<is_object_pointer_v<U>>>
  void check_type_static(U&&)
  {
    auto t1 = type_of(get_term<T>(), true_c);
    auto t2 = type_of(get_term<object_pointer_element_t<U>>(), true_c);

    if constexpr (t1 != t2) {
      static_assert(false_v<T>, "Compile time type check failed!");
      using expected = typename decltype(t1)::_show;
      using provided = typename decltype(t2)::_show;
      static_assert(false_v<expected, provided>);
    }
  }

} // namespace yave