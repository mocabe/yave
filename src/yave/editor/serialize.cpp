//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/editor/serialize.hpp>
#include <yave/node/core/serialize.hpp>
#include <yave/support/log.hpp>

#include <cereal/archives/json.hpp>
#include <boost/dll/runtime_symbol_info.hpp>
#include <boost/filesystem/path.hpp>

#include <sstream>
#include <fstream>
#include <optional>

YAVE_DECL_LOCAL_LOGGER(serialize);

namespace yave::editor {

  namespace {

    auto get_program_location()
    {
      return std::filesystem::path(
        boost::dll::program_location().remove_filename().string());
    }
  } // namespace

  auto normalize_serialize_path(const std::filesystem::path& path)
    -> std::optional<std::filesystem::path>
  {
    auto ret = path.is_relative() ? get_program_location() / path : path;

    std::error_code ec;
    ret = std::filesystem::weakly_canonical(get_program_location() / ret, ec);

    if (!ec)
      return ret;

    return std::nullopt;
  }

  bool save(const editor_data& data, const std::filesystem::path& path)
  {
    if (auto floc = normalize_serialize_path(path)) {

      try {

        auto fs = std::ofstream(*floc);

        if (!fs.is_open()) {
          log_error("Failed to open file: {}", floc->string());
          return false;
        }

        {
          cereal::JSONOutputArchive ar(fs);
          save_user_node_graph(ar, data.root_group(), data.node_graph());
        }

        log_info("Saved file to {}", floc->string());
        return true;

      } catch (...) {
        log_error("Failed to save file");
        return false;
      }
    }
    log_error("Invalid file path {}", path.string());
    return false;
  }

  bool load(editor_data& data, const std::filesystem::path& path)
  {
    if (auto floc = normalize_serialize_path(path)) {

      try {

        auto fs = std::ifstream(*floc);

        if (!fs.is_open()) {
          log_error("Failed to open file: {}", floc->string());
          return false;
        }

        // reomve current node graph
        {
          auto& ng = data.node_graph();
          ng.destroy(data.root_group());
          data.root_group() = {};
        }

        {
          cereal::JSONInputArchive ar(fs);
          load_user_node_graph(ar, data.root_group(), data.node_graph());
        }

        log_info("Loaded file {}", floc->string());
        return true;

      } catch (...) {
        // TODO: find better ways to fallback on failure
        log_error("Failed to save file");
        return false;
      }
    }
    log_error("Invalid file path {}", path.string());
    return false;
  }

} // namespace yave::editor