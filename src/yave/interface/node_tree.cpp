//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/interface/node_tree.hpp>
#include <yave/interface/error_list_wrapper.hpp>
#include <yave/interface/root_manager.hpp>

namespace yave {

  struct node_tree::impl
  {
    // node graph
    node_graph ng;
    // node info
    node_info_manager nim;
    // bind info
    bind_info_manager bim;
    // errors
    error_list_wrapper errs;
    // warnings
    error_list_wrapper wrns;

    // root manager
    root_manager rm = {ng, bim};

    // parser
    node_parser parser = {ng, bim};
    // parsed node
    parsed_node_graph parsed;
    // compiler
    node_compiler compiler = {parsed};

    void init();

    bool register_node_info(const node_info& info);
    void unregister_node_info(const node_info& name);
    bool register_bind_info(const bind_info& info);
    void unregister_bind_info(const bind_info&);

    bool exists(const node_handle& node) const;
    bool exists(const connection_handle& connection) const;

    std::vector<connection_handle> connections() const;
    std::vector<connection_handle> connections(const node_handle& node) const;
    std::vector<connection_handle>
      connections(const node_handle& node, const std::string& socket) const;

    std::vector<connection_handle> input_connections() const;
    std::vector<connection_handle>
      input_connections(const node_handle& node) const;
    std::vector<connection_handle> input_connections(
      const node_handle& node,
      const std::string& socket) const;

    std::vector<connection_handle> output_connections() const;
    std::vector<connection_handle>
      output_connections(const node_handle& node) const;
    std::vector<connection_handle> output_connections(
      const node_handle& node,
      const std::string& socket) const;

    node_handle create(const std::string& name);
    void destroy(const node_handle& handle);

    connection_handle connect(
      const node_handle& src_n,
      const std::string& src_s,
      const node_handle& dst_n,
      const std::string& dst_s);
    void disconnect(const connection_handle& handle);

    std::vector<node_handle> nodes() const;

    std::optional<node_info> get_info(const node_handle& handle) const;
    std::optional<connection_info>
      get_info(const connection_handle& handle) const;

    void run_check();
    error_list get_errors();
    error_list get_warnings();

    void clear();
  };

  node_tree::node_tree()
  {
    m_pimpl = std::make_unique<impl>();
    m_pimpl->init();
  }

  node_tree::node_tree(node_tree&& other)
  {
    m_pimpl = std::move(other.m_pimpl);
  }

  node_tree& node_tree::operator=(node_tree&& other)
  {
    m_pimpl = std::move(other.m_pimpl);
    return *this;
  }

  void node_tree::impl::init()
  {
    auto lck1 = nim.lock();
    auto lck2 = bim.lock();
    auto lck3 = ng.lock();

    /* Add primitive info */

    for (auto&& info : get_primitive_info_list()) {
      [[maybe_unused]] auto succ = nim.add(info);
    }

    for (auto&& info : get_primitive_bind_info_list()) {
      [[maybe_unused]] auto succ = bim.add(info);
    }
  }

  /* register_node_info */

  bool node_tree::impl::register_node_info(const node_info& info)
  {
    auto lck = nim.lock();
    return nim.add(info);
  }

  bool node_tree::register_node_info(const node_info& info)
  {
    return m_pimpl->register_node_info(info);
  }

  /* unregister_node_info */

  void node_tree::impl::unregister_node_info(const node_info& info)
  {
    auto lck = nim.lock();
    return nim.remove(info);
  }

  void node_tree::unregister_node_info(const node_info& info)
  {
    return m_pimpl->unregister_node_info(info);
  }

  /* register_bind_info */

  bool node_tree::impl::register_bind_info(const bind_info& info)
  {
    auto lock = bim.lock();
    return bim.add(info);
  }

  bool node_tree::register_bind_info(const bind_info& info)
  {
    return m_pimpl->register_bind_info(info);
  }

  /* unregister_bind_info */

  void node_tree::impl::unregister_bind_info(const bind_info& info)
  {
    auto lock = bim.lock();
    return bim.remove(info);
  }

  void node_tree::unregister_bind_info(const bind_info& info)
  {
    return m_pimpl->unregister_bind_info(info);
  }

  /* exists */

  bool node_tree::impl::exists(const node_handle& node) const
  {
    auto lck = ng.lock();
    return ng.exists(node);
  }
  bool node_tree::exists(const node_handle& node) const
  {
    return m_pimpl->exists(node);
  }

  /* exists */

  bool node_tree::impl::exists(const connection_handle& connection) const
  {
    auto lck = ng.lock();
    return ng.exists(connection);
  }

  bool node_tree::exists(const connection_handle& connection) const
  {
    return m_pimpl->exists(connection);
  }

  /* connections */

  std::vector<connection_handle> node_tree::impl::connections() const
  {
    auto lck = ng.lock();
    return ng.connections();
  }

  std::vector<connection_handle> node_tree::connections() const
  {
    return m_pimpl->connections();
  }

  /* connections */

  std::vector<connection_handle>
    node_tree::impl::connections(const node_handle& node) const
  {
    auto lck = ng.lock();
    return ng.connections(node);
  }

  std::vector<connection_handle>
    node_tree::connections(const node_handle& node) const
  {
    return m_pimpl->connections(node);
  }

  /* connections */

  std::vector<connection_handle> node_tree::impl::connections(
    const node_handle& node,
    const std::string& socket) const
  {
    auto lck = ng.lock();
    return ng.connections(node, socket);
  }

  std::vector<connection_handle> node_tree::connections(
    const node_handle& node,
    const std::string& socket) const
  {
    return m_pimpl->connections(node, socket);
  }

  /* input_connections */

  std::vector<connection_handle> node_tree::impl::input_connections() const
  {
    auto lck = ng.lock();
    return ng.input_connections();
  }

  std::vector<connection_handle> node_tree::input_connections() const
  {
    return m_pimpl->input_connections();
  }

  /* input_connections */

  std::vector<connection_handle>
    node_tree::impl::input_connections(const node_handle& node) const
  {
    auto lck = ng.lock();
    return ng.input_connections(node);
  }

  std::vector<connection_handle>
    node_tree::input_connections(const node_handle& node) const
  {
    return m_pimpl->input_connections(node);
  }

  /* input_connections */

  std::vector<connection_handle> node_tree::impl::input_connections(
    const node_handle& node,
    const std::string& socket) const
  {
    auto lck = ng.lock();
    return ng.input_connections(node, socket);
  }

  std::vector<connection_handle> node_tree::input_connections(
    const node_handle& node,
    const std::string& socket) const
  {
    return m_pimpl->input_connections(node, socket);
  }

  /* output_connections */

  std::vector<connection_handle> node_tree::impl::output_connections() const
  {
    auto lck = ng.lock();
    return ng.output_connections();
  }

  std::vector<connection_handle> node_tree::output_connections() const
  {
    return m_pimpl->output_connections();
  }

  /* output_connections */

  std::vector<connection_handle>
    node_tree::impl::output_connections(const node_handle& node) const
  {
    auto lck = ng.lock();
    return ng.output_connections(node);
  }

  std::vector<connection_handle>
    node_tree::output_connections(const node_handle& node) const
  {
    return m_pimpl->output_connections(node);
  }

  /* output_connections */

  std::vector<connection_handle> node_tree::impl::output_connections(
    const node_handle& node,
    const std::string& socket) const
  {
    auto lck = ng.lock();
    return ng.output_connections(node, socket);
  }

  std::vector<connection_handle> node_tree::output_connections(
    const node_handle& node,
    const std::string& socket) const
  {
    return m_pimpl->output_connections(node, socket);
  }

  /* create */

  node_handle node_tree::impl::create(const std::string& name)
  {
    auto lck1 = ng.lock();
    auto lck2 = bim.lock();
    if (auto info = nim.find(name)) {
      return ng.add(*info);
    }
    return nullptr;
  }

  node_handle node_tree::create(const std::string& name)
  {
    return m_pimpl->create(name);
  }

  /* destroy */

  void node_tree::impl::destroy(const node_handle& handle)
  {
    auto lck = ng.lock();
    return ng.remove(handle);
  }

  void node_tree::destroy(const node_handle& handle)
  {
    return m_pimpl->destroy(handle);
  }

  /* connect */

  connection_handle node_tree::impl::connect(
    const node_handle& src_n,
    const std::string& src_s,
    const node_handle& dst_n,
    const std::string& dst_s)
  {
    auto lock = ng.lock();
    return ng.connect(src_n, src_s, dst_n, dst_s);
  }

  connection_handle node_tree::connect(
    const node_handle& src_n,
    const std::string& src_s,
    const node_handle& dst_n,
    const std::string& dst_s)
  {
    return m_pimpl->connect(src_n, src_s, dst_n, dst_s);
  }

  /* disconnect */

  void node_tree::impl::disconnect(const connection_handle& handle)
  {
    auto lck = ng.lock();
    return ng.disconnect(handle);
  }

  void node_tree::disconnect(const connection_handle& handle)
  {
    return m_pimpl->disconnect(handle);
  }

  /* nodes */

  std::vector<node_handle> node_tree::impl::nodes() const
  {
    auto lck = ng.lock();
    return ng.nodes();
  }

  std::vector<node_handle> node_tree::nodes() const
  {
    return m_pimpl->nodes();
  }

  /* get_info */

  std::optional<node_info>
    node_tree::impl::get_info(const node_handle& handle) const
  {
    auto lck = ng.lock();
    return ng.get_info(handle);
  }

  std::optional<node_info> node_tree::get_info(const node_handle& handle) const
  {
    return m_pimpl->get_info(handle);
  }

  /* get_info */

  std::optional<connection_info>
    node_tree::impl::get_info(const connection_handle& handle) const
  {
    auto lck = ng.lock();
    return ng.get_info(handle);
  }

  std::optional<connection_info>
    node_tree::get_info(const connection_handle& handle) const
  {
    return m_pimpl->get_info(handle);
  }

  /* run_check */

  void node_tree::impl::run_check()
  {
    auto lck_ng = ng.lock();
    auto lck_parser = parser.lock();

    node_handle root;
    // fist stage
    {
      auto [b, e] =
        parser.parse_prime_tree(root, ng.input_sockets(root).front());

      if (!b) {
        auto lck_errs = errs.lock();
        auto lck_wrns = wrns.lock();
        errs.set_move(std::move(e));
        wrns.set_move({});
        return;
      }
    }

    // second stage
    {
      socket_instance_manager sim;
      auto [pg, e] =
        parser.type_prime_tree(root, ng.input_sockets(root).front(), sim);

        auto lck_errs = errs.lock();
        auto ock_wrns = wrns.lock();
        if (!pg) {
          errs.set_move(std::move(e));
          wrns.set_move({});
          return;
        } else {
          auto lck_parsed = parsed.lock();
          parsed          = std::move(*pg);
          errs.set_move({});
          wrns.set_move({});
        }
    }
  }

  void node_tree::run_check()
  {
    return m_pimpl->run_check();
  }

  /* get_errors */

  error_list node_tree::impl::get_errors()
  {
    auto lck = errs.lock();
    return errs.get_copy();
  }

  error_list node_tree::get_errors()
  {
    return m_pimpl->get_errors();
  }

  /* get_warnings */

  error_list node_tree::impl::get_warnings()
  {
    auto lck = wrns.lock();
    return wrns.get_copy();
  }

  error_list node_tree::get_warnings()
  {
    return m_pimpl->get_warnings();
  }

  /* clear */

  void node_tree::impl::clear()
  {
    auto lck1 = ng.lock();
    auto lck2 = nim.lock();
    auto lck3 = bim.lock();

    ng.clear();
    nim.clear();
    bim.clear();
  }

  void node_tree::clear()
  {
    m_pimpl->clear();
    m_pimpl->init();
  }
} // namespace yave