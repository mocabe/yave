//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/structured_node_graph.hpp>
#include <yave/lib/scene/scene_config.hpp>

#include <string>
#include <filesystem>

namespace yave::editor {

  /// editor data
  struct editor_data
  {
    /// data name
    std::string name;
    /// file path
    std::filesystem::path path;
    /// node graph
    structured_node_graph node_graph;
    /// scene config
    scene_config scene_config;
  };
}