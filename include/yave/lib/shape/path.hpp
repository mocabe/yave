//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/data/color/color.hpp>
#include <yave/data/vector/vector.hpp>

#include <glm/glm.hpp>

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

  /// Single shape path
  class path
  {
    /// vertex list
    std::vector<glm::fvec2> m_points;
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
    path(std::vector<glm::fvec2> ps, std::vector<path_cmd> cmds);

  public:
    auto& points() const
    {
      return m_points;
    }

    auto& points()
    {
      return m_points;
    }

    auto& commands() const
    {
      return m_commands;
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
    void move(const glm::fvec2& p);
    /// Line
    void line(const glm::fvec2& p);
    /// Quad bezier path
    void quad(const glm::fvec2& cp, const glm::fvec2& p);
    /// Cubic bezier path
    void cubic(
      const glm::fvec2& cp1,
      const glm::fvec2& cp2,
      const glm::fvec2& p);
    /// Close path
    void close();
  };

} // namespace yave