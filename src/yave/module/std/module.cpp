//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/module/std/config.hpp>
#include <yave/module/std/module.hpp>

// module defs
#include <yave/module/std/color/color.hpp>
#include <yave/module/std/filesystem/path.hpp>
#include <yave/module/std/geometry/vec.hpp>
#include <yave/module/std/geometry/mat.hpp>
#include <yave/module/std/geometry/rect.hpp>
#include <yave/module/std/image/image.hpp>
#include <yave/module/std/list/list.hpp>
#include <yave/module/std/list/algorithm.hpp>
#include <yave/module/std/logic/apply.hpp>
#include <yave/module/std/logic/if.hpp>
#include <yave/module/std/primitive/primitive.hpp>
#include <yave/module/std/render/frame.hpp>
#include <yave/module/std/render/mix.hpp>
#include <yave/module/std/time/retime.hpp>
#include <yave/module/std/time/time.hpp>
#include <yave/module/std/transform/rotate.hpp>
#include <yave/module/std/transform/translate.hpp>
#include <yave/module/std/shape/circle.hpp>
#include <yave/module/std/shape/draw.hpp>
#include <yave/module/std/shape/transform.hpp>

#include <yave/lib/vulkan/offscreen_compositor.hpp>
#include <yave/support/log.hpp>
#include <yave/support/id.hpp>

#include <optional>

YAVE_DECL_G_LOGGER(modules::std::core)

namespace yave::modules::_std {

  namespace {
    struct module_resource
    {
      module_resource(const scene_config& config, vulkan::vulkan_context& ctx)
        : offscreen_ctx {ctx}
        , image_buff {module_id}
        , frame_buff {config.width(), config.height(), config.frame_format(), module_id, offscreen_ctx}
        , compositor {config.width(), config.height(), offscreen_ctx}
      {
      }

      vulkan::offscreen_context offscreen_ctx;

      image_buffer_manager image_buff;
      frame_buffer_manager frame_buff;
      vulkan::rgba32f_offscreen_compositor compositor;
    };
  } // namespace

  struct module::impl
  {
    impl(vulkan::vulkan_context& vulkan_ctx)
      : id {uid::random_generate()}
      , initialized {false}
      , config {std::nullopt}
      , vulkan_ctx {vulkan_ctx}
      , resource {nullptr}
    {
    }

    void init(const scene_config& cfg)
    {
      config      = cfg;
      resource    = std::make_unique<module_resource>(cfg, vulkan_ctx);
      initialized = true;
    }

    void deinit()
    {
      config      = std::nullopt;
      resource    = nullptr;
      initialized = false;
    }

    uid id;
    bool initialized;
    std::optional<scene_config> config;
    vulkan::vulkan_context& vulkan_ctx;
    std::unique_ptr<module_resource> resource;
  };

  module::module(vulkan::vulkan_context& vulkan_ctx)
  {
    init_logger();
    m_pimpl = std::make_unique<impl>(vulkan_ctx);
  }

  module::~module() noexcept
  {
    if (initialized())
      deinit();
  }

  void module::init(const scene_config& config)
  {
    assert(!initialized());
    m_pimpl->init(config);
  }

  void module::deinit()
  {
    assert(initialized());
    m_pimpl->deinit();
  }

  void module::update(const scene_config& new_cfg)
  {
    deinit();
    init(new_cfg);
  }

  bool module::initialized() const
  {
    return m_pimpl->initialized;
  }

  auto module::get_node_declarations() const -> std::vector<node_declaration>
  {
    auto& fmngr    = m_pimpl->resource->frame_buff;

    return {get_node_declaration<node::Color>(),
            get_node_declaration<node::FilePath>(),
            get_node_declaration<node::Vec2>(),
            get_node_declaration<node::Vec3>(),
            get_node_declaration<node::Vec4>(),
            get_node_declaration<node::Mat4>(),
            get_node_declaration<node::Rect2>(),
            get_node_declaration<node::Rect3>(),
            get_node_declaration<node::Image>(),
            get_node_declaration<node::ListNil>(),
            get_node_declaration<node::ListCons>(),
            get_node_declaration<node::ListDecompose>(),
            get_node_declaration<node::ListMap>(),
            get_node_declaration<node::ListRepeat>(),
            get_node_declaration<node::ListEnumerate>(),
            get_node_declaration<node::Apply>(),
            get_node_declaration<node::If>(),
            get_node_declaration<node::Int>(),
            get_node_declaration<node::Float>(),
            get_node_declaration<node::Bool>(),
            get_node_declaration<node::String>(),
            get_node_declaration<node::Frame>(),
            get_node_declaration<node::MixFrame>(),
            get_node_declaration<node::ReTime>(),
            get_node_declaration<node::DelayTime>(),
            get_node_declaration<node::ScaleTime>(),
            get_node_declaration<node::Time>(),
            get_node_declaration<node::Translate>(),
            get_node_declaration<node::Rotate>(),
            get_node_declaration<node::RotateX>(),
            get_node_declaration<node::RotateY>(),
            get_node_declaration<node::RotateZ>(),
            get_node_declaration<node::CircleShape>(),
            get_node_declaration<node::DrawShape>(fmngr),
            get_node_declaration<node::ShapeTranslate>(),
            get_node_declaration<node::ShapeRotate>(),
            get_node_declaration<node::ShapeScale>()};
  }

  auto module::get_node_definitions() const -> std::vector<node_definition>
  {
    std::vector<node_definition> ret;

    // helper
    auto add = [&](auto&& defs) {
      for (auto&& def : defs)
        ret.push_back(def);
    };

    auto& imngr      = m_pimpl->resource->image_buff;
    auto& fmngr      = m_pimpl->resource->frame_buff;
    auto& compositor = m_pimpl->resource->compositor;

    add(yave::get_node_definitions<node::Image, _std::tag>(imngr));
    add(yave::get_node_definitions<node::Frame, _std::tag>(fmngr));

    add(yave::get_node_definitions<node::Color, _std::tag>());
    add(yave::get_node_definitions<node::FilePath, _std::tag>());
    add(yave::get_node_definitions<node::Vec2, _std::tag>());
    add(yave::get_node_definitions<node::Vec3, _std::tag>());
    add(yave::get_node_definitions<node::Vec4, _std::tag>());
    add(yave::get_node_definitions<node::Mat4, _std::tag>());
    add(yave::get_node_definitions<node::Rect2, _std::tag>());
    add(yave::get_node_definitions<node::Rect3, _std::tag>());
    add(yave::get_node_definitions<node::ListNil, _std::tag>());
    add(yave::get_node_definitions<node::ListCons, _std::tag>());
    add(yave::get_node_definitions<node::ListDecompose, _std::tag>());
    add(yave::get_node_definitions<node::ListMap, _std::tag>());
    add(yave::get_node_definitions<node::ListRepeat, _std::tag>());
    add(yave::get_node_definitions<node::ListEnumerate, _std::tag>());
    add(yave::get_node_definitions<node::If, _std::tag>());
    add(yave::get_node_definitions<node::Apply, _std::tag>());
    add(yave::get_node_definitions<node::Int, _std::tag>());
    add(yave::get_node_definitions<node::Float, _std::tag>());
    add(yave::get_node_definitions<node::Bool, _std::tag>());
    add(yave::get_node_definitions<node::String, _std::tag>());
    add(yave::get_node_definitions<node::MixFrame, _std::tag>(fmngr, compositor));
    add(yave::get_node_definitions<node::Time, _std::tag>());
    add(yave::get_node_definitions<node::ReTime, _std::tag>());
    add(yave::get_node_definitions<node::DelayTime, _std::tag>());
    add(yave::get_node_definitions<node::ScaleTime, _std::tag>());
    add(yave::get_node_definitions<node::Rotate, _std::geometry::tag>());
    add(yave::get_node_definitions<node::RotateX, _std::geometry::tag>());
    add(yave::get_node_definitions<node::RotateY, _std::geometry::tag>());
    add(yave::get_node_definitions<node::RotateZ, _std::geometry::tag>());
    add(yave::get_node_definitions<node::Translate, _std::geometry::tag>());
    add(yave::get_node_definitions<node::CircleShape, _std::tag>());
    add(yave::get_node_definitions<node::DrawShape, _std::tag>(fmngr));
    add(yave::get_node_definitions<node::ShapeTranslate, _std::tag>());
    add(yave::get_node_definitions<node::ShapeRotate, _std::tag>());
    add(yave::get_node_definitions<node::ShapeScale, _std::tag>());

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
    return _std::module_name;
  }

  auto module::module_id() const -> uuid
  {
    return _std::module_id;
  }

} // namespace yave::modules::_std