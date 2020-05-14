//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/editor/data_context.hpp>
#include <yave/support/log.hpp>
#include <yave/support/overloaded.hpp>

#include <functional>
#include <chrono>
#include <thread>
#include <queue>
#include <stack>
#include <variant>

YAVE_DECL_G_LOGGER(data_context)

namespace yave::editor {

  namespace {

    struct cmd_undo
    {
    };

    struct cmd_redo
    {
    };

    using cmd_ptr = std::unique_ptr<data_command>;

    using cmd = std::variant<cmd_ptr, cmd_undo, cmd_redo>;

    // internal editor data holder
    struct editor_data
    {
      auto& node_graph()
      {
        return m_node_graph;
      }

      auto& node_graph() const
      {
        return m_node_graph;
      }

    private:
      structured_node_graph m_node_graph;
    };

  } // namespace

  class data_context::impl
  {
    data_context* _this;

  public:
    impl(data_context* _this_ptr)
      : _this {_this_ptr}
    {
      init_logger();
      thread_run();
    }

    ~impl() noexcept
    {
      if (thread.joinable()) {
        terminate_flag = true;
        exec(make_data_command([](auto&) {}, [](auto&) {}));
        thread.join();
      }
    }

  private:
    /// operator mutex
    std::mutex cmd_mtx;
    /// operator cond
    std::condition_variable cmd_cond;
    /// command queue
    std::queue<cmd> cmd_queue;
    /// undo stack
    std::stack<cmd_ptr> cmd_undo_stack;
    /// exec stack
    std::stack<cmd_ptr> cmd_redo_stack;

  public:
    /// editor data
    editor::editor_data editor_data;

  private:
    /// data processing thread
    std::thread thread;
    /// thread termination flag
    std::atomic<bool> terminate_flag = false;
    /// error
    std::exception_ptr exception;

  public:
    auto cmd_lock()
    {
      return std::unique_lock {cmd_mtx};
    }

    void check_exception()
    {
      if (exception)
        std::rethrow_exception(exception);
    }

  public:
    void exec(cmd_ptr&& op)
    {
      auto lck = cmd_lock();
      if (op)
        cmd_queue.emplace(std::move(op));
      cmd_cond.notify_one();
    }

    void undo()
    {
      auto lck = cmd_lock();
      cmd_queue.emplace(cmd_undo {});
      cmd_cond.notify_one();
    }

    void redo()
    {
      auto lck = cmd_lock();
      cmd_queue.emplace(cmd_redo {});
      cmd_cond.notify_one();
    }

  public:
    void exec_one()
    {
      cmd top;
      {
        auto lck = cmd_lock();
        top      = std::move(cmd_queue.front());
        cmd_queue.pop();
      }

      // switch based on command types
      std::visit(
        [&](auto&& x) { process(std::forward<decltype(x)>(x)); },
        std::move(top));
    }

    // process normal commands
    void process(cmd_ptr&& top)
    {
      // execute
      top->exec(*_this);
      // register to undo stack
      auto lck = cmd_lock();
      cmd_undo_stack.push(std::move(top));
    }

    // process undo
    void process(cmd_undo)
    {
      cmd_ptr top;
      {
        auto lck = cmd_lock();

        if (cmd_undo_stack.empty())
          return;

        top = std::move(cmd_undo_stack.top());
        cmd_undo_stack.pop();
      }

      // execute
      top->undo(*_this);

      {
        auto lck = cmd_lock();
        cmd_redo_stack.push(std::move(top));
      }
    }

    // process redo
    void process(cmd_redo)
    {
      cmd_ptr top;
      {
        auto lck = cmd_lock();

        if (cmd_redo_stack.empty())
          return;

        top = std::move(cmd_redo_stack.top());
        cmd_redo_stack.pop();
      }

      // execute
      top->exec(*_this);

      {
        auto lck = cmd_lock();
        cmd_undo_stack.push(std::move(top));
      }
    }

  public:
    void wait_cmd()
    {
      auto lck = cmd_lock();
      Info(g_logger, "waiting...");
      cmd_cond.wait(lck, [&] { return !cmd_queue.empty(); });
    }

    auto cmd_size()
    {
      auto lck = cmd_lock();
      return cmd_queue.size();
    }

    void thread_run()
    {
      Info(g_logger, "Start running data thread");

      thread = std::thread([&] {
        try {
          while (!terminate_flag) {

            wait_cmd();

            while (cmd_size())
              exec_one();
          }
          Info(g_logger, "Terminating data thread");
        } catch (...) {
          Error(g_logger, "Exception detected in data thread");
          exception = std::current_exception();
        }
      });
    }
  };

  data_context::data_context()
    : m_pimpl {std::make_unique<impl>(this)}
  {
  }

  data_context::~data_context() noexcept = default;

  void data_context::exec(cmd_ptr&& op)
  {
    m_pimpl->check_exception();
    m_pimpl->exec(std::move(op));
  }

  void data_context::undo()
  {
    m_pimpl->check_exception();
    m_pimpl->undo();
  }

  void data_context::redo()
  {
    m_pimpl->check_exception();
    m_pimpl->redo();
  }

  auto data_context::node_graph() const -> const structured_node_graph&
  {
    m_pimpl->check_exception();
    return m_pimpl->editor_data.node_graph();
  }

  auto data_context::node_graph() -> structured_node_graph&
  {
    m_pimpl->check_exception();
    return m_pimpl->editor_data.node_graph();
  }

} // namespace yave::editor