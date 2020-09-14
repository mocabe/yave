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
    vector<object_ptr<Path>> m_paths;
    /// command list
    vector<object_ptr<ShapeCmd>> m_commands;

  public:
    shape(const yave::shape& s)
    {
      // path
      {
        auto tmp = std::vector<object_ptr<Path>>();
        tmp.reserve(s.paths().size());

        for (auto&& p : s.paths())
          tmp.push_back(make_object<Path>(p));

        m_paths = std::move(tmp);
      }

      // cmd
      {
        auto tmp = std::vector<object_ptr<ShapeCmd>>();
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
  };
} // namespace yave::data
