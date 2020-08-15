//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/editor/view_context.hpp>

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
    data_context& data_ctx;
    /// view context
    view_context& view_ctx;

  public:
    /// data
    view_data data;
    /// command queue
    std::queue<cmd_ptr> cmd_queue;

  public:
    impl(data_context& d, view_context& v)
      : data_ctx {d}
      , view_ctx {v}
      , data {}
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
        auto access = view_context_access(view_ctx);
        cmd->exec(access);
      }
    }

    /// event loop
    void run()
    {
      while (!data.wm.should_close()) {
        // do event handling
        data.wm.exec(data_ctx, view_ctx);
        // update view model
        view_ctx.exec_all();
      }
    }
  };

  view_context::view_context(data_context& dctx)
    : m_pimpl {std::make_unique<impl>(dctx, *this)}
  {
  }

  view_context::~view_context() noexcept = default;

  void view_context::push(cmd_ptr&& cmd) const
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