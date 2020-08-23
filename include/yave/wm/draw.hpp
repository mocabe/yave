//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/wm/draw_list.hpp>
#include <yave/support/enum_flag.hpp>

#include <span>

namespace yave::wm {

  enum class draw_shape_command
  {
  };

  struct draw_shape
  {
    std::vector<glm::vec2> points;
    std::vector<char> commands;
  };

  /// add line
  void dl_draw_line(
    draw_list& dl,
    const glm::vec2& p1,
    const glm::vec2& p2,
    const float& width,
    const glm::vec4& col,
    const draw_scissor& scissor,
    const draw_tex& tex);

  /// add polyline
  void dl_draw_polyline(
    draw_list& dl,
    const std::span<glm::vec2>& ps,
    const float& width,
    const glm::vec4& col,
    const draw_scissor& scissor,
    const draw_tex& tex);

  /// add closed polyline
  void dl_draw_closed_polyline(
    draw_list& dl,
    const std::span<glm::vec2>& ps,
    const float& width,
    const glm::vec4& col,
    const draw_scissor& scissor,
    const draw_tex& tex);

  /// add convex polygon
  void dl_fil_convex_polygon(
    draw_list& dl,
    const std::span<glm::vec2>& ps,
    const glm::vec4& col,
    const draw_scissor& scissor,
    const draw_tex& tex);

  /// add rect
  /// \param p1 upper left corner
  /// \param p2 lower right corner
  void dl_draw_rect(
    draw_list& dl,
    const glm::vec2& p1,
    const glm::vec2& p2,
    const float& width,
    const glm::vec4& col,
    const draw_scissor& scissor,
    const draw_tex& tex);

  /// add rect
  /// \param p1 upper left corner
  /// \param p2 lower right corner
  void dl_fill_rect(
    draw_list& dl,
    const glm::vec2& p1,
    const glm::vec2& p2,
    const glm::vec4& col,
    const draw_scissor& scissor,
    const draw_tex& tex);

  /// add rounded rect
  /// \param p1 upper left corner
  /// \param p2 lower right corner
  void dl_draw_rounded_rect(
    draw_list& dl,
    const glm::vec2& p1,
    const glm::vec2& p2,
    const float& width,
    const float& radius,
    const glm::vec4& col,
    const draw_scissor& scissor,
    const draw_tex& tex);

  /// add rounded rect
  /// \param p1 upper left corner
  /// \param p2 lower right corner
  void dl_fill_rounded_rect(
    draw_list& dl,
    const glm::vec2& p1,
    const glm::vec2& p2,
    const float& radius,
    const glm::vec4& col,
    const draw_scissor& scissor,
    const draw_tex& tex);

  void dl_draw_circle(
    draw_list& dl,
    const glm::vec2& center,
    const float& radius,
    const float& width,
    const glm::vec4& col,
    const draw_scissor& scissor,
    const draw_tex& tex);

  void dl_fill_circle(
    draw_list& dl,
    const glm::vec2& center,
    const float& radius,
    const glm::vec4& col,
    const draw_scissor& scissor,
    const draw_tex& tex);

} // namespace yave::wm