//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/lib/project/project_config.hpp>
#include <yave/module/module.hpp>
#include <yave/module/module_loader.hpp>
#include <yave/node/core/node_declaration_store.hpp>
#include <yave/node/core/node_definition_store.hpp>
#include <yave/node/core/managed_node_graph.hpp>

#include <memory>
#include <filesystem>
#include <optional>

namespace yave::app {

  /// Data type to store all static configurations for a project.
  struct project_config
  {
    /// Project name
    std::string name;
    /// List of modules
    std::vector<std::string> modules;
    /// Project file path (if exists)
    std::filesystem::path path;
    /// Project scene config
    scene_config scene_cfg;
  };

  /// Data type to store all on-memory data for a project
  struct project_data
  {
    /// list of module backends
    std::vector<std::shared_ptr<module>> modules;
    /// set of all declarations (from modules)
    node_declaration_store decls;
    /// set of all definitions (from modules)
    node_definition_store defs;
    /// node graph
    managed_node_graph graph;
  };

  /// On memory project data holder.
  /// Stores both project configuration and live data.
  class project
  {
  public:
    /// Create new empty project.
    /// \param config initial scene config
    /// \param loader module loader to use
    project(const project_config& config, module_loader& loader);

    /// Dtor
    ~project() noexcept;

    project(const project&) = delete;
    project& operator=(const project&) = delete;

  public:
    /// Get name of this project
    auto get_name() const -> std::string;
    /// Change name of this project
    void set_name(const std::string& new_name);

  public:
    /// Get current scene config
    auto get_scene_config() const -> scene_config;
    /// Change scene config
    void set_scene_config(const scene_config& new_config);

  public:
    /// Has file backing this project?
    bool has_path() const;
    /// Get project path
    auto get_path() const -> std::optional<std::filesystem::path>;
    /// Change project path
    void set_path(const std::filesystem::path& new_path);

  public:
    /// Access decls
    auto node_declarations() const -> const node_declaration_store&;
    /// Access defs
    auto node_definitions() const -> const node_definition_store&;
    /// Access modules names
    auto modules() const -> const std::vector<std::string>&;
    /// Access node graph
    auto graph() -> managed_node_graph&;
    /// Access node graph
    auto graph() const -> const managed_node_graph&;

  private:
    class impl;
    std::unique_ptr<impl> m_pimpl;
  };
} // namespace yave