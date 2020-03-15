//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/app/node_data_thread.hpp>
#include <yave/support/log.hpp>
#include <yave/support/overloaded.hpp>

#include <range/v3/view.hpp>
using namespace ranges;

#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <variant>

YAVE_DECL_G_LOGGER(node_data_thread)

namespace yave::app {

  class node_data_thread::impl
  {
  public:
    struct queue_data
    {
      /// node data operation
      std::function<void(managed_node_graph&)> op;
      /// request timestamp
      std::chrono::time_point<std::chrono::steady_clock> request_time;
    };

  public:
    std::shared_ptr<managed_node_graph> graph;
    std::shared_ptr<const node_data_snapshot> snapshot;

  public:
    std::thread thread;
    std::queue<queue_data> queue;
    std::mutex m_mtx;
    std::condition_variable cond;
    std::atomic<int> terminate_flag;
    std::mutex notify_mtx;
    std::condition_variable notify_cond;

  public:
    impl(std::shared_ptr<managed_node_graph> g)
      : graph {std::move(g)}
      , terminate_flag {0}
    {
      init_logger();

      // initial snapshot
      snapshot = std::make_shared<node_data_snapshot>(
        graph->clone(),
        std::chrono::steady_clock::now(),
        std::chrono::steady_clock::now());
    }

    ~impl() noexcept
    {
      if (is_running())
        stop();
    }

    void send(std::function<void(managed_node_graph&)>&& op)
    {
      {
        std::unique_lock lck {m_mtx};
        queue.push(
          queue_data {std::move(op), std::chrono::steady_clock::now()});
      }
      cond.notify_one();
    }

    bool is_running() const noexcept
    {
      return thread.joinable();
    }

    void stop()
    {
      if (!is_running())
        throw std::runtime_error("Data thread not running");

      terminate_flag = 1;
      send([](managed_node_graph&) {});
      thread.join();
    }

    void start()
    {
      if (is_running())
        throw std::runtime_error("Data thread already running");

      Info(g_logger, "Starting data thread");

      thread = std::thread([&]() {
        // check terminate flag every loop
        while (terminate_flag == 0) {

          // wait queue
          std::unique_lock lck {m_mtx};
          cond.wait(lck, [&] { return !queue.empty(); });

          try {

            auto start = std::chrono::steady_clock::now();

            // execute current commands in queue
            std::chrono::steady_clock::time_point request_time;
            while (!queue.empty()) {
              // pop
              auto top = std::move(queue.front());
              queue.pop();

              // execute commands
              top.op(*graph);

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
              &snapshot,
              std::make_shared<const node_data_snapshot>(
                graph->clone(),
                request_time,
                std::chrono::steady_clock::now()));

            // notify update for waiting threads
            notify_cond.notify_all();
          } catch (const std::exception& e) {
            Info(
              g_logger,
              "Detected exception ({}) in data thread, recover from snapshot.",
              e.what());
            *graph = snapshot->graph.clone();
          } catch (...) {
            Info(
              g_logger,
              "Detected exception in data thread, recover from snapshot.");
            *graph = snapshot->graph.clone();
          }
        }
        Info(g_logger, "Data thread terminated");
      });
    }

    auto get_snapshot() const noexcept -> std::shared_ptr<const node_data_snapshot>
    {
      return std::atomic_load(&snapshot);
    }

    auto wait_update(std::chrono::time_point<std::chrono::steady_clock>
                       min_request_time) noexcept
      -> std::shared_ptr<const node_data_snapshot>
    {
      {
        std::unique_lock lck {notify_mtx};
        notify_cond.wait(
          lck, [&] { return get_snapshot()->request_time > min_request_time; });
      }
      return get_snapshot();
    }
  };

  node_data_thread::node_data_thread(std::shared_ptr<managed_node_graph> graph)
    : m_pimpl {std::make_unique<impl>(graph)}
  {
  }

  node_data_thread::~node_data_thread() noexcept
  {
  }

  bool node_data_thread::is_running() const noexcept
  {
    return m_pimpl->is_running();
  }

  void node_data_thread::send(std::function<void(managed_node_graph&)> op)
  {
    return m_pimpl->send(std::move(op));
  }

  void node_data_thread::stop()
  {
    m_pimpl->stop();
  }

  void node_data_thread::start()
  {
    m_pimpl->start();
  }

  auto node_data_thread::snapshot() const noexcept
    -> std::shared_ptr<const node_data_snapshot>
  {
    return m_pimpl->get_snapshot();
  }

  auto node_data_thread::wait_update(
    std::chrono::time_point<std::chrono::steady_clock>
      min_request_time) noexcept -> std::shared_ptr<const node_data_snapshot>
  {
    return m_pimpl->wait_update(min_request_time);
  }
} // namespace yave