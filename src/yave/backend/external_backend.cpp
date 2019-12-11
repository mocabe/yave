//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/backend/external_backend.hpp>
#include <yave/support/log.hpp>

YAVE_DECL_G_LOGGER(external_backend)

namespace yave {

  external_backend::external_backend(
    const object_ptr<const ExternalBackendInfo>& info)
    : m_backend_info {info}
  {
    init_logger();
  }

  external_backend::~external_backend()
  {
  }

  void external_backend::init(const scene_config& config)
  {
    if (m_initialized) {
      Info(
        g_logger,
        "external_backend::init(): backend already initialized, ignored.");
      return;
    }

    m_instance_id = m_backend_info->init(config);

    if (m_instance_id == uid()) {
      Error(g_logger, "external_backend::init(): Failed to initialize backend");
      throw std::runtime_error("Failed to initialized backend");
    }

    m_initialized = true;
  }

  void external_backend::deinit()
  {
    if (!m_initialized) {
      Info(g_logger, "external_backend::deinit(): Not initialized, ignored.");
      return;
    }
    m_backend_info->deinit(m_instance_id);
    m_initialized = false;
  }

  void external_backend::update(const scene_config& config)
  {
    if (!m_initialized) {
      Warning(
        g_logger,
        "external_backend::update(): Backend not initialized, ignored.");
      return;
    }
    if (!m_backend_info->update(m_instance_id, config)) {
      Error(g_logger, "Failed to update beckend's scene config");
    }
  }

  bool external_backend::initialized() const
  {
    return m_initialized;
  }

  auto external_backend::get_node_declarations() const
    -> std::vector<node_declaration>
  {
    if (!m_initialized) {
      Error(
        g_logger,
        "external_backend::get_node_declarations(): Backend not initialized");
      throw std::runtime_error("Failed to get declaration list from backend");
    }

    // get declarations
    auto decls = m_backend_info->get_node_declarations(m_instance_id);

    if (!decls) {
      Error(
        g_logger,
        "external_backend::get_node_declarations(): Failed to get backend "
        "declarations");
      throw std::runtime_error("Failed to get declaration list from backend");
    }

    std::vector<node_declaration> ret;
    for (auto&& decl : decls->list) {
      ret.push_back(decl->node_declaration());
    }
    return ret;
  }

  auto external_backend::get_node_definitions() const
    -> std::vector<node_definition>
  {
    if (!m_initialized) {
      Error(
        g_logger,
        "external_backend::get_node_definitions(): Backend not initialized");
      throw std::runtime_error("Failed to get definition list from backend");
    }

    // get definitions
    auto defs = m_backend_info->get_node_definitions(m_instance_id);

    if (!defs) {
      Error(
        g_logger,
        "external_backend::get_node_definitions(): Failed to get definitios");
      throw std::runtime_error("Failed to get definition list from backend");
    }

    std::vector<node_definition> ret;
    for (auto&& def : defs->list) {
      ret.push_back(def->node_definition());
    }
    return ret;
  }

  auto external_backend::get_scene_config() const -> scene_config
  {
    if (!m_initialized) {
      Error(
        g_logger, "external_backend::get_config(): Backend not initialized");
      throw std::runtime_error("Failed to get scene config from backend");
    }
    return *m_backend_info->get_scene_config(m_instance_id);
  }

  auto external_backend::instance_id() const -> uid
  {
    return m_instance_id;
  }

  auto external_backend::name() const -> std::string
  {
    return m_backend_info->name();
  }

  auto external_backend::backend_id() const -> uuid
  {
    return m_backend_info->backend_id();
  }
} // namespace yave