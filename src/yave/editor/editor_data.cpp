//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/editor/editor_data.hpp>

namespace yave::editor {

  class editor_data::impl
  {
  public: /* main */
    /// name
    std::string name;
    /// file path
    std::filesystem::path path;

  public: /* node */
    /// scene config
    yave::scene_config scene_config;
    /// loaded module names
    std::vector<std::string> loaded_module_names;
    /// module loader
    std::vector<std::unique_ptr<yave::module_loader>> module_loaders;
    /// node decl list
    node_declaration_store node_decls;
    /// node def list
    node_definition_store node_defs;
    /// node graph
    structured_node_graph node_graph;
    /// node group
    node_handle root_group;

  public:
    /// compiler interface
    compile_thread_data compiler;
    /// executor interface
    execute_thread_data executor;

  public:
    /// update channel
    node_argument_update_channel updates;

  public:
    impl(data_context& dctx)
      : compiler {dctx}
      , executor {dctx}
    {
      init();
    }

    ~impl() noexcept
    {
      deinit();
    }

  public:
    /// init data
    void init();
    /// deinit data
    void deinit() noexcept;

  public:
    /// add module loader
    void add_module_loader(std::unique_ptr<yave::module_loader> loader);

    /// load current modules
    void load_modules(std::vector<std::string> module_names);
    /// unload modules
    void unload_modules();

    /// init modules
    void init_modules(const yave::scene_config& cfg);
    /// update modules
    void update_modules(const yave::scene_config& sfg);
    /// deinit moudles
    void deinit_modules();

    /// init task threads
    void init_threads();
    /// deinit task threads
    void deinit_threads();
  };

  void editor_data::impl::init()
  {
    name         = "";
    path         = "";
    scene_config = yave::scene_config();

    root_group = node_graph.create_group({}, {});
    node_graph.set_name(root_group, "ProjectRoot");
    node_graph.add_output_socket(root_group, "frame_out");
  }

  void editor_data::impl::deinit() noexcept
  {
    for (auto&& loader : module_loaders)
      assert(loader->get().empty());

    assert(loaded_module_names.empty());
  }

  void editor_data::impl::add_module_loader(
    std::unique_ptr<yave::module_loader> loader)
  {
    assert(loader);
    module_loaders.push_back(std::move(loader));
  }

  void editor_data::impl::load_modules(std::vector<std::string> module_names)
  {
    std::vector<module_loader*> loaded;
    loaded.reserve(module_loaders.size());

    for (auto&& loader : module_loaders) {

      if (!loader->load(module_names)) {
        // unload modules already loaded
        for (auto&& l : loaded) {
          l->unload(module_names);
        }
        throw std::runtime_error("failed load moudle");
      }
      loaded.push_back(loader.get());
    }
    loaded_module_names = module_names;
  }

  void editor_data::impl::unload_modules()
  {
    for (auto&& loader : module_loaders) {
      for (auto m : loader->get()) {
        // make sure all modules are deinitialized
        if (m->initialized()) {
          assert(false);
          m->deinit();
        }
      }
      loader->unload(loaded_module_names);
    }
    loaded_module_names = {};
  }

  void editor_data::impl::init_modules(const yave::scene_config& cfg)
  {
    // init modules
    for (auto&& loader : module_loaders) {
      for (auto&& m : loader->get()) {
        assert(!m->initialized());
        if (!m->initialized()) {
          m->init(cfg);
          assert(m->initialized());
        }
      }
    }
    scene_config = cfg;

    // load decls/defs
    for (auto&& loader : module_loaders) {
      for (auto&& m : loader->get()) {
        (void)node_decls.add(m->get_node_declarations());
        (void)node_defs.add(m->get_node_definitions());
      }
    }

    // add declarations
    auto decls  = node_decls.get_list();
    auto remain = decls.size();
    while (true) {

      // nothing to add
      if (remain < 1)
        break;

      auto remain_old = remain;

      for (auto i = size_t(0); i < remain; ++i) {

        // decl to add
        auto& decl = decls[i];

        // create one decl and remove from queue
        if (create_declaration(node_graph, decl)) {
          decl = nullptr;
          std::swap(decl, decls[--remain]);
          break;
        }
      }

      // could not add any
      if (remain_old == remain)
        break;
    }
  }

  void editor_data::impl::update_modules(const yave::scene_config& cfg)
  {
    for (auto&& loader : module_loaders) {
      for (auto&& m : loader->get()) {
        assert(m->initialized());
        if (m->initialized())
          m->update(cfg);
      }
    }
    scene_config = cfg;
  }

  void editor_data::impl::deinit_modules()
  {
    node_decls.clear();
    node_defs.clear();

    for (auto&& loader : module_loaders) {
      for (auto&& m : loader->get()) {
        assert(m->initialized());
        if (m->initialized())
          m->deinit();
      }
    }
    // leave scene_config as-is
  }

  void editor_data::impl::init_threads()
  {
    compiler.init();
    executor.init();
  }

  void editor_data::impl::deinit_threads()
  {
    compiler.deinit();
    executor.deinit();
  }

  editor_data::editor_data(data_context& dctx)
    : m_pimpl {std::make_unique<impl>(dctx)}
  {
  }

  editor_data::~editor_data() noexcept             = default;
  editor_data::editor_data(editor_data&&) noexcept = default;

  void editor_data::add_module_loader(
    std::unique_ptr<yave::module_loader> loader)
  {
    m_pimpl->add_module_loader(std::move(loader));
  }

  void editor_data::load_modules(std::vector<std::string> module_names)
  {
    m_pimpl->load_modules(std::move(module_names));
  }

  void editor_data::unload_modules()
  {
    m_pimpl->unload_modules();
  }

  void editor_data::init_modules(const yave::scene_config& cfg)
  {
    m_pimpl->init_modules(cfg);
  }

  void editor_data::update_modules(const yave::scene_config& cfg)
  {
    m_pimpl->update_modules(cfg);
  }

  void editor_data::deinit_modules()
  {
    m_pimpl->deinit_modules();
  }

  void editor_data::init_threads()
  {
    m_pimpl->init_threads();
  }

  void editor_data::deinit_threads()
  {
    m_pimpl->deinit_threads();
  }

  auto editor_data::name() const -> const std::string&
  {
    return m_pimpl->name;
  }

  auto editor_data::path() const -> const std::filesystem::path&
  {
    return m_pimpl->path;
  }

  auto editor_data::scene_config() const -> const yave::scene_config&
  {
    return m_pimpl->scene_config;
  }

  auto editor_data::node_declarations() const -> const node_declaration_store&
  {
    return m_pimpl->node_decls;
  }

  auto editor_data::node_definitions() const -> const node_definition_store&
  {
    return m_pimpl->node_defs;
  }

  auto editor_data::node_graph() const -> const structured_node_graph&
  {
    return m_pimpl->node_graph;
  }

  auto editor_data::node_graph() -> structured_node_graph&
  {
    return m_pimpl->node_graph;
  }

  auto editor_data::root_group() const -> node_handle
  {
    return m_pimpl->root_group;
  }

  auto editor_data::compile_thread() -> compile_thread_data&
  {
    return m_pimpl->compiler;
  }

  auto editor_data::compile_thread() const -> const compile_thread_data&
  {
    return m_pimpl->compiler;
  }

  auto editor_data::execute_thread() -> execute_thread_data&
  {
    return m_pimpl->executor;
  }

  auto editor_data::execute_thread() const -> const execute_thread_data&
  {
    return m_pimpl->executor;
  }

  auto editor_data::update_channel() -> node_argument_update_channel&
  {
    return m_pimpl->updates;
  }

  auto editor_data::update_channel() const
    -> const node_argument_update_channel&
  {
    return m_pimpl->updates;
  }

} // namespace yave::editor