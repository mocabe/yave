//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/module/std/config.hpp>
#include <yave/module/std/module.hpp>

// module defs
#include <yave/module/std/color/color.hpp>
#include <yave/module/std/vec/vec.hpp>
#include <yave/module/std/mat/mat.hpp>
#include <yave/module/std/list/list.hpp>
#include <yave/module/std/list/algorithm.hpp>
#include <yave/module/std/maybe/maybe.hpp>
#include <yave/module/std/function/apply.hpp>
#include <yave/module/std/function/fix.hpp>
#include <yave/module/std/logic/if.hpp>
#include <yave/module/std/logic/ops.hpp>
#include <yave/module/std/primitive/primitive.hpp>
#include <yave/module/std/num/num.hpp>
#include <yave/module/std/num/conversion.hpp>
#include <yave/module/std/string/string.hpp>
#include <yave/module/std/logic/bool.hpp>
#include <yave/module/std/frame/frame.hpp>
#include <yave/module/std/frame/blend.hpp>
#include <yave/module/std/time/retime.hpp>
#include <yave/module/std/time/time.hpp>
#include <yave/module/std/time/ops.hpp>
#include <yave/module/std/shape/shape.hpp>
#include <yave/module/std/shape/draw.hpp>
#include <yave/module/std/shape/transform.hpp>
#include <yave/module/std/shape/merge.hpp>
#include <yave/module/std/ops/ops.hpp>
#include <yave/module/std/math/ops.hpp>
#include <yave/module/std/math/trigonometric.hpp>
#include <yave/module/std/random/random.hpp>
#include <yave/module/std/animation/animation.hpp>

#include <yave/lib/vulkan/offscreen_compositor.hpp>
#include <yave/support/log.hpp>
#include <yave/support/id.hpp>

#include <optional>

YAVE_DECL_LOCAL_LOGGER(modules::std::core)

namespace yave::modules::_std {

  namespace {
    struct module_resource
    {
      module_resource(const scene_config& config, vulkan::vulkan_context& ctx)
        : offscreen_ctx {ctx}
        , frame_buff {config.width(), config.height(), config.frame_format(), module_id, offscreen_ctx}
        , compositor {config.width(), config.height(), offscreen_ctx}
      {
      }

      vulkan::offscreen_context offscreen_ctx;
      data::frame_buffer_manager frame_buff;
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

    return {get_node_declaration<node::Color::Color>(),
            get_node_declaration<node::Color::FromFloats>(),
            get_node_declaration<node::Vec::Vec2>(),
            get_node_declaration<node::Vec::Vec3>(),
            get_node_declaration<node::Vec::Vec4>(),
            get_node_declaration<node::Mat::Mat4>(),
            get_node_declaration<node::Mat::Translate>(),
            get_node_declaration<node::Mat::Rotate>(),
            get_node_declaration<node::Mat::RotateX>(),
            get_node_declaration<node::Mat::RotateY>(),
            get_node_declaration<node::Mat::RotateZ>(),
            get_node_declaration<node::List::Nil>(),
            get_node_declaration<node::List::Cons>(),
            get_node_declaration<node::List::Head>(),
            get_node_declaration<node::List::Tail>(),
            get_node_declaration<node::List::At>(),
            get_node_declaration<node::List::List>(),
            get_node_declaration<node::List::Algo::Map>(),
            get_node_declaration<node::List::Algo::Repeat>(),
            get_node_declaration<node::List::Algo::Enumerate>(),
            get_node_declaration<node::List::Algo::Fold>(),
            get_node_declaration<node::Maybe::Just>(),
            get_node_declaration<node::Maybe::Nothing>(),
            get_node_declaration<node::Maybe::IsJust>(),
            get_node_declaration<node::Maybe::IsNothing>(),
            get_node_declaration<node::Maybe::GetValue>(),
            get_node_declaration<node::Maybe::Map>(),
            get_node_declaration<node::Functional::Apply>(),
            get_node_declaration<node::Functional::Fix>(),
            get_node_declaration<node::Logic::Bool>(),
            get_node_declaration<node::Logic::If>(),
            get_node_declaration<node::Logic::And>(),
            get_node_declaration<node::Logic::Or>(),
            get_node_declaration<node::Logic::Xor>(),
            get_node_declaration<node::Logic::Not>(),
            get_node_declaration<node::Num::Int>(),
            get_node_declaration<node::Num::Float>(),
            get_node_declaration<node::String::String>(),
            get_node_declaration<node::Num::ToFloat>(),
            get_node_declaration<node::Num::ToInt>(),
            get_node_declaration<node::Frame::Frame>(),
            get_node_declaration<node::Frame::Blend>(),
            get_node_declaration<node::Time::ReTime>(),
            get_node_declaration<node::Time::Delay>(),
            get_node_declaration<node::Time::Scale>(),
            get_node_declaration<node::Time::Time>(),
            get_node_declaration<node::Time::Seconds>(),
            get_node_declaration<node::Time::FromSeconds>(),
            get_node_declaration<node::Shape::Shape>(),
            get_node_declaration<node::Shape::Circle>(),
            get_node_declaration<node::Shape::Rect>(),
            get_node_declaration<node::Shape::Fill>(),
            get_node_declaration<node::Shape::Stroke>(),
            get_node_declaration<node::Shape::Draw>(fmngr),
            get_node_declaration<node::Shape::Translate>(),
            get_node_declaration<node::Shape::Rotate>(),
            get_node_declaration<node::Shape::Scale>(),
            get_node_declaration<node::Shape::Merge>(),
            get_node_declaration<node::Ops::Add>(),
            get_node_declaration<node::Ops::Sub>(),
            get_node_declaration<node::Ops::Mul>(),
            get_node_declaration<node::Ops::Div>(),
            get_node_declaration<node::Ops::Eq>(),
            get_node_declaration<node::Ops::Neq>(),
            get_node_declaration<node::Ops::Less>(),
            get_node_declaration<node::Ops::LessEq>(),
            get_node_declaration<node::Ops::Greater>(),
            get_node_declaration<node::Ops::GreaterEq>(),
            get_node_declaration<node::Math::Sin>(),
            get_node_declaration<node::Math::Cos>(),
            get_node_declaration<node::Math::Tan>(),
            get_node_declaration<node::Random::Uniform>(),
            get_node_declaration<node::Random::Normal>(),
            get_node_declaration<node::Animation::Animation>(),
            get_node_declaration<node::Animation::Blank>(),
            get_node_declaration<node::Animation::IsBlank>(),
            get_node_declaration<node::Animation::GetValue>(),
            get_node_declaration<node::Animation::GetLength>(),
            get_node_declaration<node::Animation::Map>(),
            get_node_declaration<node::Animation::Concat>(),
            get_node_declaration<node::Animation::Merge>(),
            get_node_declaration<node::Animation::Stretch>(),
            get_node_declaration<node::Animation::Extend>()};
  }

  auto module::get_node_definitions() const -> std::vector<node_definition>
  {
    std::vector<node_definition> ret;

    // helper
    auto add = [&](auto&& defs) {
      for (auto&& def : defs)
        ret.push_back(def);
    };

    auto& fmngr      = m_pimpl->resource->frame_buff;
    auto& compositor = m_pimpl->resource->compositor;

    add(yave::get_node_definitions<node::Frame::Frame, _std::tag>(fmngr, compositor));

    add(yave::get_node_definitions<node::Color::Color, _std::tag>());
    add(yave::get_node_definitions<node::Color::FromFloats, _std::tag>());
    add(yave::get_node_definitions<node::Vec::Vec2, _std::tag>());
    add(yave::get_node_definitions<node::Vec::Vec3, _std::tag>());
    add(yave::get_node_definitions<node::Vec::Vec4, _std::tag>());
    add(yave::get_node_definitions<node::Mat::Mat4, _std::tag>());
    add(yave::get_node_definitions<node::Mat::Rotate, _std::tag>());
    add(yave::get_node_definitions<node::Mat::RotateX, _std::tag>());
    add(yave::get_node_definitions<node::Mat::RotateY, _std::tag>());
    add(yave::get_node_definitions<node::Mat::RotateZ, _std::tag>());
    add(yave::get_node_definitions<node::Mat::Translate, _std::tag>());
    add(yave::get_node_definitions<node::List::Nil, _std::tag>());
    add(yave::get_node_definitions<node::List::Cons, _std::tag>());
    add(yave::get_node_definitions<node::List::Head, _std::tag>());
    add(yave::get_node_definitions<node::List::Tail, _std::tag>());
    add(yave::get_node_definitions<node::List::At, _std::tag>());
    add(yave::get_node_definitions<node::List::Algo::Map, _std::tag>());
    add(yave::get_node_definitions<node::List::Algo::Repeat, _std::tag>());
    add(yave::get_node_definitions<node::List::Algo::Enumerate, _std::tag>());
    add(yave::get_node_definitions<node::List::Algo::Fold, _std::tag>());
    add(yave::get_node_definitions<node::Maybe::Just, _std::tag>());
    add(yave::get_node_definitions<node::Maybe::Nothing, _std::tag>());
    add(yave::get_node_definitions<node::Maybe::IsJust, _std::tag>());
    add(yave::get_node_definitions<node::Maybe::IsNothing, _std::tag>());
    add(yave::get_node_definitions<node::Maybe::GetValue, _std::tag>());
    add(yave::get_node_definitions<node::Maybe::Map, _std::tag>());
    add(yave::get_node_definitions<node::Logic::Bool, _std::tag>());
    add(yave::get_node_definitions<node::Logic::If, _std::tag>());
    add(yave::get_node_definitions<node::Logic::And, _std::tag>());
    add(yave::get_node_definitions<node::Logic::Or, _std::tag>());
    add(yave::get_node_definitions<node::Logic::Xor, _std::tag>());
    add(yave::get_node_definitions<node::Logic::Not, _std::tag>());
    add(yave::get_node_definitions<node::Functional::Apply, _std::tag>());
    add(yave::get_node_definitions<node::Functional::Fix, _std::tag>());
    add(yave::get_node_definitions<node::Num::Int, _std::tag>());
    add(yave::get_node_definitions<node::Num::Float, _std::tag>());
    add(yave::get_node_definitions<node::String::String, _std::tag>());
    add(yave::get_node_definitions<node::Num::ToFloat, _std::tag>());
    add(yave::get_node_definitions<node::Num::ToInt, _std::tag>());
    add(yave::get_node_definitions<node::Frame::Blend, _std::tag>(fmngr, compositor));
    add(yave::get_node_definitions<node::Time::Time, _std::tag>());
    add(yave::get_node_definitions<node::Time::Seconds, _std::tag>());
    add(yave::get_node_definitions<node::Time::FromSeconds, _std::tag>());
    add(yave::get_node_definitions<node::Time::ReTime, _std::tag>());
    add(yave::get_node_definitions<node::Time::Delay, _std::tag>());
    add(yave::get_node_definitions<node::Time::Scale, _std::tag>());
    add(yave::get_node_definitions<node::Ops::Add, _std::time::tag>());
    add(yave::get_node_definitions<node::Ops::Sub, _std::time::tag>());
    add(yave::get_node_definitions<node::Ops::Eq, _std::time::tag>());
    add(yave::get_node_definitions<node::Ops::Neq, _std::time::tag>());
    add(yave::get_node_definitions<node::Ops::Less, _std::time::tag>());
    add(yave::get_node_definitions<node::Ops::LessEq, _std::time::tag>());
    add(yave::get_node_definitions<node::Ops::Greater, _std::time::tag>());
    add(yave::get_node_definitions<node::Ops::GreaterEq, _std::time::tag>());
    add(yave::get_node_definitions<node::Shape::Shape, _std::tag>());
    add(yave::get_node_definitions<node::Shape::Circle, _std::tag>());
    add(yave::get_node_definitions<node::Shape::Rect, _std::tag>());
    add(yave::get_node_definitions<node::Shape::Fill, _std::tag>());
    add(yave::get_node_definitions<node::Shape::Stroke, _std::tag>());
    add(yave::get_node_definitions<node::Shape::Draw, _std::tag>(fmngr, compositor));
    add(yave::get_node_definitions<node::Shape::Translate, _std::tag>());
    add(yave::get_node_definitions<node::Shape::Rotate, _std::tag>());
    add(yave::get_node_definitions<node::Shape::Scale, _std::tag>());
    add(yave::get_node_definitions<node::Shape::Merge, _std::tag>());
    add(yave::get_node_definitions<node::Ops::Add, _std::math::tag>());
    add(yave::get_node_definitions<node::Ops::Sub, _std::math::tag>());
    add(yave::get_node_definitions<node::Ops::Mul, _std::math::tag>());
    add(yave::get_node_definitions<node::Ops::Div, _std::math::tag>());
    add(yave::get_node_definitions<node::Ops::Eq, _std::math::tag>());
    add(yave::get_node_definitions<node::Ops::Neq, _std::math::tag>());
    add(yave::get_node_definitions<node::Ops::Less, _std::math::tag>());
    add(yave::get_node_definitions<node::Ops::LessEq, _std::math::tag>());
    add(yave::get_node_definitions<node::Ops::Greater, _std::math::tag>());
    add(yave::get_node_definitions<node::Ops::GreaterEq, _std::math::tag>());
    add(yave::get_node_definitions<node::Math::Sin, _std::tag>());
    add(yave::get_node_definitions<node::Math::Cos, _std::tag>());
    add(yave::get_node_definitions<node::Math::Tan, _std::tag>());
    add(yave::get_node_definitions<node::Random::Uniform, _std::tag>());
    add(yave::get_node_definitions<node::Random::Normal, _std::tag>());
    add(yave::get_node_definitions<node::Animation::Animation, _std::tag>());
    add(yave::get_node_definitions<node::Animation::Blank, _std::tag>());
    add(yave::get_node_definitions<node::Animation::IsBlank, _std::tag>());
    add(yave::get_node_definitions<node::Animation::GetValue, _std::tag>());
    add(yave::get_node_definitions<node::Animation::GetLength, _std::tag>());
    add(yave::get_node_definitions<node::Animation::Map, _std::tag>());
    add(yave::get_node_definitions<node::Animation::Concat, _std::tag>());
    add(yave::get_node_definitions<node::Animation::Merge, _std::tag>());
    add(yave::get_node_definitions<node::Animation::Stretch, _std::tag>());
    add(yave::get_node_definitions<node::Animation::Extend, _std::tag>());

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