//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/module/module.hpp>
#include <yave/module/module_loader.hpp>
#include <yave/node/core/node_declaration_store.hpp>
#include <yave/node/core/node_definition_store.hpp>
#include <yave/node/core/structured_node_graph.hpp>
#include <yave/lib/scene/scene_config.hpp>

#include <string>
#include <filesystem>

namespace yave::editor {

  /// editor data
  struct editor_data
  {
  public: /* main */
    /// name
    std::string name;
    /// file path
    std::filesystem::path path;

  public: /* node */
    /// scene config
    yave::scene_config scene_config;
    /// module loader
    std::unique_ptr<yave::module_loader> module_loader;
    /// node decl list
    node_declaration_store node_decls;
    /// node def list
    node_definition_store node_defs;
    /// node graph
    structured_node_graph node_graph;
  };
} // namespace yave::editor