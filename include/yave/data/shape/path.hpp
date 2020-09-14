//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/config/config.hpp>
#include <yave/data/vector/vector.hpp>
#include <yave/data/vec/vec.hpp>

#include <yave/lib/shape/path.hpp>

namespace yave::data {

  using path_cmd = yave::path_cmd;
  using path_point = vec2;

  /// path data.
  class path
  {
    /// points
    vector<path_point> m_points;
    /// commands
    vector<path_cmd> m_commands;

  public:
    path(const yave::path& p)
      : m_points {p.points()}
      , m_commands {p.commands()}
    {
    }

    path(yave::path&& p)
      : m_points {std::move(p).points()}
      , m_commands {std::move(p).commands()}
    {
    }

    path(const path&)     = default;
    path(path&&) noexcept = default;

    /// access points
    auto& points() const
    {
      return m_points;
    }

    /// access commands
    auto& commands() const
    {
      return m_commands;
    }

    /// convert to path
    operator yave::path() const
    {
      return {m_points, m_commands};
    }
  };

} // namespace yave::data