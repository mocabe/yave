//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/exception.hpp>

namespace yave {

  /// bad_value_cast exception
  class bad_value_cast : public std::logic_error
  {
  public:
    explicit bad_value_cast(
      object_ptr<const Type> from,
      object_ptr<const Type> to)
      : std::logic_error("bad_value_cast")
      , m_from {std::move(from)}
      , m_to {std::move(to)}
    {
    }

    /// get from
    [[nodiscard]] auto from() const -> const object_ptr<const Type>&
    {
      return m_from;
    }

    /// get to
    [[nodiscard]] auto to() const -> const object_ptr<const Type>&
    {
      return m_to;
    }

  private:
    /// cast from
    object_ptr<const Type> m_from;
    /// cast to
    object_ptr<const Type> m_to;
  };

  struct bad_value_object_value
  {
    /// cast from
    object_ptr<const Type> from;
    /// cast to
    object_ptr<const Type> to;
  };

  /// Exception object for bad_value_cast exception
  using BadValueCast = Box<bad_value_object_value>;

  // ------------------------------------------
  // conversion

  [[nodiscard]] inline object_ptr<Exception> to_Exception(
    const bad_value_cast& e)
  {
    return make_object<Exception>(
      e.what(), make_object<BadValueCast>(e.from(), e.to()));
  }

} // namespace yave

// BadValueCast
YAVE_DECL_TYPE(yave::BadValueCast, "2845d0fa-fed4-40b2-be84-bb014c4e8446");