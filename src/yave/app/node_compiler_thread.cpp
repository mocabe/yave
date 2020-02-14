//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/app/node_compiler_thread.hpp>
#include <yave/support/log.hpp>

YAVE_DECL_G_LOGGER(node_compiler_thread)

namespace yave::app {

  struct node_compiler_thread::_queue_data
  {
    std::shared_ptr<const node_data_snapshot> snapshot;
    node_definition_store defs;
  };

  node_compiler_thread::node_compiler_thread()
    : m_terminate_flag {0}
    , m_compile_flag {0}
    , m_result {std::make_shared<compile_result>()}
  {
    init_logger();

    m_result->success  = false;
    m_result->bgn_time = std::chrono::steady_clock::now();
    m_result->end_time = std::chrono::steady_clock::now();
  }

  node_compiler_thread::~node_compiler_thread() noexcept
  {
    if (is_running())
      stop();
  }

  void node_compiler_thread::start()
  {
    if (is_running())
      throw std::runtime_error("Compiler thread already running");

    Info(g_logger, "Starting compiler thread");

    m_thread = std::thread([&]() {
      try {
        while (true) {

          std::unique_lock lck {m_mtx};
          m_cond.wait(lck, [&] { return m_terminate_flag || m_compile_flag; });

          // terminate
          if (m_terminate_flag)
            break;

          // compile
          if (m_compile_flag) {

            m_compile_flag = 0;

            auto start = std::chrono::steady_clock::now();

            assert(!m_queue.empty());

            auto top = std::move(m_queue.front());
            m_queue.pop();

            auto result = std::make_shared<compile_result>();

            // parse
            auto parsed = m_parser.parse(top.snapshot->graph);

            if (!parsed) {
              result->success      = false;
              result->parse_errors = m_parser.get_errors();
              result->bgn_time     = start;
              result->end_time     = std::chrono::steady_clock::now();
              std::atomic_store(&m_result, std::move(result));
              continue;
            } else {
              // compile
              auto exe = m_compiler.compile(std::move(*parsed), top.defs);
              if (!exe) {
                result->success        = false;
                result->compile_errors = m_compiler.get_errors();
                result->bgn_time       = start;
                result->end_time       = std::chrono::steady_clock::now();
                std::atomic_store(&m_result, std::move(result));
                continue;
              } else {
                result->success  = true;
                result->bgn_time = start;
                result->end_time = std::chrono::steady_clock::now();
                result->exe      = std::move(*exe);
                std::atomic_store(&m_result, std::move(result));
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

  void node_compiler_thread::stop()
  {
    if (!is_running())
      throw std::runtime_error("Compiler thread not running");

    m_terminate_flag = 1;
    m_cond.notify_one();
    m_thread.join();
  }

  bool node_compiler_thread::is_running() const noexcept
  {
    return m_thread.joinable();
  }

  void node_compiler_thread::compile(
    const std::shared_ptr<const node_data_snapshot>& snapshot,
    const node_definition_store& decl)
  {
    if (!is_running())
      throw std::runtime_error("Compiler thread not running");

    std::unique_lock lck {m_mtx};

    m_queue.push({snapshot, decl});

    m_compile_flag = 1;
    m_cond.notify_one();
  }

  auto node_compiler_thread::get_last_result() const
    -> std::shared_ptr<compile_result>
  {
    return std::atomic_load(&m_result);
  }

} // namespace yave::app