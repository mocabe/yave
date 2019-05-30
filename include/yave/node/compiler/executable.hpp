//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/core/rts/object_ptr.hpp>
#include <yave/core/data_types/frame.hpp>

namespace yave {

  /// Executable wrapper
  class Executable
  {
  public:
    /// Ctor
    Executable(object_ptr<const Object> obj, object_ptr<const Type> type);
    /// Ctor
    Executable(const Executable& exe);
    /// Ctor
    Executable(Executable&& exe);
    /// operator=
    Executable& operator=(const Executable& exe);
    /// operator=
    Executable& operator=(Executable&& exe);

    /// Get type.
    object_ptr<const Type> type() const;

    /// Execute.
    object_ptr<const Object> execute(frame frame) const;

  private:
    object_ptr<const Object> m_obj;
    object_ptr<const Type> m_type;
  };
} // namespace yave