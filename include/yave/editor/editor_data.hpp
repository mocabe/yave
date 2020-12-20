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

#include <yave/editor/compile_thread.hpp>
#include <yave/editor/execute_thread.hpp>
#include <yave/editor/update_channel.hpp>

#include <string>
#include <filesystem>
#include <any>

namespace yave::editor {

  /// editor data
  struct editor_data
  {
    class impl;
    std::unique_ptr<impl> m_pimpl;

  public:
    editor_data(data_context& dctx);
    ~editor_data() noexcept;
    editor_data(editor_data &&) noexcept;
    editor_data(const editor_data &) = delete;

  public:
    /// add module loader
    void add_module_loader(std::unique_ptr<yave::module_loader> loader);
    /// load current modules
    void load_modules(std::vector<std::string> moudle_names);
    /// unload modules
    void unload_modules();
    /// init modules
    void init_modules(const yave::scene_config &cfg);
    /// update modules
    void update_modules(const yave::scene_config &sfg);
    /// deinit moudles
    void deinit_modules();
    /// init task threads
    void init_threads();
    /// deinit task threads
    void deinit_threads();

  public: /* main */
    /// name
    auto name() const -> const std::string &;
    /// file path
    auto path() const -> const std::filesystem::path &;

  public: /* node */
    /// scene config
    auto scene_config() const -> const yave::scene_config &;
    /// node decl list
    auto node_declarations() const -> const node_declaration_store &;
    /// node def list
    auto node_definitions() const -> const node_definition_store &;
    /// node graph
    auto node_graph() const -> const structured_node_graph &;
    auto node_graph() -> structured_node_graph &;
    /// node group
    auto root_group() const -> const node_handle &;
    auto root_group() -> node_handle &;

  public:
    /// compiler interface
    auto compile_thread() -> compile_thread_data &;
    auto compile_thread() const -> const compile_thread_data &;
    /// executor interface
    auto execute_thread() -> execute_thread_data &;
    auto execute_thread() const -> const execute_thread_data &;

  public:
    /// update channel
    auto update_channel() -> node_argument_update_channel &;
    auto update_channel() const -> const node_argument_update_channel &;
  };
} // namespace yave::editor