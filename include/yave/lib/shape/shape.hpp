//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/lib/shape/path.hpp>

namespace yave {

  /// path shape
  class shape
  {
    /// list of paths in composition order
    std::vector<path> m_paths = {};
    /// transform
    glm::fmat3 m_transform = glm::fmat3(1.f);

  public:
    shape()                 = default;
    shape(const shape&)     = default;
    shape(shape&&) noexcept = default;
    shape& operator=(const shape&) = default;
    shape& operator=(shape&&) noexcept = default;

  public:
    /// construct shape from path list
    shape(std::vector<path> paths);

  public:
    auto& paths() const
    {
      return m_paths;
    }

    auto& paths()
    {
      return m_paths;
    }

    auto& transform() const
    {
      return m_transform;
    }

  public:
    [[nodiscard]] bool empty() const
    {
      return m_paths.empty();
    }

    [[nodiscard]] auto size() const -> size_t
    {
      return m_paths.size();
    }

  public:
    /// Add new path
    void add(path p);
    /// Apply current transform to all paths
    void apply_transform();

  public:
    /// transform
    /// \requires `mat` is valid transformation matrix
    void transform(const glm::fmat3& mat);
    /// translate
    void translate(float x, float y);
    /// rotate around some point
    void rotate(float degree, const glm::fvec2& center = {0, 0});
    /// scale
    void scale(float sx, float sy, const glm::fvec2& center = {0, 0});
  };

  /// Merge two shapes
  [[nodiscard]] auto merge(const shape& s1, const shape& s2) -> shape;

} // namespace yave