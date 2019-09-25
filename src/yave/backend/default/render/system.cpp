//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/backend/default/render/system.hpp>
#include <yave/support/log.hpp>

/* backend objects */
#include <yave/backend/default/render/blend_op.hpp>
#include <yave/backend/default/render/compositor.hpp>
#include <yave/backend/default/render/frame_buffer_constructor.hpp>
#include <yave/backend/default/render/frame_constructor.hpp>
#include <yave/backend/default/render/image_output.hpp>
#include <yave/backend/default/render/keyframe.hpp>
#include <yave/backend/default/render/primitive_constructor.hpp>
#include <yave/backend/default/render/primitive.hpp>

namespace {

  std::shared_ptr<spdlog::logger> g_logger;

  void init_logger()
  {
    [[maybe_unused]] static auto init = [] {
      g_logger = yave::add_logger("yave::backend::default_render");
      return 1;
    }();
  }
}

namespace yave::backend::default_render {

  struct backend::instance
  {
    uid id;
    scene_config config;
    std::unique_ptr<frame_buffer_manager> fb_manager;
  };

  auto backend::_find_instance(const uid& id)
  {
    return std::find_if(m_instances.begin(), m_instances.end(), [&](auto& e) {
      return e.id == id;
    });
  }

  uid backend::_create_instance(const scene_config& config)
  {
    auto new_id = uid::random_generate();
    m_instances.push_back(instance {
      new_id,
      config,
      std::make_unique<frame_buffer_manager>(
        config.width(), config.height(), config.frame_buffer_format())});
    return new_id;
  }

  void backend::_destroy_instance(const uid& id)
  {
    auto iter = _find_instance(id);
    if (iter != m_instances.end()) {
      m_instances.erase(iter);
    }
  }

  backend::backend()
  {
    init_logger();

    // clang-format off
    m_backend_info = make_object<BackendInfo>(
      name(),
      backend_id(),
      (void*)this,
      [](void* handle, const scene_config& config) noexcept { return ((backend*)handle)->init(config); },
      [](void* handle, uid id) noexcept { return ((backend*)handle)->deinit(id); },
      [](void* handle, uid id, const scene_config& config) noexcept { return ((backend*)handle)->update(id, config); },
      [](void* handle, uid id) noexcept { return ((backend*)handle)->get_config(id); },
      [](void* handle, uid id) noexcept { return ((backend*)handle)->get_binds(id); }
    );
    // clang-format on
  }

  auto backend::init(const scene_config& config) noexcept -> uid
  {
    try {
      if (m_initialized)
        return uid();

      auto id       = _create_instance(config);
      m_initialized = true;
      return id;

    } catch (...) {
      return uid();
    }
  }

  void backend::deinit(uid id) noexcept
  {
    try {
      if (!m_initialized)
        return;

      _destroy_instance(id);

    } catch (...) {
      return;
    }
  }

  bool backend::update(uid id, const scene_config& config) noexcept
  {
    try {
      if (!m_initialized)
        return false;

      auto iter = _find_instance(id);

      if (iter != m_instances.end()) {
        auto fbm = std::make_unique<frame_buffer_manager>(
          config.width(), config.height(), config.frame_buffer_format());
        iter->fb_manager = std::move(fbm);
        iter->config     = config;
      }
      return true;

    } catch (...) {
      return false;
    }
  }

  auto backend::get_config(uid id) noexcept -> object_ptr<SceneConfig>
  {
    try {
      auto iter = _find_instance(id);
      if (iter != m_instances.end()) {
        return make_object<SceneConfig>(iter->config);
      }
      return nullptr;
    } catch (...) {
      return nullptr;
    }
  }

  auto backend::get_binds(uid id) noexcept -> object_ptr<BackendBindInfoList>
  {
    auto iter = _find_instance(id);

    if (iter == m_instances.end())
      return nullptr;

    instance& inst = *iter;

    std::vector<object_ptr<BackendBindInfo>> binds;

    // add backends impl
    {
      // clang-format off

      binds.push_back(make_object<BackendBindInfo>(get_bind_info<node::FrameBuffer, tags::default_render>(*(inst.fb_manager))));
      binds.push_back(make_object<BackendBindInfo>(get_bind_info<node::BlendOpSrc, tags::default_render>()));
      binds.push_back(make_object<BackendBindInfo>(get_bind_info<node::BlendOpDst, tags::default_render>()));
      binds.push_back(make_object<BackendBindInfo>(get_bind_info<node::BlendOpOver, tags::default_render>()));
      binds.push_back(make_object<BackendBindInfo>(get_bind_info<node::BlendOpIn, tags::default_render>()));
      binds.push_back(make_object<BackendBindInfo>(get_bind_info<node::BlendOpOut, tags::default_render>()));
      binds.push_back(make_object<BackendBindInfo>(get_bind_info<node::BlendOpAdd, tags::default_render>()));
      binds.push_back(make_object<BackendBindInfo>(get_bind_info<node::LayerCompositor, tags::default_render>()));
      binds.push_back(make_object<BackendBindInfo>(get_bind_info<node::Frame, tags::default_render>()));
      binds.push_back(make_object<BackendBindInfo>(get_bind_info<node::LayerImageOutput, tags::default_render>()));
      binds.push_back(make_object<BackendBindInfo>(get_bind_info<node::KeyframeIntValue, tags::default_render>()));
      binds.push_back(make_object<BackendBindInfo>(get_bind_info<node::KeyframeFloatValue, tags::default_render>()));
      binds.push_back(make_object<BackendBindInfo>(get_bind_info<node::KeyframeBoolValue, tags::default_render>()));
      binds.push_back(make_object<BackendBindInfo>(get_bind_info<node::KeyframeIntValueExtractor, tags::default_render>()));
      binds.push_back(make_object<BackendBindInfo>(get_bind_info<node::KeyframeFloatValueExtractor , tags::default_render>()));
      binds.push_back(make_object<BackendBindInfo>(get_bind_info<node::KeyframeBoolValueExtractor, tags::default_render>()));
      binds.push_back(make_object<BackendBindInfo>(get_bind_info<node::Int8, tags::default_render>()));
      binds.push_back(make_object<BackendBindInfo>(get_bind_info<node::Int16, tags::default_render>()));
      binds.push_back(make_object<BackendBindInfo>(get_bind_info<node::Int32, tags::default_render>()));
      binds.push_back(make_object<BackendBindInfo>(get_bind_info<node::Int64, tags::default_render>()));
      binds.push_back(make_object<BackendBindInfo>(get_bind_info<node::UInt8, tags::default_render>()));
      binds.push_back(make_object<BackendBindInfo>(get_bind_info<node::UInt16, tags::default_render>()));
      binds.push_back(make_object<BackendBindInfo>(get_bind_info<node::UInt32, tags::default_render>()));
      binds.push_back(make_object<BackendBindInfo>(get_bind_info<node::UInt64, tags::default_render>()));
      binds.push_back(make_object<BackendBindInfo>(get_bind_info<node::Float, tags::default_render>()));
      binds.push_back(make_object<BackendBindInfo>(get_bind_info<node::Double, tags::default_render>()));
      binds.push_back(make_object<BackendBindInfo>(get_bind_info<node::Bool, tags::default_render>()));
      binds.push_back(make_object<BackendBindInfo>(get_bind_info<node::String, tags::default_render>()));

      // clang-format on
    }

    return make_object<BackendBindInfoList>(binds);
  }

  auto backend::name() const noexcept -> std::string
  {
    return "default_render";
  }

  auto backend::backend_id() const noexcept -> const uuid&
  {
    return default_render::backend_id;
  }

  auto backend::get_backend_info() const noexcept
    -> object_ptr<const BackendInfo>
  {
    return m_backend_info;
  }

} // namespace yave::backend::default_render