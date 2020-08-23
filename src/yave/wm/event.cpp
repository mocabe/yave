//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/wm/event.hpp>

namespace yave::wm {

  event::event() noexcept
  {
  }

  event::~event() noexcept
  {
  }

  bool event::accepted() const
  {
    return m_status == 1;
  }

  bool event::ignored() const
  {
    return m_status == -1;
  }

  void event::accept()
  {
    m_status = 1;
  }

  void event::ignore()
  {
    m_status = -1;
  }

} // namespace yave::wm
