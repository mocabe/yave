//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave-imgui/application.hpp>
#include <yave-imgui/root_window.hpp>

#include <yave/lib/vulkan/vulkan_context.hpp>
#include <yave/lib/imgui/imgui_context.hpp>
#include <yave/module/std/module_loader.hpp>
#include <yave/editor/data_context.hpp>
#include <yave/editor/view_context.hpp>
#include <yave/editor/editor_data.hpp>
#include <yave/editor/compile_thread.hpp>
#include <yave/editor/execute_thread.hpp>

#include <imgui.h>
#include <imgui_internal.h>

namespace yave::editor::imgui {

  class application::impl
  {
  public: /* vulkan */
    yave::vulkan::vulkan_context vulkan_ctx;

  public: /* imgui */
    yave::imgui::imgui_context imgui_ctx;

  public: /* editor contexts */
    yave::editor::data_context data_ctx;
    yave::editor::view_context view_ctx;

  public: /* threads */
    yave::editor::compile_thread compiler;
    yave::editor::execute_thread executor;

  public:
    impl();
    ~impl() noexcept;

  public:
    /// run main loop
    void run();

  public:
    void init_data();
    void deinit_data();
  };

  using vkflags = vulkan::vulkan_context::init_flags;

  /// initialize internal data structure
  void application::impl::init_data()
  {
    auto lck   = data_ctx.lock();
    auto& data = data_ctx.data();

    data.name         = "New Project";
    data.path         = "";
    data.scene_config = scene_config();

    data.module_loader =
      std::make_unique<modules::_std::module_loader>(vulkan_ctx);

    // load std module
    if (!data.module_loader->load({modules::_std::module_name}))
      throw std::runtime_error("failed load moudle");

    // init module
    for (auto&& m : data.module_loader->get())
      m->init(data.scene_config);

    // load decl/def
    for (auto&& m : data.module_loader->get()) {
      if (!data.node_decls.add(m->get_node_declarations()))
        throw std::runtime_error("Failed to add node decl");
      if (!data.node_defs.add(m->get_node_definitions()))
        throw std::runtime_error("Failed to add node def");
    }

    // register functions
    for (auto&& decl : data.node_decls.enumerate())
      if (!data.node_graph.create_function(*decl))
        throw std::runtime_error("failed to create builtin function");

    // create root group
    data.root_group = data.node_graph.create_group({}, {});
    data.node_graph.set_name(data.root_group, "root");
    data.node_graph.add_output_socket(data.root_group, "out");

    // init compiler
    data.compiler.init(compiler);
    data.executor.init(executor);
  }

  void application::impl::deinit_data()
  {
    auto lck   = data_ctx.lock();
    auto& data = data_ctx.data();

    data.compiler.deinit();
    data.executor.deinit();

    // deinit modules
    for (auto&& m : data.module_loader->get())
      m->deinit();
  }

  application::impl::impl()
    : vulkan_ctx {vkflags::enable_logging | vkflags::enable_validation}
    , imgui_ctx {vulkan_ctx}
    , data_ctx {}
    , view_ctx {data_ctx}
    , compiler {data_ctx}
    , executor {data_ctx}
  {
    init_data();

    // root window
    view_ctx.window_manager().root()->add_window(
      std::make_unique<root_window>(imgui_ctx));
  }

  application::impl::~impl() noexcept
  {
    deinit_data();
  }

  void application::impl::run()
  {
    auto& glfw_win = imgui_ctx.window_context().glfw_win();

    while (!imgui_ctx.window_context().should_close()) {
      imgui_ctx.begin_frame();
      {
        view_ctx.draw();
      }
      imgui_ctx.end_frame();
      imgui_ctx.render();
    }
    // avoid resource destruction before finishing render tasks.
    imgui_ctx.window_context().device().waitIdle();
  }

  application::application()
    : m_pimpl {std::make_unique<impl>()}
  {
  }

  application::~application() noexcept = default;

  void application::run()
  {
    m_pimpl->run();
  }
} // namespace yave::editor::imgui