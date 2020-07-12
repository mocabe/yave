//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/lib/shape/path.hpp>
#include <glm/gtx/matrix_transform_2d.hpp>

namespace yave {

  namespace {

    /// add new path point
    constexpr auto add_path_point = [](auto& ps, auto& cmds, auto p, auto cmd) {
      ps.push_back(p);
      cmds.push_back(cmd);
    };

    /// reserve extra n points
    constexpr auto next_path_points(auto& ps, auto& cmds, size_t n)
    {
      ps.reserve(ps.size() + n);
      cmds.reserve(cmds.size() + n);
    }
  } // namespace

  path::path(std::vector<fvec2> ps, std::vector<path_cmd> cmds)
  {
    assert(ps.size() == cmds.size());
    m_points   = std::move(ps);
    m_commands = std::move(cmds);
  }

  bool path::closed() const
  {
    if (empty())
      return false;

    return m_commands.back() == path_cmd::close;
  }

  bool path::empty() const
  {
    return m_points.empty();
  }

  auto path::size() const -> size_t
  {
    return m_points.size();
  }

  void path::move(const fvec2& p)
  {
    add_path_point(m_points, m_commands, p, path_cmd::move);
  }

  void path::line(const fvec2& p)
  {
    add_path_point(m_points, m_commands, p, path_cmd::line);
  }

  void path::quad(const fvec2& cp, const fvec2& p)
  {
    next_path_points(m_points, m_commands, 2);
    add_path_point(m_points, m_commands, cp, path_cmd::quad);
    add_path_point(m_points, m_commands, p, path_cmd::line);
  }

  void path::cubic(const fvec2& cp1, const fvec2& cp2, const fvec2& p)
  {
    next_path_points(m_points, m_commands, 3);
    add_path_point(m_points, m_commands, cp1, path_cmd::cubic);
    add_path_point(m_points, m_commands, cp2, path_cmd::cubic);
    add_path_point(m_points, m_commands, p, path_cmd::line);
  }

  void path::close()
  {
    if (empty() || closed())
      return;

    add_path_point(m_points, m_commands, fvec2(), path_cmd::close);
  }

} // namespace yave