//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/backend/backend.hpp>
#include <yave/support/log.hpp>

namespace {

  std::shared_ptr<spdlog::logger> g_logger;

  void init_logger()
  {
    [[maybe_unused]] static auto init = [] {
      g_logger = yave::add_logger("backend");
      return 1;
    }();
  }
} // namespace

namespace yave {

  backend::backend(const object_ptr<const BackendInfo>& info)
    : m_backend_info {info}
  {
    init_logger();
  }

  void backend::init(const scene_config& config)
  {
    if (m_initialized) {
      Info(g_logger, "backend::init(): backend already initialized, ignored.");
      return;
    }

    m_instance_id = m_backend_info->init(config);

    if (m_instance_id == uid()) {
      Error(g_logger, "backend::init(): Failed to initialize backend");
      throw std::runtime_error("Failed to initialized backend");
    }

    m_initialized = true;
  }

  void backend::deinit()
  {
    if (!m_initialized) {
      Info(g_logger, "backend::deinit(): Not initialized, ignored.");
      return;
    }
    m_backend_info->deinit(m_instance_id);
    m_initialized = false;
  }

  void backend::update(const scene_config& config)
  {
    if (!m_initialized) {
      Warning(g_logger, "backend::update(): Backend not initialized, ignored.");
      return;
    }
    if (!m_backend_info->update(m_instance_id, config)) {
      Error(g_logger, "Failed to update beckend's scene config");
    }
  }

  bool backend::initialized() const
  {
    return m_initialized;
  }

  auto backend::get_binds() const -> bind_info_manager
  {
    if (!m_initialized) {
      Error(g_logger, "backend::get_binds(): Backend not initialized");
      throw std::runtime_error("Failed to get bind info list from backend");
    }

    /// get binds
    auto binds = m_backend_info->get_binds(m_instance_id);

    if (binds->info_list.empty()) {
      Error(g_logger, "backend::get_binds(): Failed to get backend info list");
      throw std::runtime_error("Failed to get bind info list from backend");
    }

    // add to bim
    bind_info_manager bim;
    for (auto&& info : binds->info_list) {
      if (!bim.add(info->bind_info())) {
        Error(
          g_logger, "backend::get_binds(): Failed to add bind info to manager");
        throw std::runtime_error("Failed to get bind info from backend");
      }
    }

    return bim;
  }

  auto backend::get_config() const -> scene_config
  {
    if (!m_initialized) {
      Error(g_logger, "backend::get_config(): Backend not initialized");
      throw std::runtime_error("Failed to get scene config from backend");
    }
    return *m_backend_info->get_config(m_instance_id);
  }

  auto backend::instance_id() const -> uid
  {
    return m_instance_id;
  }

  auto backend::name() const -> std::string
  {
    return m_backend_info->name();
  }

  auto backend::backend_id() const -> uuid
  {
    return m_backend_info->backend_id();
  }
}