//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/data/shape/path.hpp>
#include <yave/lib/shape/shape.hpp>
#include <yave/obj/shape/path.hpp>
#include <yave/obj/shape/shape_cmd.hpp>

namespace yave::data {

  /// shape data
  class shape
  {
    /// path list
    vector<object_ptr<const Path>> m_paths;
    /// command list
    vector<object_ptr<const ShapeCmd>> m_commands;

  public:
    shape(const yave::shape& s)
    {
      // path
      {
        auto tmp = std::vector<object_ptr<const Path>>();
        tmp.reserve(s.paths().size());

        for (auto&& p : s.paths())
          tmp.push_back(make_object<Path>(p));

        m_paths = std::move(tmp);
      }

      // cmd
      {
        auto tmp = std::vector<object_ptr<const ShapeCmd>>();
        tmp.reserve(s.commands().size());

        for (auto&& c : s.commands())
          tmp.push_back(make_object<ShapeCmd>(c));

        m_commands = std::move(tmp);
      }
    }

    operator yave::shape() const
    {
      auto pths = std::vector<yave::path>();
      {
        pths.reserve(m_paths.size());

        for (auto&& p : m_paths) {
          pths.push_back(yave::path(*p));
        }
      }

      auto cmds = std::vector<yave::shape_cmd>();
      {
        cmds.reserve(m_commands.size());

        for (auto&& c : m_commands) {
          cmds.push_back(yave::shape_cmd(*c));
        }
      }

      return {std::move(pths), std::move(cmds)};
    }

  public:
    // should match yave::shape::merge.
    void merge(const data::shape& other)
    {
      {
        auto ps = std::vector<object_ptr<const Path>>();
        ps.reserve(m_paths.size() + other.m_paths.size());
        ps.insert(ps.end(), m_paths.begin(), m_paths.end());
        ps.insert(ps.end(), other.m_paths.begin(), other.m_paths.end());
        m_paths = ps;
      }
      {
        auto cs = std::vector<object_ptr<const ShapeCmd>>();
        cs.reserve(m_commands.size() + other.m_commands.size());
        cs.insert(cs.end(), m_commands.begin(), m_commands.end());
        auto base = cs.size();
        for (auto&& c : other.m_commands) {
          auto tmp = c.clone();
          tmp->path_idx += base;
          cs.push_back(std::move(tmp));
        }
        m_commands = cs;
      }
    }
  };
} // namespace yave::data
