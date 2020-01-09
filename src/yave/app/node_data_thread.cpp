//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/app/node_data_thread.hpp>
#include <yave/support/log.hpp>

#include <range/v3/view.hpp>
using namespace ranges;

YAVE_DECL_G_LOGGER(node_data_thread)

namespace yave {

  void node_data_thread_op_nop::exec(managed_node_graph&) const noexcept
  {
  }

  void node_data_thread_op_list::exec(managed_node_graph& g) const
  {
    for (auto&& op : ops)
      std::visit(overloaded {[&](auto&& x) { x.exec(g); }}, op);
  }

  void node_data_thread_op_create::exec(managed_node_graph& g) const
  {
    auto p = g.node(parent_group.id());
    (void)g.create(p, name);
  }

  void node_data_thread_op_destroy::exec(managed_node_graph& g) const
  {
    auto n = g.node(node.id());
    g.destroy(n);
  }

  void node_data_thread_op_connect::exec(managed_node_graph& g) const
  {
    auto src = g.socket(src_socket.id());
    auto dst = g.socket(dst_socket.id());
    (void)g.connect(src, dst);
  }

  void node_data_thread_op_disconnect::exec(managed_node_graph& g) const
  {
    auto c = g.connection(connection.id());
    g.disconnect(c);
  }

  void node_data_thread_op_group::exec(managed_node_graph& g) const
  {
    auto p  = g.node(parent_group.id());
    auto ns = nodes //
              | views::transform([&](auto&& n) { return g.node(n.id()); })
              | to_vector;

    g.group(p, ns);
  }

  void node_data_thread_op_ungroup::exec(managed_node_graph& g) const
  {
    auto n = g.node(group.id());
    g.ungroup(n);
  }

  void node_data_thread_op_add_group_input::exec(managed_node_graph& g) const
  {
    auto n = g.node(group.id());
    g.add_group_input_socket(n, socket, index);
  }

  void node_data_thread_op_add_group_output::exec(managed_node_graph& g) const
  {
    auto n = g.node(group.id());
    g.add_group_output_socket(n, socket, index);
  }

  void node_data_thread_op_set_group_input::exec(managed_node_graph& g) const
  {
    auto n = g.node(group.id());
    g.set_group_input_socket(n, socket, index);
  }

  void node_data_thread_op_set_group_output::exec(managed_node_graph& g) const
  {
    auto n = g.node(group.id());
    g.set_group_output_socket(n, socket, index);
  }

  void node_data_thread_op_remove_group_input::exec(managed_node_graph& g) const
  {
    auto n = g.node(group.id());
    g.remove_group_input_socket(n, index);
  }

  void node_data_thread_op_remove_group_output::exec(
    managed_node_graph& g) const
  {
    auto n = g.node(group.id());
    g.remove_group_output_socket(n, index);
  }

  void node_data_thread_op_set_pos::exec(managed_node_graph& g) const
  {
    auto n = g.node(node.id());
    g.set_pos(n, new_pos);
  }

  void node_data_thread_op_set_data::exec(managed_node_graph& g) const
  {
    auto n = g.node(node.id());
    g.set_data(n, data);
  }

  struct node_data_thread::_queue_data
  {
    /// node data operation
    node_data_thread_op op;
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

  void node_data_thread::send(node_data_thread_op op)
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
    send(node_data_thread_op_nop {});
    m_thread.join();
  }

  void node_data_thread::start()
  {
    if (is_running())
      throw std::runtime_error("Data thread already running");

    Info(g_logger, "Starting data thread");

    m_thread = std::thread([&]() {
      while (m_terminate_flag == 0) {
        // wait queue
        std::unique_lock lck {m_mtx};
        m_cond.wait(lck, [&] { return !m_queue.empty(); });
        // pop
        auto top = std::move(m_queue.front());
        m_queue.pop();

        try {
          // dispatch commands
          std::visit(overloaded {[&](auto&& x) { x.exec(*m_graph); }}, top.op);
          // update snapshot atomically
          std::atomic_store(
            &m_snapshot,
            std::make_shared<const node_data_snapshot>(
              m_graph->clone(),
              top.request_time,
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