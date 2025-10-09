//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

namespace yave::ui {

  /// window layout constraints
  class window_anchor
  {
  public:
    window_anchor()                     = default;
    window_anchor(const window_anchor&) = default;

    /// align center
    bool is_center() const
    {
      return m_center;
    }

    bool is_high() const
    {
      return m_high;
    }

    bool is_low()
    {
      return m_low;
    }

    void set_center(bool b)
    {
      m_center = b;
      m_low = m_high = false;
    }

    void set_high(bool b)
    {
      m_high   = b;
      m_center = false;
    }

    void set_low(bool b)
    {
      m_low    = b;
      m_center = false;
    }

  private:
    bool m_high   = false;
    bool m_low    = false;
    bool m_center = false;
  };
}