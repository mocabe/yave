//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/object_ptr.hpp>
#include <yave/data/lib/frame.hpp>

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
    executable(executable&& exe);
    /// operator=
    executable& operator=(const executable& exe);
    /// operator=
    executable& operator=(executable&& exe);

    /// Get type.
    object_ptr<const Type> type() const;

    /// Execute.
    object_ptr<const Object> execute(frame frame);

  private:
    object_ptr<const Object> m_obj;
    object_ptr<const Type> m_type;
  };
} // namespace yave