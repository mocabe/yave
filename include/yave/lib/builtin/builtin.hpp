//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/lib/primitive/primitive.hpp>
#include <yave/lib/keyframe/keyframe.hpp>

namespace yave {

  /// builtin type
  using builtin_t = std::variant<primitive_t>;

  /// Container of builtin_t for multi-thread access.
  class builtin_container
  {
  public:
    /// Constructor
    builtin_container()
      : m_builtin {}
      , m_mtx {}
    {
    }

    /// Constructor
    builtin_container(const builtin_t& builtin)
      : m_builtin {builtin}
      , m_mtx {}
    {
    }

    /// Copy constructor
    builtin_container(const builtin_container& other)
      : m_builtin {other.m_builtin}
      , m_mtx {}
    {
    }

    /// Set builtin_t value
    void set(const builtin_t& builtin)
    {
      std::lock_guard lck {m_mtx};
      m_builtin = builtin;
    }

    /// Get primitive_t value
    [[nodiscard]] builtin_t get() const
    {
      std::lock_guard lck {m_mtx};
      return m_builtin;
    }

  private:
    builtin_t m_builtin;
    mutable atomic_spinlock<uint8_t> m_mtx;
  };
}