//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/editor/view_context.hpp>
#include <yave/editor/render_context.hpp>
#include <yave/wm/key_events.hpp>
#include <yave/wm/mouse_events.hpp>
#include <yave/wm/event_dispatcher.hpp>

#include <yave/support/log.hpp>

#include <queue>
#include <chrono>
#include <thread>
#include <iostream>

YAVE_DECL_G_LOGGER(view_context)

namespace yave::editor {

  namespace {

    // command
    using cmd_ptr = std::unique_ptr<view_command>;

    // internal view data holder
    class view_data
    {
    public:
      /// widnow manager
      wm::window_manager wm;

    public:
      view_data()
        : wm {}
      {
        init_logger();
      }
    };

  } // namespace

  class view_context::impl
  {
  public:
    /// data context
    data_context* pdata;
    /// view context
    view_context* pview;

  public:
    /// data
    view_data data;
    /// command queue
    std::queue<cmd_ptr> cmd_queue;

  public:
    impl(data_context* d, view_context* v)
      : pdata {d}
      , pview {v}
    {
    }

  public:
    void push(cmd_ptr&& cmd)
    {
      if (cmd)
        cmd_queue.push(std::move(cmd));
    }

    void exec_all()
    {
      while (!cmd_queue.empty()) {
        auto cmd = std::move(cmd_queue.front());
        cmd_queue.pop();
        // exec
        cmd->exec(*pview);
      }
    }

  public:
    /// view update stage
    void stage_update()
    {
      // update windows
      data.wm.update(*pdata, *pview);
    }
    /// event handling stage
    void stage_events()
    {
      // dispatch events
      data.wm.events(*pdata, *pview);
    }
    /// rendering stage
    void stage_render()
    {
      // render windows
      data.wm.render();
    }
    /// command process stage
    void stage_commands()
    {
      // run commands
      pview->exec_all();
    }

  public:
    /// main loop
    void run()
    {
      // main loop
      while (!data.wm.should_close()) {
        // update view data
        stage_update();
        // event handling
        stage_events();
        // render content
        stage_render();
        // execute commands
        stage_commands();
      }
    }

  };

  view_context::view_context(data_context& dctx)
    : m_pimpl {std::make_unique<impl>(&dctx, this)}
  {
  }

  view_context::~view_context() noexcept = default;

  void view_context::push(cmd_ptr&& cmd)
  {
    m_pimpl->push(std::move(cmd));
  }

  void view_context::exec_all()
  {
    m_pimpl->exec_all();
  }

  void view_context::run()
  {
    m_pimpl->run();
  }

  auto view_context::window_manager() const -> const wm::window_manager&
  {
    return m_pimpl->data.wm;
  }

  auto view_context::window_manager() -> wm::window_manager&
  {
    return m_pimpl->data.wm;
  }

} // namespace yave::editor
