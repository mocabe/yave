//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/module/std/decl/filesystem/path.hpp>
#include <yave/node/core/function.hpp>
#include <yave/obj/filesystem/path.hpp>
#include <yave/obj/string/string.hpp>

namespace yave {

  namespace modules::_std::filesystem {
    struct FilePathConstructor
      : NodeFunction<FilePathConstructor, String, FilesystemPath>
    {
      return_type code() const
      {
        auto str = eval_arg<0>();
        return make_object<FilesystemPath>(str->c_str());
      }
    };
  } // namespace modules::_std::filesystem

  template <>
  struct node_definition_traits<node::FilePath, modules::_std::tag>
  {
    static auto get_node_definitions()
    {
      auto info = get_node_declaration<node::FilePath>();
      return std::vector {node_definition(
        info.name(),
        0,
        make_object<modules::_std::filesystem::FilePathConstructor>(),
        info.description())};
    }
  };
}