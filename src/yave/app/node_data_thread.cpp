//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/app/node_data_thread.hpp>
#include <yave/support/log.hpp>
#include <yave/support/overloaded.hpp>

#include <range/v3/view.hpp>
using namespace ranges;

YAVE_DECL_G_LOGGER(node_data_thread)

namespace yave::app {

  struct node_data_thread::_queue_data
  {
    /// node data operation
    std::function<void(managed_node_graph&)> op;
    /// request timestamp
    std::chrono::time_point<std::chrono::steady_clock> request_time;
  };

  node_data_thread::node_data_thread(std::shared_ptr<managed_node_graph> graph)
    : m_graph {std::move(graph)}
    , m_terminate_flag {0}
  {
    init_logger();

    // initial snapshot
    m_snapshot = std::make_shared<node_data_snapshot>(
      m_graph->clone(),
      std::chrono::steady_clock::now(),
      std::chrono::steady_clock::now());
  }

  node_data_thread::~node_data_thread() noexcept
  {
    if (is_running())
      stop();
  }

  bool node_data_thread::is_running() const noexcept
  {
    return m_thread.joinable();
  }

  void node_data_thread::send(std::function<void(managed_node_graph&)> op)
  {
    {
      std::unique_lock lck {m_mtx};
      m_queue.push(
        _queue_data {std::move(op), std::chrono::steady_clock::now()});
    }
    m_cond.notify_one();
  }

  void node_data_thread::stop()
  {
    if (!is_running())
      throw std::runtime_error("Data thread not running");

    m_terminate_flag = 1;
    send([](managed_node_graph&) {});
    m_thread.join();
  }

  void node_data_thread::start()
  {
    if (is_running())
      throw std::runtime_error("Data thread already running");

    Info(g_logger, "Starting data thread");

    m_thread = std::thread([&]() {
      // check terminate flag every loop
      while (m_terminate_flag == 0) {

        // wait queue
        std::unique_lock lck {m_mtx};
        m_cond.wait(lck, [&] { return !m_queue.empty(); });

        try {

          auto start = std::chrono::steady_clock::now();

          // execute current commands in queue
          std::chrono::steady_clock::time_point request_time;
          while (!m_queue.empty()) {
            // pop
            auto top = std::move(m_queue.front());
            m_queue.pop();

            // execute commands
            top.op(*m_graph);

            // std::this_thread::sleep_for(std::chrono::milliseconds(5));
            // update time stamp
            request_time = top.request_time;

            // force update after 16ms
            if (
              std::chrono::steady_clock::now() - start
              > std::chrono::milliseconds(16))
              break;
          }

          // update snapshot atomically
          std::atomic_store(
            &m_snapshot,
            std::make_shared<const node_data_snapshot>(
              m_graph->clone(),
              request_time,
              std::chrono::steady_clock::now()));

          // notify update for waiting threads
          m_notify_cond.notify_all();
        } catch (const std::exception& e) {
          Info(
            g_logger,
            "Detected exception ({}) in data thread, recover from snapshot.",
            e.what());
          *m_graph = m_snapshot->graph.clone();
        } catch (...) {
          Info(
            g_logger,
            "Detected exception in data thread, recover from snapshot.");
          *m_graph = m_snapshot->graph.clone();
        }
      }
      Info(g_logger, "Data thread terminated");
    });
  }

  auto node_data_thread::snapshot() const noexcept
    -> std::shared_ptr<const node_data_snapshot>
  {
    return std::atomic_load(&m_snapshot);
  }

  auto node_data_thread::wait_update(
    std::chrono::time_point<std::chrono::steady_clock>
      min_request_time) noexcept -> std::shared_ptr<const node_data_snapshot>
  {
    {
      std::unique_lock lck {m_notify_mtx};
      m_notify_cond.wait(
        lck, [&] { return snapshot()->request_time > min_request_time; });
    }
    return snapshot();
  }
} // namespace yave