//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/config/config.hpp>

#include <glm/glm.hpp>
#include <vector>
#include <span>

namespace yave {

  /// Path command for each vertex
  enum class path_cmd : uint8_t
  {
    move  = 1, ///< Move
    close = 2, ///< Close path
    line  = 3, ///< Linear CP
    quad  = 4, ///< Quadratic CP
    cubic = 5, ///< Cubic CP
  };

  /// Path point
  using path_point = glm::dvec2;

  /// Single shape path
  class path
  {
    /// vertex list
    std::vector<path_point> m_points;
    /// command list.
    /// points.size() == commands.size()
    std::vector<path_cmd> m_commands;

  public:
    path()                = default;
    path(const path&)     = default;
    path(path&&) noexcept = default;
    path& operator=(const path&) = default;
    path& operator=(path&&) noexcept = default;

  public:
    /// construct path from unchecked points/commands pair
    /// \requires: ps.size() == cmds.size()
    path(std::vector<path_point> ps, std::vector<path_cmd> cmds);
    /// construct from spans
    /// \requires: ps.size() == cmds.size()
    path(std::span<path_point> ps, std::span<path_cmd> cmds);

  public:
    auto& points() const&
    {
      return m_points;
    }

    auto points() &
    {
      return std::span(m_points);
    }

    auto&& points() &&
    {
      return std::move(m_points);
    }

    auto& commands() const&
    {
      return m_commands;
    }

    auto commands() &
    {
      return std::span(m_commands);
    }

    auto&& commands() &&
    {
      return std::move(m_commands);
    }

  public:
    /// Closed?
    [[nodiscard]] bool closed() const;
    /// Empty?
    [[nodiscard]] bool empty() const;
    /// Size
    [[nodiscard]] auto size() const -> size_t;

  public:
    /// Move
    void move(const path_point& p);
    /// Line
    void line(const path_point& p);
    /// Quad bezier path
    void quad(const path_point& cp, const path_point& p);
    /// Cubic bezier path
    void cubic(
      const path_point& cp1,
      const path_point& cp2,
      const path_point& p);
    /// Close path
    void close();
  };

} // namespace yave