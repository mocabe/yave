//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/lib/shape/path.hpp>

#include <variant>

namespace yave {

  /// path fill
  struct shape_op_fill
  {
    /// fill color of path
    glm::fvec4 color;
  };

  /// path stroke
  struct shape_op_stroke
  {
    /// stroke color of path
    glm::fvec4 color;
    /// stroke width
    float width = 0.f;
  };

  /// shape operation
  using shape_op = std::variant<shape_op_fill, shape_op_stroke>;

  /// shape command
  struct shape_cmd
  {
    /// index of target path
    size_t path_idx;
    /// shape op
    shape_op op;
  };

  /// path collection
  class shape
  {
    /// list of paths
    std::vector<path> m_paths;
    /// list of shape commands
    std::vector<shape_cmd> m_commands;

  public:
    shape()                 = default;
    shape(const shape&)     = default;
    shape(shape&&) noexcept = default;
    shape& operator=(const shape&) = default;
    shape& operator=(shape&&) noexcept = default;

  public:
    shape(std::vector<path> pths, std::vector<shape_cmd> cmds);

  public:
    auto& paths() const
    {
      return m_paths;
    }

    auto& commands() const
    {
      return m_commands;
    }

    [[nodiscard]] bool empty() const
    {
      return m_paths.empty();
    }

  public:
    /// Add new path
    /// \returns index of inserted path
    auto add_path(path p) -> size_t;

  public:
    /// Fill all paths
    void fill(const glm::fvec4& color);
    /// Fill path
    void fill(size_t idx, const glm::fvec4& color);

    /// Stroke all paths
    void stroke(const glm::fvec4& color, float thickness);
    /// Stroke path
    void stroke(size_t idx, const glm::fvec4& color, float thickness);

  public:
    /// transform all paths
    /// \requires `mat` is valid transformation matrix
    void transform(const glm::fmat3& mat);
    /// translate all paths
    void translate(float x, float y);
    /// rotate all paths around some point
    void rotate(float degree, const glm::fvec2& center = {0, 0});
    /// scale all paths
    void scale(float sx, float sy, const glm::fvec2& center = {0, 0});

  public:
    /// merge other shape
    void merge(const shape& other);
  };

} // namespace yave