//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/module/std/core/config.hpp>
#include <yave/module/std/core/module_definition.hpp>

// module defs
#include <yave/module/std/core/def/time.hpp>
#include <yave/module/std/core/def/transform.hpp>
#include <yave/module/std/core/def/set.hpp>
#include <yave/module/std/core/def/translate.hpp>
#include <yave/module/std/core/def/rotate.hpp>
#include <yave/module/std/core/def/list.hpp>
#include <yave/module/std/core/def/primitive.hpp>
#include <yave/module/std/core/def/if.hpp>
#include <yave/module/std/core/def/vec.hpp>
#include <yave/module/std/core/def/bind.hpp>

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
    return {get_node_declaration<node::Time>(),
            get_node_declaration<node::Transform>(),
            get_node_declaration<node::SetTime>(),
            get_node_declaration<node::SetTransform>(),
            get_node_declaration<node::Translate>(),
            get_node_declaration<node::Rotate>(),
            get_node_declaration<node::RotateX>(),
            get_node_declaration<node::RotateY>(),
            get_node_declaration<node::RotateZ>(),
            get_node_declaration<node::ListNil>(),
            get_node_declaration<node::ListCons>(),
            get_node_declaration<node::ListDecompose>(),
            get_node_declaration<node::Int>(),
            get_node_declaration<node::Float>(),
            get_node_declaration<node::Bool>(),
            get_node_declaration<node::String>(),
            get_node_declaration<node::If>(),
            get_node_declaration<node::Vec2>(),
            get_node_declaration<node::Vec3>(),
            get_node_declaration<node::Vec4>(),
            get_node_declaration<node::Bind>()};
  }

  auto module::get_node_definitions() const -> std::vector<node_definition>
  {
    std::vector<node_definition> ret;

    // helper
    auto add = [&](auto&& defs) {
      for (auto&& def : defs)
        ret.push_back(def);
    };

    add(yave::get_node_definitions<node::Time, _std::core::tag>());
    add(yave::get_node_definitions<node::Transform, _std::core::tag>());
    add(yave::get_node_definitions<node::SetTime, _std::core::tag>());
    add(yave::get_node_definitions<node::SetTransform, _std::core::tag>());
    add(yave::get_node_definitions<node::Translate, _std::core::tag>());
    add(yave::get_node_definitions<node::Rotate, _std::core::tag>());
    add(yave::get_node_definitions<node::RotateX, _std::core::tag>());
    add(yave::get_node_definitions<node::RotateY, _std::core::tag>());
    add(yave::get_node_definitions<node::RotateZ, _std::core::tag>());
    add(yave::get_node_definitions<node::ListNil, _std::core::tag>());
    add(yave::get_node_definitions<node::ListCons, _std::core::tag>());
    add(yave::get_node_definitions<node::ListDecompose, _std::core::tag>());
    add(yave::get_node_definitions<node::Int, _std::core::tag>());
    add(yave::get_node_definitions<node::Float, _std::core::tag>());
    add(yave::get_node_definitions<node::Bool, _std::core::tag>());
    add(yave::get_node_definitions<node::String, _std::core::tag>());
    add(yave::get_node_definitions<node::If, _std::core::tag>());
    add(yave::get_node_definitions<node::Vec2, _std::core::tag>());
    add(yave::get_node_definitions<node::Vec3, _std::core::tag>());
    add(yave::get_node_definitions<node::Vec4, _std::core::tag>());
    add(yave::get_node_definitions<node::Bind, _std::core::tag>());

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