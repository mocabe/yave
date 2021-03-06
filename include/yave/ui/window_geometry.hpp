//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/ui/vec.hpp>
#include <yave/ui/size.hpp>

#include <cassert>

namespace yave::ui {

  /// Window layout geometry
  class window_geometry
  {
    /// window offset
    ui::vec m_offset;
    /// window size
    ui::size m_size;

    void validate()
    {
      assert(0.0 <= m_offset.x && 0.0 <= m_offset.y);
      assert(0.0 <= m_size.w && 0.0 <= m_size.h);
    }

  public:
    window_geometry() = default;

    window_geometry(ui::vec off, ui::size sz)
      : m_offset {off}
      , m_size {sz}
    {
      validate();
    }

    auto& offset() const
    {
      return m_offset;
    }

    auto& size() const
    {
      return m_size;
    }

    void set_size(ui::size s)
    {
      m_size = s;
      validate();
    }

    void set_offset(ui::vec o)
    {
      m_offset = o;
      validate();
    }
  };

} // namespace yave::ui