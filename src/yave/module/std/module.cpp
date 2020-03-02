//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/module/std/config.hpp>
#include <yave/module/std/module.hpp>

// module defs
#include <yave/module/std/def/filesystem/path.hpp>
#include <yave/module/std/def/geometry/vec.hpp>
#include <yave/module/std/def/geometry/rect.hpp>
#include <yave/module/std/def/image/image.hpp>
#include <yave/module/std/def/list/list.hpp>
#include <yave/module/std/def/logic/apply.hpp>
#include <yave/module/std/def/logic/if.hpp>
#include <yave/module/std/def/prim/primitive.hpp>
#include <yave/module/std/def/render/frame.hpp>
#include <yave/module/std/def/time/set_time.hpp>
#include <yave/module/std/def/time/time.hpp>
#include <yave/module/std/def/transform/transform.hpp>
#include <yave/module/std/def/transform/set_transform.hpp>
#include <yave/module/std/def/transform/rotate.hpp>
#include <yave/module/std/def/transform/translate.hpp>

#include <yave/support/log.hpp>
#include <yave/support/id.hpp>

#include <optional>

YAVE_DECL_G_LOGGER(modules::std::core)

namespace yave::modules::_std {

  namespace {
    struct module_resource
    {
      module_resource(const scene_config& config)
        : image_buff {module_id}
        , frame_buff {config.width(),
                      config.height(),
                      config.frame_buffer_format(),
                      module_id}
      {
      }
      image_buffer_manager image_buff;
      frame_buffer_manager frame_buff;
    };
  } // namespace

  struct module::impl
  {
    uid id;
    bool initialized;
    std::optional<scene_config> config;
    std::unique_ptr<module_resource> resource;
  };

  module::module()
  {
    init_logger();

    m_pimpl     = std::make_unique<impl>();
    m_pimpl->id = uid::random_generate();
  }

  module::~module() noexcept
  {
    if (initialized())
      deinit();
  }

  void module::init(const scene_config& config)
  {
    assert(!initialized());

    m_pimpl->config      = config;
    m_pimpl->resource    = std::make_unique<module_resource>(config);
    m_pimpl->initialized = true;
  }

  void module::deinit()
  {
    assert(initialized());
    m_pimpl->config      = std::nullopt;
    m_pimpl->resource    = nullptr;
    m_pimpl->initialized = false;
  }

  void module::update(const scene_config& config)
  {
    m_pimpl->config = config;
  }

  bool module::initialized() const
  {
    return m_pimpl->initialized;
  }

  auto module::get_node_declarations() const -> std::vector<node_declaration>
  {
    auto& fmngr = m_pimpl->resource->frame_buff;

    return {get_node_declaration<node::FilePath>(),
            get_node_declaration<node::Vec2>(),
            get_node_declaration<node::Vec3>(),
            get_node_declaration<node::Vec4>(),
            get_node_declaration<node::Rect2>(),
            get_node_declaration<node::Rect3>(),
            get_node_declaration<node::Image>(),
            get_node_declaration<node::ListNil>(),
            get_node_declaration<node::ListCons>(),
            get_node_declaration<node::ListDecompose>(),
            get_node_declaration<node::Apply>(),
            get_node_declaration<node::If>(),
            get_node_declaration<node::Int>(),
            get_node_declaration<node::Float>(),
            get_node_declaration<node::Bool>(),
            get_node_declaration<node::String>(),
            get_node_declaration<node::Frame>(),
            get_node_declaration<node::SetTime>(),
            get_node_declaration<node::Time>(),
            get_node_declaration<node::Transform>(),
            get_node_declaration<node::SetTransform>(),
            get_node_declaration<node::Translate>(),
            get_node_declaration<node::Rotate>(),
            get_node_declaration<node::RotateX>(),
            get_node_declaration<node::RotateY>(),
            get_node_declaration<node::RotateZ>()};
  }

  auto module::get_node_definitions() const -> std::vector<node_definition>
  {
    std::vector<node_definition> ret;

    // helper
    auto add = [&](auto&& defs) {
      for (auto&& def : defs)
        ret.push_back(def);
    };

    auto& imngr = m_pimpl->resource->image_buff;
    auto& fmngr = m_pimpl->resource->frame_buff;

    add(yave::get_node_definitions<node::Image, _std::tag>(imngr));
    add(yave::get_node_definitions<node::Frame, _std::tag>(fmngr));

    add(yave::get_node_definitions<node::FilePath, _std::tag>());
    add(yave::get_node_definitions<node::Vec2, _std::tag>());
    add(yave::get_node_definitions<node::Vec3, _std::tag>());
    add(yave::get_node_definitions<node::Vec4, _std::tag>());
    add(yave::get_node_definitions<node::Rect2, _std::tag>());
    add(yave::get_node_definitions<node::Rect3, _std::tag>());
    add(yave::get_node_definitions<node::ListNil, _std::tag>());
    add(yave::get_node_definitions<node::ListCons, _std::tag>());
    add(yave::get_node_definitions<node::ListDecompose, _std::tag>());
    add(yave::get_node_definitions<node::If, _std::tag>());
    add(yave::get_node_definitions<node::Apply, _std::tag>());
    add(yave::get_node_definitions<node::Int, _std::tag>());
    add(yave::get_node_definitions<node::Float, _std::tag>());
    add(yave::get_node_definitions<node::Bool, _std::tag>());
    add(yave::get_node_definitions<node::String, _std::tag>());
    add(yave::get_node_definitions<node::Time, _std::tag>());
    add(yave::get_node_definitions<node::SetTime, _std::tag>());
    add(yave::get_node_definitions<node::Transform, _std::tag>());
    add(yave::get_node_definitions<node::SetTransform, _std::tag>());
    add(yave::get_node_definitions<node::Translate, _std::tag>());
    add(yave::get_node_definitions<node::Rotate, _std::tag>());
    add(yave::get_node_definitions<node::RotateX, _std::tag>());
    add(yave::get_node_definitions<node::RotateY, _std::tag>());
    add(yave::get_node_definitions<node::RotateZ, _std::tag>());

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
    return "std";
  }

  auto module::module_id() const -> uuid
  {
    return _std::module_id;
  }

} // namespace yave::modules::_std