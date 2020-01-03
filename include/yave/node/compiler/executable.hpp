//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/object_ptr.hpp>
#include <yave/lib/frame_time/frame_time.hpp>

namespace yave {

  /// Executable wrapper
  class executable
  {
  public:
    /// Ctor
    executable(object_ptr<const Object> obj, object_ptr<const Type> type);
    /// Ctor
    executable(const executable& exe);
    /// Ctor
    executable(executable&& exe) noexcept;
    /// operator=
    executable& operator=(const executable& exe);
    /// operator=
    executable& operator=(executable&& exe) noexcept;

    /// Get type.
    [[nodiscard]] auto type() const -> const object_ptr<const Type>&;

    /// Get object.
    [[nodiscard]] auto object() const -> const object_ptr<const Object>&;

    /// Execute.
    [[nodiscard]] auto execute(frame_time frame) -> object_ptr<const Object>;

  private:
    object_ptr<const Object> m_obj;
    object_ptr<const Type> m_type;
  };
} // namespace yave