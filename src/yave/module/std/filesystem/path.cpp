//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/module/std/filesystem/path.hpp>
#include <yave/module/std/primitive/data_holder.hpp>
#include <yave/node/core/function.hpp>
#include <yave/obj/filesystem/path.hpp>
#include <yave/obj/string/string.hpp>

namespace yave {

  auto node_declaration_traits<node::FilePath>::get_node_declaration()
    -> node_declaration
  {
    return node_declaration(
      "FilePath",
      "std/filesystem",
      "File path constructor",
      {"path"},
      {"path"},
      {{0, make_data_type_holder<String>()}});
  }

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

  auto node_definition_traits<node::FilePath, modules::_std::tag>::
    get_node_definitions() -> std::vector<node_definition>
  {
    auto info = get_node_declaration<node::FilePath>();
    return std::vector {node_definition(
      info.name(),
      0,
      make_object<modules::_std::filesystem::FilePathConstructor>(),
      info.description())};
  }
} // namespace yave