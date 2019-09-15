//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/box.hpp>
#include <yave/rts/object_cast.hpp>

#include <yave/obj/string/string.hpp>

#include <exception>

namespace yave {

  /// \brief Exception value
  ///
  /// Exception object holds a pointer to arbitary error value.
  /// This is useful to detect exception object with single type comparison.
  struct exception_object_value
  {
    template <class T>
    exception_object_value(
      object_ptr<const String> msg,
      object_ptr<T> err) noexcept
      : message {std::move(msg)}
      , error_value {std::move(err)}
    {
    }

    template <class T>
    exception_object_value(const char* msg, object_ptr<T> err)
      : exception_object_value(make_object<String>(msg), std::move(err))
    {
    }

    /// message
    object_ptr<const String> message;
    /// pointer to error value
    object_ptr<const Object> error_value;
  };

  /// Exception
  using Exception = Box<exception_object_value>;

  // ------------------------------------------
  // helper

  template <class T>
  [[nodiscard]] inline auto add_exception_tag(object_ptr<T> e) noexcept
    -> object_ptr<T>
  {
    _get_storage(e).set_pointer_tag(
      object_ptr_storage::pointer_tags::exception);
    return e;
  }

  [[nodiscard]] inline bool
    has_exception_tag(const object_ptr<const Object>& obj) noexcept
  {
    return _get_storage(obj).is_exception();
  }

  [[nodiscard]] inline auto
    get_tagged_exception(const object_ptr<const Object>& obj) noexcept
    -> object_ptr<const Exception>
  {
    assert(has_exception_tag(obj));
    return static_object_cast<const Exception>(obj);
  }

  // ------------------------------------------
  // conversion

  [[nodiscard]] inline auto to_Exception(const std::exception& e)
    -> object_ptr<Exception>
  {
    return make_object<Exception>(e.what(), object_ptr(nullptr));
  }

} // namespace yave

// Exception
YAVE_DECL_TYPE(yave::Exception, "1a30465c-ee14-473e-bcb9-5ef2462d933f");