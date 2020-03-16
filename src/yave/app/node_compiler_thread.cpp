//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/app/node_compiler_thread.hpp>
#include <yave/support/log.hpp>

#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <variant>

YAVE_DECL_G_LOGGER(node_compiler_thread)

namespace yave::app {

  class node_compiler_thread::impl
  {
  public:
    struct queue_data
    {
      std::shared_ptr<const node_data_snapshot> snapshot;
      node_definition_store defs;
    };

  public:
    std::thread thread;
    std::atomic<int> terminate_flag;
    std::mutex mtx;
    std::condition_variable cond;
    std::queue<queue_data> queue;
    std::shared_ptr<compile_result> result;
    node_parser parser;
    node_compiler compiler;

  public:
    impl()
      : terminate_flag {0}
      , result {std::make_shared<compile_result>()}
    {
      init_logger();

      result->success  = false;
      result->bgn_time = std::chrono::steady_clock::now();
      result->end_time = std::chrono::steady_clock::now();
    }

    ~impl() noexcept
    {
      if (is_running())
        stop();
    }

    void start()
    {
      if (is_running())
        throw std::runtime_error("Compiler thread already running");

      Info(g_logger, "Starting compiler thread");

      thread = std::thread([&]() {
        try {
          while (true) {

            std::unique_lock lck {mtx};
            cond.wait(
              lck, [&] { return terminate_flag || !queue.empty(); });

            // terminate
            if (terminate_flag)
              break;

            // compile
            if (!queue.empty()) {

              auto start = std::chrono::steady_clock::now();

              assert(!queue.empty());

              auto top = std::move(queue.front());
              queue.pop();

              auto new_result = std::make_shared<compile_result>();

              // parse
              auto parsed = parser.parse(top.snapshot->graph);

              if (!parsed) {
                new_result->success      = false;
                new_result->parse_errors = parser.get_errors();
                new_result->bgn_time     = start;
                new_result->end_time     = std::chrono::steady_clock::now();
                std::atomic_store(&result, std::move(new_result));
                continue;
              } else {
                // compile
                auto exe = compiler.compile(std::move(*parsed), top.defs);
                if (!exe) {
                  new_result->success        = false;
                  new_result->compile_errors = compiler.get_errors();
                  new_result->bgn_time       = start;
                  new_result->end_time       = std::chrono::steady_clock::now();
                  std::atomic_store(&result, std::move(new_result));
                  continue;
                } else {
                  new_result->success  = true;
                  new_result->bgn_time = start;
                  new_result->end_time = std::chrono::steady_clock::now();
                  new_result->exe      = std::move(*exe);
                  std::atomic_store(&result, std::move(new_result));
                  continue;
                }
              }
            }
          }
        } catch (const std::exception& e) {
          Info(g_logger, "Unexpected error in compiler thread: {}", e.what());
        } catch (...) {
          Info(g_logger, "Unexpected error in compiler thread");
        }
        Info(g_logger, "Compile thread terminated");
      });
    }

    void stop()
    {
      if (!is_running())
        throw std::runtime_error("Compiler thread not running");

      terminate_flag = 1;
      cond.notify_one();
      thread.join();
    }

    bool is_running() const noexcept
    {
      return thread.joinable();
    }

    void compile(
      const std::shared_ptr<const node_data_snapshot>& snapshot,
      const node_definition_store& decl)
    {
      if (!is_running())
        throw std::runtime_error("Compiler thread not running");

      std::unique_lock lck {mtx};
      queue.push({snapshot, decl});
      cond.notify_one();
    }

    auto get_last_result() const -> std::shared_ptr<compile_result>
    {
      return std::atomic_load(&result);
    }
  };

  node_compiler_thread::node_compiler_thread()
    : m_pimpl {std::make_unique<impl>()}
  {
  }

  node_compiler_thread::~node_compiler_thread() noexcept
  {
  }

  void node_compiler_thread::start()
  {
     m_pimpl->start();
  }

  void node_compiler_thread::stop()
  {
    m_pimpl->stop();
  }

  bool node_compiler_thread::is_running() const noexcept
  {
    return m_pimpl->is_running();
  }

  void node_compiler_thread::compile(
    const std::shared_ptr<const node_data_snapshot>& snapshot,
    const node_definition_store& decl)
  {
    m_pimpl->compile(snapshot, decl);
  }

  auto node_compiler_thread::get_last_result() const
    -> std::shared_ptr<compile_result>
  {
    return m_pimpl->get_last_result();
  }

} // namespace yave::app