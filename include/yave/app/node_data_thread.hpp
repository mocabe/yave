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

namespace yave::app {

  /// op nop
  struct node_data_thread_op_nop
  {
    void exec(managed_node_graph&) const noexcept;
  };

  /// op list
  struct node_data_thread_op_list;

  /// op create
  struct node_data_thread_op_create
  {
    node_handle parent_group;
    std::string name;

    void exec(managed_node_graph& g) const;
  };

  /// op destroy
  struct node_data_thread_op_destroy
  {
    node_handle node;

    void exec(managed_node_graph& g) const;
  };

  /// op connect
  struct node_data_thread_op_connect
  {
    socket_handle src_socket;
    socket_handle dst_socket;

    void exec(managed_node_graph& g) const;
  };

  /// op disconnect
  struct node_data_thread_op_disconnect
  {
    connection_handle connection;

    void exec(managed_node_graph& g) const;
  };

  //// op group
  struct node_data_thread_op_group
  {
    node_handle parent_group;
    std::vector<node_handle> nodes;

    void exec(managed_node_graph& g) const;
  };

  /// op ungroup
  struct node_data_thread_op_ungroup
  {
    node_handle group;

    void exec(managed_node_graph& g) const;
  };

  /// op add group input socket
  struct node_data_thread_op_add_group_input
  {
    node_handle group;
    std::string socket;
    size_t index = -1;

    void exec(managed_node_graph& g) const;
  };

  /// op add add group output socket
  struct node_data_thread_op_add_group_output
  {
    node_handle group;
    std::string socket;
    size_t index = -1;

    void exec(managed_node_graph& g) const;
  };

  /// op set group input socket
  struct node_data_thread_op_set_group_input
  {
    node_handle group;
    std::string socket;
    size_t index;

    void exec(managed_node_graph& g) const;
  };

  /// op set group output socket
  struct node_data_thread_op_set_group_output
  {
    node_handle group;
    std::string socket;
    size_t index;

    void exec(managed_node_graph& g) const;
  };

  /// op remove group input socket
  struct node_data_thread_op_remove_group_input
  {
    node_handle group;
    size_t index;

    void exec(managed_node_graph& g) const;
  };

  /// op remove group output socket
  struct node_data_thread_op_remove_group_output
  {
    node_handle group;
    size_t index;

    void exec(managed_node_graph& g) const;
  };

  /// op set node pos
  struct node_data_thread_op_set_pos
  {
    node_handle node;
    tvec2<float> new_pos;

    void exec(managed_node_graph& g) const;
  };

  /// op set data
  struct node_data_thread_op_set_data
  {
    node_handle node;
    object_ptr<Object> data;

    void exec(managed_node_graph& g) const;
  };

  /// Node data thread operations
  using node_data_thread_op = std::variant<
    node_data_thread_op_nop,
    node_data_thread_op_list,
    node_data_thread_op_create,
    node_data_thread_op_destroy,
    node_data_thread_op_connect,
    node_data_thread_op_disconnect,
    node_data_thread_op_group,
    node_data_thread_op_ungroup,
    node_data_thread_op_add_group_input,
    node_data_thread_op_add_group_output,
    node_data_thread_op_set_group_input,
    node_data_thread_op_set_group_output,
    node_data_thread_op_remove_group_input,
    node_data_thread_op_remove_group_output,
    node_data_thread_op_set_pos,
    node_data_thread_op_set_data>;

  struct node_data_thread_op_list
  {
    std::vector<node_data_thread_op> ops;

    void exec(managed_node_graph& g) const;
  };

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

    node_data_thread(const node_data_thread_op&) = delete;
    node_data_thread(node_data_thread_op&&)      = delete;

    /// Send operation
    /// \requires is_running()
    void send(node_data_thread_op op);

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