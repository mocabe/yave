//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/interface/error_list_wrapper.hpp>

namespace yave {

  error_list_wrapper::error_list_wrapper()
  {
  }

  error_list error_list_wrapper::get_copy() const
  {
    error_list ret;
    for (auto&& e : m_error_list) {
      ret.push_back(e.clone());
    }
    return ret;
  }

  void error_list_wrapper::set_copy(const error_list& errors)
  {
    m_error_list.clear();
    for (auto&& e : errors) {
      m_error_list.push_back(e.clone());
    }
  }

  void error_list_wrapper::set_move(error_list&& errors)
  {
    m_error_list = std::move(errors);
  }

  std::unique_lock<std::mutex> error_list_wrapper::lock() const
  {
    return std::unique_lock(m_mtx);
  }
} // namespace yave