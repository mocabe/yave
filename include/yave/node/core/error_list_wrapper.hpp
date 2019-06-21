//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/support/error.hpp>

#include <mutex>

namespace yave {

  class error_list_wrapper
  {
  public:
    /// Ctor
    error_list_wrapper();
    /// Get copy of error list.
    [[nodiscard]] error_list get_copy() const;
    /// Set copy of error list.
    void set_copy(const error_list& errors);
    /// Move error list to this wrapper.
    void set_move(error_list&& errors);
    /// Lock
    [[nodiscard]] std::unique_lock<std::mutex> lock() const;

  private:
    /// error list
    error_list m_error_list;

  private:
    /// mutex
    mutable std::mutex m_mtx;
  };
} // namespace yave