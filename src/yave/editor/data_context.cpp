//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/editor/data_context.hpp>
#include <yave/editor/data_command.hpp>
#include <yave/editor/editor_data.hpp>
#include <yave/support/log.hpp>
#include <yave/support/overloaded.hpp>

#include <functional>
#include <chrono>
#include <thread>
#include <queue>
#include <stack>
#include <variant>
#include <list>
#include <algorithm>
#include <typeinfo>
#include <typeindex>

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
    std::list<unique_context_data> data_list;

  private:
    /// data processing thread
    std::thread thread;
    /// thread termination flag
    std::atomic<bool> terminate_flag = false;
    /// error
    std::exception_ptr exception;

  private:
    /// data lock
    std::mutex data_mtx;

  private:
    /// acquire command queue lock
    auto lock_queue()
    {
      return std::unique_lock {cmd_mtx};
    }

    /// acquire data lock
    auto lock_data()
    {
      return std::unique_lock {data_mtx};
    }

  public:
    /// check exception in data thread
    void check_exception()
    {
      if (exception)
        std::rethrow_exception(exception);
    }

    /// push exec
    void exec(cmd_ptr&& op)
    {
      auto lck = lock_queue();
      if (op)
        cmd_queue.emplace(std::move(op));
      cmd_cond.notify_one();
    }

    /// push undo
    void undo()
    {
      auto lck = lock_queue();
      cmd_queue.emplace(cmd_undo {});
      cmd_cond.notify_one();
    }

    /// push redo
    void redo()
    {
      auto lck = lock_queue();
      cmd_queue.emplace(cmd_redo {});
      cmd_cond.notify_one();
    }

    /// get read lock
    auto lock()
    {
      return lock_data();
    }

  public:
    void add_data(unique_context_data new_data)
    {
      assert(!new_data.empty());

      auto lb = std::lower_bound(
        data_list.begin(),
        data_list.end(),
        new_data.type(),
        [](auto& l, auto& r) {
          return std::type_index(l.type()) < std::type_index(r);
        });

      if (lb == data_list.end() || lb->type() != new_data.type()) {
        data_list.insert(lb, std::move(new_data));
        return;
      }

      throw std::runtime_error("data_context: data type already exists");
    }

    void remove_data(const std::type_info& id)
    {
      auto lb = std::lower_bound(
        data_list.begin(), data_list.end(), id, [](auto& l, auto& r) {
          return std::type_index(l.type()) < std::type_index(r);
        });

      if (lb == data_list.end() || lb->type() != id)
        return;

      data_list.erase(lb);
    }

    auto find_data(const std::type_info& id) -> void*
    {
      auto lb = std::lower_bound(
        data_list.begin(), data_list.end(), id, [](auto& l, auto& r) {
          return std::type_index(l.type()) < std::type_index(r);
        });

      if (lb == data_list.end() || lb->type() != id)
        return nullptr;

      return lb->data();
    }

  public:
    // execute single command
    void exec_one()
    {
      cmd top;
      {
        auto lck = lock_queue();
        top      = std::move(cmd_queue.front());
        cmd_queue.pop();
      }

      std::visit(
        [&](auto&& x) { process(std::forward<decltype(x)>(x)); },
        std::move(top));
    }

    // process normal commands
    void process(cmd_ptr&& top)
    {
      {
        auto lck = data_context_access(*_this, lock_data());
        top->exec(lck);
      }

      // dispose command if not undoable
      if (top->type() == data_command_type::undo_redo) {
        auto lck = lock_queue();
        cmd_undo_stack.push(std::move(top));
      }
    }

    // process undo
    void process(cmd_undo)
    {
      cmd_ptr top;
      {
        auto lck = lock_queue();

        if (cmd_undo_stack.empty())
          return;

        top = std::move(cmd_undo_stack.top());
        cmd_undo_stack.pop();
      }

      {
        auto lck = data_context_access(*_this, lock_data());
        top->undo(lck);
      }

      {
        auto lck = lock_queue();
        cmd_redo_stack.push(std::move(top));
      }
    }

    // process redo
    void process(cmd_redo)
    {
      cmd_ptr top;
      {
        auto lck = lock_queue();

        if (cmd_redo_stack.empty())
          return;

        top = std::move(cmd_redo_stack.top());
        cmd_redo_stack.pop();
      }

      {
        auto lck = data_context_access(*_this, lock_data());
        top->exec(lck);
      }

      {
        auto lck = lock_queue();
        cmd_undo_stack.push(std::move(top));
      }
    }

  public:
    void wait_cmd()
    {
      auto lck = lock_queue();
      cmd_cond.wait(lck, [&] { return !cmd_queue.empty(); });
    }

    auto cmd_size()
    {
      auto lck = lock_queue();
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

  void data_context::exec(cmd_ptr&& op) const
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

  auto data_context::lock() -> data_context_access
  {
    m_pimpl->check_exception();
    return data_context_access(*this, m_pimpl->lock());
  }

  auto data_context::lock() const -> const_data_context_access
  {
    m_pimpl->check_exception();
    return const_data_context_access(*this, m_pimpl->lock());
  }

  void data_context::add_data(unique_context_data new_data)
  {
    m_pimpl->check_exception();
    m_pimpl->add_data(std::move(new_data));
  }

  void data_context::remove_data(const std::type_info& ti)
  {
    m_pimpl->check_exception();
    m_pimpl->remove_data(ti);
  }

  auto data_context::get_data(const std::type_info& id) const -> const void*
  {
    m_pimpl->check_exception();
    return m_pimpl->find_data(id);
  }

  auto data_context::get_data(const std::type_info& id) -> void*
  {
    m_pimpl->check_exception();
    return m_pimpl->find_data(id);
  }

} // namespace yave::editor