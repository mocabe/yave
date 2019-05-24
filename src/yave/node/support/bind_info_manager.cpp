//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/support/bind_info_manager.hpp>
#include <yave/support/log.hpp>

namespace {
  // logger
  std::shared_ptr<spdlog::logger> g_bind_mngr_logger;

  // init
  void init_bind_mngr_logger()
  {
    [[maybe_unused]] static auto init_logger = [] {
      g_bind_mngr_logger = yave::add_logger("bind_info_manager");
      return 1;
    }();
  }
} // namespace

namespace yave {

  bind_info_manager::bind_info_manager()
    : m_info {}
    , m_mtx {}
  {
    init_bind_mngr_logger();
  }

  bind_info_manager::bind_info_manager(const std::vector<bind_info>& binds)
  {
    init_bind_mngr_logger();

    for (auto&& b : binds) {
      if (!add(b))
        throw std::runtime_error("Failed to add bind_info on initialization");
    }
  }

  bind_info_manager::bind_info_manager(const bind_info_manager& other)
    : m_info {other.m_info}
    , m_mtx {}
  {
  }

  bind_info_manager::bind_info_manager(bind_info_manager&& other)
    : m_info {std::move(other.m_info)}
    , m_mtx {}
  {
  }

  bind_info_manager& bind_info_manager::
    operator=(const bind_info_manager& other)
  {
    m_info = other.m_info;
    return *this;
  }

  bind_info_manager& bind_info_manager::operator=(bind_info_manager&& other)
  {
    m_info = std::move(other.m_info);
    return *this;
  }

  bool bind_info_manager::add(const bind_info& info)
  {
    auto iter = m_info.emplace(info.name(), std::make_shared<info_type>(info));
    if (iter == m_info.end())
      return false;

    Info(
      g_bind_mngr_logger,
      "Added bind_info: {}(in:{})",
      iter->first,
      iter->second->input_sockets().size());

    return true;
  }

  void bind_info_manager::remove(const std::string& name)
  {
    auto [bgn, end] = m_info.equal_range(name);

    auto iter = bgn;
    while (iter != end) {
      assert(iter->second->name() == name);
      Info(
        g_bind_mngr_logger,
        "Removed bind_info: {}(in:{})",
        iter->first,
        iter->second->input_sockets().size());
      // erase
      m_info.erase(iter++);
    }
  }

  void bind_info_manager::remove(
    const std::string& name,
    const std::vector<std::string>& input,
    const std::string& output)
  {
    auto [bgn, end] = m_info.equal_range(name);

    auto iter = bgn;
    while (iter != end) {
      assert(iter->second->name() == name);
      if (
        iter->second->output_socket() == output &&
        iter->second->input_sockets() == input) {
        Info(
          g_bind_mngr_logger,
          "Removed bind_info: {}(in:{})",
          iter->first,
          iter->second->input_sockets().size());
        // erase
        m_info.erase(iter++);
      } else
        ++iter;
    }
  }

  bool bind_info_manager::exists(const std::string& name) const
  {
    return m_info.find(name) != m_info.end();
  }

  std::vector<std::shared_ptr<const bind_info_manager::info_type>>
    bind_info_manager::enumerate()
  {
    std::vector<std::shared_ptr<const info_type>> ret;
    for (auto&& [name, ptr] : m_info) {
      assert(name == ptr->name());
      ret.push_back(ptr);
    }
    return ret;
  }

  std::vector<std::shared_ptr<const bind_info_manager::info_type>>
    bind_info_manager::find(const std::string& name) const
  {
    auto [bgn, end] = m_info.equal_range(name);

    std::vector<std::shared_ptr<const info_type>> ret;
    for (auto iter = bgn; iter != end; ++iter) {
      ret.push_back(iter->second);
    }
    return ret;
  }

  std::vector<std::shared_ptr<const bind_info_manager::info_type>>
    bind_info_manager::find(
      const std::string& name,
      const std::vector<std::string>& input,
      const std::string& output) const
  {
    auto [bgn, end] = m_info.equal_range(name);

    std::vector<std::shared_ptr<const info_type>> ret;
    for (auto iter = bgn; iter != end; ++iter) {
      if (
        iter->second->output_socket() == output &&
        iter->second->input_sockets() == input)
        ret.push_back(iter->second);
    }
    return ret;
  }

  std::vector<std::shared_ptr<const bind_info_manager::info_type>>
    bind_info_manager::find(const bind_info& info) const
  {
    return find(info.name(), info.input_sockets(), info.output_socket());
  }

  std::vector<std::shared_ptr<const bind_info_manager::info_type>>
    bind_info_manager::get_binds(const node_info& info) const
  {
    return get_binds(info.name(), info.input_sockets(), info.output_sockets());
  }

  std::vector<std::shared_ptr<const bind_info_manager::info_type>>
    bind_info_manager::get_binds(
      const std::string& name,
      const std::vector<std::string>& input_sockets,
      const std::vector<std::string>& output_sockets) const
  {
    auto [bgn, end] = m_info.equal_range(name);

    std::vector<std::shared_ptr<const info_type>> ret;
    for (auto iter = bgn; iter != end; ++iter) {
      if (iter->second->is_bind_of(name, input_sockets, output_sockets))
        ret.push_back(iter->second);
    }
    return ret;
  }

  std::unique_lock<std::mutex> bind_info_manager::lock() const
  {
    return std::unique_lock(m_mtx);
  }

  void bind_info_manager::clear()
  {
    m_info.clear();
  }

} // namespace yave