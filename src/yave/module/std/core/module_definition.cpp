//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/module/std/core/config.hpp>
#include <yave/module/std/core/module_definition.hpp>

// module defs
#include <yave/module/std/core/def/frame_time.hpp>
#include <yave/module/std/core/def/time_transform.hpp>
#include <yave/module/std/core/def/list.hpp>
#include <yave/module/std/core/def/primitive.hpp>
#include <yave/module/std/core/def/if.hpp>

#include <yave/support/log.hpp>
#include <yave/support/id.hpp>

#include <optional>

YAVE_DECL_G_LOGGER(modules::std::core)

namespace yave::modules::_std::core {

  struct module::impl
  {
    uid id;
    bool initialized;
    std::optional<scene_config> config;
  };

  module::module()
  {
    init_logger();

    m_pimpl     = std::make_unique<impl>();
    m_pimpl->id = uid::random_generate();
  }

  module::~module() noexcept
  {
  }

  void module::init(const scene_config& config)
  {
    if (m_pimpl->initialized) {
      Error(g_logger, "Failed to initialize module: Already initialized");
      throw std::runtime_error("module: Already initialized");
    }

    m_pimpl->config      = config;
    m_pimpl->initialized = true;
  }

  void module::deinit()
  {
    // release resources
    m_pimpl->config      = std::nullopt;
    m_pimpl->initialized = false;
  }

  void module::update(const scene_config& config)
  {
    m_pimpl->config = config;
  }

  bool module::initialized() const
  {
    return true;
  }

  auto module::get_node_declarations() const -> std::vector<node_declaration>
  {
    return {get_node_declaration<node::FrameTime>(),
            get_node_declaration<node::TimeTransform>(),
            get_node_declaration<node::ListNil>(),
            get_node_declaration<node::ListCons>(),
            get_node_declaration<node::ListHead>(),
            get_node_declaration<node::ListTail>(),
            get_node_declaration<node::Int>(),
            get_node_declaration<node::Float>(),
            get_node_declaration<node::Bool>(),
            get_node_declaration<node::String>(),
            get_node_declaration<node::If>()};
  }

  auto module::get_node_definitions() const -> std::vector<node_definition>
  {
    std::vector<node_definition> ret;

    // helper
    auto add = [&](auto&& defs) {
      for (auto&& def : defs)
        ret.push_back(def);
    };

    add(yave::get_node_definitions<node::FrameTime, _std::core::tag>());
    add(yave::get_node_definitions<node::TimeTransform, _std::core::tag>());
    add(yave::get_node_definitions<node::ListNil, _std::core::tag>());
    add(yave::get_node_definitions<node::ListCons, _std::core::tag>());
    add(yave::get_node_definitions<node::ListHead, _std::core::tag>());
    add(yave::get_node_definitions<node::ListTail, _std::core::tag>());
    add(yave::get_node_definitions<node::Int, _std::core::tag>());
    add(yave::get_node_definitions<node::Float, _std::core::tag>());
    add(yave::get_node_definitions<node::Bool, _std::core::tag>());
    add(yave::get_node_definitions<node::String, _std::core::tag>());
    add(yave::get_node_definitions<node::If, _std::core::tag>());

    return ret;
  }

  auto module::get_scene_config() const -> scene_config
  {
    return *m_pimpl->config;
  }

  auto module::instance_id() const -> uid
  {
    return m_pimpl->id;
  }

  auto module::name() const -> std::string
  {
    return "std::core";
  }

  auto module::module_id() const -> uuid
  {
    return _std::core::module_id;
  }

} // namespace yave::modules::_std::core