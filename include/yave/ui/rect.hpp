//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/ui/vec.hpp>
#include <yave/ui/size.hpp>

#include <algorithm>
#include <cassert>

namespace yave::ui {

  /// 2D rect
  class rect
  {
    ui::vec m_pos;
    ui::size m_size;

    void verify()
    {
      if (m_size.w < 0 || m_size.h < 0) {
        assert(false);
        m_size = {};
      }
    }

  public:
    rect()            = default;
    rect(const rect&) = default;
    rect& operator=(const rect&) = default;

    rect(ui::vec pos, ui::size size)
      : m_pos {pos}
      , m_size {size}
    {
      verify();
    }

    rect(ui::vec p1, ui::vec p2)
      : m_pos {p1}
      , m_size {p2 - p1}
    {
      verify();
    }

    rect(ui::size s)
      : m_pos {}
      , m_size {s}
    {
      verify();
    }

    auto& pos() const
    {
      return m_pos;
    }

    auto& size() const
    {
      return m_size;
    }

    static auto intersect(const rect& rect1, const rect& rect2)
      -> std::optional<rect>
    {
      auto p1 = rect1.m_pos;
      auto p2 = rect1.m_pos + rect1.m_size.vec();
      auto q1 = rect2.m_pos;
      auto q2 = rect2.m_pos + rect2.m_size.vec();

      auto r1 = vec(std::max(p1.x, q1.x), std::max(p1.y, q1.y));
      auto r2 = vec(std::min(p2.x, q2.x), std::min(p2.y, q2.y));

      if (r1.x <= r2.x && r1.y <= r2.y) {
        return rect(r1, r2);
      }
      return std::nullopt;
    }
  };

} // namespace yave::ui