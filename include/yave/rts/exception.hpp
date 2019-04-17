//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/box.hpp>
#include <yave/rts/string.hpp>
#include <yave/rts/object_cast.hpp>

#include <exception>

namespace yave {

  /// \brief Exception value
  ///
  /// Exception object holds a pointer to arbitary error value.
  /// This is useful to detect exception object with single type comparison.
  struct exception_object_value
  {
    template <class T>
    exception_object_value(object_ptr<const String> msg, object_ptr<T> err)
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
  [[nodiscard]] object_ptr<T> add_exception_tag(object_ptr<T> e)
  {
    _get_storage(e).set_pointer_tag(
      object_ptr_storage::pointer_tags::exception);
    return e;
  }

  [[nodiscard]] inline bool
    has_exception_tag(const object_ptr<const Object>& obj)
  {
    return _get_storage(obj).is_exception();
  }

  [[nodiscard]] inline object_ptr<const Exception>
    get_tagged_exception(const object_ptr<const Object>& obj)
  {
    assert(has_exception_tag(obj));
    return static_object_cast<const Exception>(obj);
  }

  // ------------------------------------------
  // conversion

  [[nodiscard]] inline object_ptr<Exception>
    to_Exception(const std::exception& e)
  {
    return make_object<Exception>(e.what(), object_ptr(nullptr));
  }

} // namespace yave

// Exception
YAVE_DECL_TYPE(yave::Exception, _Exception);