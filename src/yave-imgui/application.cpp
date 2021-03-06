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

  public:
    impl();
    ~impl() noexcept;

  public:
    /// run main loop
    void run();

  public:
    void init_data();
    void init_threads();
    void deinit_threads();
    void deinit_data();
  };

  /// initialize internal data structure
  void application::impl::init_data()
  {
    data_ctx.add_data(editor_data(data_ctx));

    auto lck   = data_ctx.get_data<editor_data>();
    auto& data = lck.ref();

    data.add_module_loader(
      std::make_unique<modules::_std::module_loader>(vulkan_ctx));

    data.load_modules({{modules::_std::module_name}});
    data.init_modules(data.scene_config());
  }

  void application::impl::init_threads()
  {
    data_ctx.add_data(compile_thread(data_ctx));
    data_ctx.add_data(execute_thread(data_ctx));

    {
      auto lck = data_ctx.get_data<execute_thread>();
      lck.ref().start();
    }
    {
      auto lck = data_ctx.get_data<compile_thread>();
      lck.ref().start();
      lck.ref().notify_compile();
    }
  }

  void application::impl::deinit_threads()
  {
    {
      auto lck = data_ctx.get_data<compile_thread>();
      lck.ref().stop();
    }
    {
      auto lck = data_ctx.get_data<execute_thread>();
      lck.ref().stop();
    }
  }

  void application::impl::deinit_data()
  {
    auto lck   = data_ctx.get_data<editor_data>();
    auto& data = lck.ref();
    data.deinit_modules();
    data.unload_modules();
  }

  application::impl::impl()
    : vulkan_ctx {{}}
    , imgui_ctx {vulkan_ctx, {.width = 1280, .height = 720, .name = "yave"}}
    , data_ctx {}
    , view_ctx {data_ctx}
  {
    init_data();
    init_threads();

    // root window
    view_ctx.window_manager().root()->add_window(
      std::make_unique<root_window>(imgui_ctx));
  }

  application::impl::~impl() noexcept
  {
    deinit_threads();
    deinit_data();
  }

  void application::impl::run()
  {
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