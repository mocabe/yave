//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/ui/size.hpp>

#include <limits>
#include <cassert>

namespace yave::ui {

  /// Box layout constraints
  struct box_constraints
  {
  private:
    static constexpr auto inf = std::numeric_limits<f64>::infinity();

    /// minimum size required for window
    ui::size m_min = {0.0, 0.0};
    /// maximum size avalable for window
    ui::size m_max = {inf, inf};

    void validate() const
    {
      assert(0.0 <= m_min.w && m_min.w <= m_max.w);
      assert(0.0 <= m_min.h && m_min.h <= m_max.h);
      assert(m_min.w != inf && m_min.h != inf);
    }

  public:
    box_constraints() = default;

    box_constraints(ui::size sz)
      : m_min {sz}
      , m_max {sz}
    {
      validate();
    }

    box_constraints(ui::size min, ui::size max)
      : m_min {min}
      , m_max {max}
    {
      validate();
    }

    auto& min() const
    {
      return m_min;
    }

    auto& max() const
    {
      return m_max;
    }

    bool is_w_bounded() const
    {
      return m_max.w != inf;
    }

    bool is_h_bounded() const
    {
      return m_max.h != inf;
    }

    bool is_bounded() const
    {
      return is_w_bounded() && is_h_bounded();
    }

    bool is_w_tight() const
    {
      return m_min.w == m_max.h;
    }

    bool is_h_tight() const
    {
      return m_min.h == m_max.h;
    }

    bool is_tight() const
    {
      return is_w_tight() && is_h_tight();
    }
  };
}