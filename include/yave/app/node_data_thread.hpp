//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/managed_node_graph.hpp>

#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <variant>
#include <chrono>
#include <functional>

namespace yave::app {

  struct node_data_snapshot
  {
    node_data_snapshot(
      managed_node_graph&& g,
      std::chrono::time_point<std::chrono::steady_clock> rt,
      std::chrono::time_point<std::chrono::steady_clock> pt) noexcept
      : graph {std::move(g)}
      , request_time {rt}
      , process_time {pt}
    {
    }

    /// node graph
    managed_node_graph graph;
    /// timestamp of request
    std::chrono::time_point<std::chrono::steady_clock> request_time;
    /// timestamp on process
    std::chrono::time_point<std::chrono::steady_clock> process_time;
  };

  /// Single-Write, Single-Read node data thread
  class node_data_thread
  {
  public:
    /// Init data thread
    node_data_thread(std::shared_ptr<managed_node_graph> graph);

    /// Dtor
    ~node_data_thread() noexcept;

    /// Send operation
    /// \requires is_running()
    void send(std::function<void(managed_node_graph&)> function);

    /// Stop running thread
    /// \requres is_running();
    void stop();

    /// Start thread
    /// \requires !is_running();
    void start();

    /// Thread runnig?
    bool is_running() const noexcept;

    /// Get snapshot of node graph
    auto snapshot() const noexcept -> std::shared_ptr<const node_data_snapshot>;

    /// Wait until snapshot update
    auto wait_update(std::chrono::time_point<std::chrono::steady_clock>
                       min_request_time) noexcept
      -> std::shared_ptr<const node_data_snapshot>;

  private:
    std::shared_ptr<managed_node_graph> m_graph;
    std::shared_ptr<const node_data_snapshot> m_snapshot;

  private:
    std::thread m_thread;

  private:
    struct _queue_data;
    std::queue<_queue_data> m_queue;
    std::mutex m_mtx;
    std::condition_variable m_cond;
    std::atomic<int> m_terminate_flag;

  private:
    std::mutex m_notify_mtx;
    std::condition_variable m_notify_cond;
  };
} // namespace yave