//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/object_ptr.hpp>
#include <yave/data/frame_demand/frame_demand.hpp>

namespace yave {

  /// Executable wrapper
  class executable
  {
  public:
    /// Ctor
    executable() = default;
    /// Ctor
    executable(object_ptr<const Object> obj, object_ptr<const Type> type);
    /// Ctor
    executable(const executable& other) = delete;
    /// Ctor
    executable(executable&& other) noexcept;
    /// operator=
    executable& operator=(const executable& other) = delete;
    /// operator=
    executable& operator=(executable&& other) noexcept;

    /// Get object.
    [[nodiscard]] auto object() const -> const object_ptr<const Object>&;

    /// Get type.
    [[nodiscard]] auto type() const -> const object_ptr<const Type>&;

    /// Execute.
    [[nodiscard]] auto execute(const time& time) -> object_ptr<const Object>;

    /// Clone.
    [[nodiscard]] auto clone() const -> executable;

  private:
    object_ptr<const Object> m_obj;
    object_ptr<const Type> m_type;
  };
} // namespace yave