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

    using cmd_t = std::variant<cmd_ptr, cmd_undo, cmd_redo>;

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
        cmd(make_data_command([](auto&) {}, [](auto&) {}));
        thread.join();
      }
    }

  private:
    /// operator mutex
    std::mutex cmd_mtx;
    /// operator cond
    std::condition_variable cmd_cond;
    /// command queue
    std::queue<cmd_t> cmd_queue;
    /// undo stack
    std::stack<cmd_ptr> cmd_undo_stack;
    /// exec stack
    std::stack<cmd_ptr> cmd_redo_stack;

  public:
    /// editor data list
    std::vector<std::shared_ptr<data_holder>> data_list;

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

    /// push command
    void cmd(cmd_ptr&& op)
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

  public:
    void add_data(unique_any new_data)
    {
      auto lck = lock_data();

      assert(!new_data.empty());

      auto lb = std::lower_bound(
        data_list.begin(),
        data_list.end(),
        new_data.type(),
        [](auto& l, auto& r) {
          return std::type_index(l->data.type()) < std::type_index(r);
        });

      if (lb == data_list.end() || (*lb)->data.type() != new_data.type()) {

        data_list.insert(
          lb, std::make_shared<data_holder>(std::move(new_data)));

        return;
      }

      throw std::runtime_error("data_context: data type already exists");
    }

    void remove_data(const std::type_info& id)
    {
      auto lck = lock_data();

      auto lb = std::lower_bound(
        data_list.begin(), data_list.end(), id, [](auto& l, auto& r) {
          return std::type_index(l->data.type()) < std::type_index(r);
        });

      if (lb == data_list.end() || (*lb)->data.type() != id)
        return;

      data_list.erase(lb);
    }

    auto find_data(const std::type_info& id) -> std::shared_ptr<data_holder>
    {
      auto lck = lock_data();

      auto lb = std::lower_bound(
        data_list.begin(), data_list.end(), id, [](auto& l, auto& r) {
          return std::type_index(l->data.type()) < std::type_index(r);
        });

      if (lb == data_list.end() || (*lb)->data.type() != id)
        return nullptr;

      return *lb;
    }

  public:
    // execute single command
    void exec_one()
    {
      cmd_t top;
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
        top->exec(*_this);
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
        top->undo(*_this);
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
        top->exec(*_this);
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

  void data_context::cmd(cmd_ptr&& op) const
  {
    m_pimpl->check_exception();
    m_pimpl->cmd(std::move(op));
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

  void data_context::_add_data(unique_any new_data)
  {
    m_pimpl->check_exception();
    m_pimpl->add_data(std::move(new_data));
  }

  void data_context::_remove_data(const std::type_info& ti)
  {
    m_pimpl->check_exception();
    m_pimpl->remove_data(ti);
  }

  auto data_context::_get_data(const std::type_info& id) const
    -> std::shared_ptr<data_holder>
  {
    m_pimpl->check_exception();
    return m_pimpl->find_data(id);
  }

} // namespace yave::editor