//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/app/project.hpp>

#include <yave/node/core/managed_node_graph.hpp>
#include <yave/node/core/node_declaration_store.hpp>
#include <yave/node/core/node_definition_store.hpp>
#include <yave/module/module.hpp>

#include <yave/support/log.hpp>

YAVE_DECL_G_LOGGER(project)

namespace yave::app {

  class project::impl
  {
  public:
    // project configurations
    project_config cfg;
    // project live data
    project_data data;

  public:
    void init_modules()
    {
      Info(g_logger, "init module backends");
      for (auto&& m : data.modules) {
        m->init(cfg.scene_cfg);
      }
    }

    void deinit_modules()
    {
      Info(g_logger, "deinit module bakcneds");
      for (auto&& m : data.modules) {
        m->deinit();
      }
    }

    void update_modules()
    {
      Info(g_logger, "update module backends");
      for (auto&& m : data.modules) {
        m->update(cfg.scene_cfg);
      }
    }

  public:
    impl(const project_config& init_config, module_loader& loader)
      : cfg {init_config}
    {
      init_logger();

      Info(g_logger, "Creating new project:");
      Info(g_logger, "  name: {}", cfg.name);
      Info(g_logger, "  path: {}", cfg.path.string());
      Info(
        g_logger,
        "  scene resolution: {}x{}",
        cfg.scene_cfg.width(),
        cfg.scene_cfg.height());

      Info(
        g_logger, "  scene frame: {}", to_string(cfg.scene_cfg.frame_format()));

      Info(
        g_logger, "  scene audio: {}", to_string(cfg.scene_cfg.audio_format()));

      Info(g_logger, "  following modules will be enabled:");

      for (auto&& m : cfg.modules) {
        Info(g_logger, "    {}", m);
      }

      // load moudles
      if (!loader.load(cfg.modules)) {
        Error(g_logger, "Failed to load some modules");
        for (auto&& e : loader.last_errors()) {
          Error(g_logger, "  {}", e.message());
        }
        Warning(g_logger, "Some modules will not be loaded");
      }

      data.modules = loader.get();

      // init
      init_modules();

      // register
      for (auto&& m : data.modules) {
        Info(
          g_logger,
          "Added module: {} (inst {} of module {})",
          m->name(),
          to_string(m->instance_id()),
          to_string(m->module_id()));
        assert(m->initialized());
        if (!data.decls.add(m->get_node_declarations())) {
          Error(g_logger, "Failed to register node declaration");
        }
        data.defs.add(m->get_node_definitions());
      }
    }

    ~impl() noexcept
    {
      deinit_modules();
    }

  public:
    auto& get_name() const
    {
      return cfg.name;
    }

    void set_name(const std::string& new_name)
    {
      Info(g_logger, "Set new project name: {}", new_name);
      cfg.name = new_name;
    }

  public:
    auto& get_scene_config() const
    {
      return cfg.scene_cfg;
    }

    void set_scene_config(const scene_config& new_cfg)
    {
      Info(g_logger, "Set new scene config");
      cfg.scene_cfg = new_cfg;
      update_modules();
    }

  public:
    bool has_path() const
    {
      return cfg.path == std::filesystem::path();
    }

    auto& get_path() const
    {
      return cfg.path;
    }

    void set_path(const std::filesystem::path& new_path)
    {
      Info(g_logger, "Set new project path: {}", new_path.string());
      cfg.path = new_path;
    }
  };

  project::project(const project_config& config, module_loader& loader)
    : m_pimpl {std::make_unique<impl>(config, loader)}
  {
  }

  project::~project() noexcept = default;

  auto project::get_name() const -> std::string
  {
    return m_pimpl->get_name();
  }

  void project::set_name(const std::string& new_name)
  {
    m_pimpl->set_name(new_name);
  }

  auto project::get_scene_config() const -> scene_config
  {
    return m_pimpl->get_scene_config();
  }

  void project::set_scene_config(const scene_config& new_config)
  {
    m_pimpl->set_scene_config(new_config);
  }

  bool project::has_path() const
  {
    return m_pimpl->has_path();
  }

  auto project::get_path() const -> std::optional<std::filesystem::path>
  {
    return m_pimpl->get_path();
  }

  void project::set_path(const std::filesystem::path& new_path)
  {
    m_pimpl->set_path(new_path);
  }

  auto project::node_declarations() const -> const node_declaration_store&
  {
    return m_pimpl->data.decls;
  }

  auto project::node_definitions() const -> const node_definition_store&
  {
    return m_pimpl->data.defs;
  }

  auto project::modules() const -> const std::vector<std::string>&
  {
    return m_pimpl->cfg.modules;
  }

  auto project::graph() -> managed_node_graph&
  {
    return m_pimpl->data.graph;
  }

  auto project::graph() const -> const managed_node_graph&
  {
    return m_pimpl->data.graph;
  }
} // namespace yave::app