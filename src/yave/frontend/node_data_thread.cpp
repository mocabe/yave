//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/frontend/node_data_thread.hpp>
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

  node_data_thread::node_data_thread(std::shared_ptr<managed_node_graph> graph)
    : m_graph {std::move(graph)}
    , m_terminate_flag {0}
  {
    init_logger();

    // initial snapshot
    m_snapshot = std::make_shared<managed_node_graph>(m_graph->clone());
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
    std::unique_lock lck {m_mtx};
    m_queue.emplace(std::move(op));
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
        // wait
        std::unique_lock lck {m_mtx};
        m_cond.wait(lck, [&] { return !m_queue.empty(); });
        // pop
        auto top = std::move(m_queue.front());
        m_queue.pop();
        // exec
        try {
          // dispatch
          std::visit(overloaded {[&](auto&& op) { op.exec(*m_graph); }}, top);
          // update snapshot
          m_snapshot = std::make_shared<managed_node_graph>(m_graph->clone());
        } catch (const std::exception& e) {
          Info(
            g_logger,
            "Detected exception ({}) in data thread, recover from snapshot.",
            e.what());
          *m_graph = m_snapshot->clone();
        } catch (...) {
          Info(
            g_logger,
            "Detected exception in data thread, recover from snapshot.");
          *m_graph = m_snapshot->clone();
        }
      }
      Info(g_logger, "Data thread terminated");
    });
  }

  auto node_data_thread::snapshot() const noexcept
    -> const std::shared_ptr<const managed_node_graph>&
  {
    assert(m_snapshot);
    return m_snapshot;
  }

} // namespace yave