//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/config/config.hpp>
#include <yave/editor/editor_data.hpp>

namespace yave::editor {

  /// Determine actual file location for serialization from possibly relative
  /// path.
  /// Relative path will be converted into absolute path based on program
  /// location.
  [[nodiscard]] auto normalize_serialize_path(const std::filesystem::path&)
    -> std::optional<std::filesystem::path>;

  /// save project to file
  /// \param data editor data to save
  /// \param path path to project file
  [[nodiscard]] bool save(const editor_data&, const std::filesystem::path&);

  /// load project
  /// \param data data to load into
  /// \param path path to project file
  [[nodiscard]] bool load(editor_data&, const std::filesystem::path&);

} // namespace yave::editor