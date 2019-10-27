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

  auto backend::_lock() const -> std::unique_lock<std::mutex>
  {
    return std::unique_lock {m_mtx};
  }

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
      auto lck = _lock();

      auto id = _create_instance(config);
      Info(g_logger, "Initialized new backend instance: id={}", to_string(id));
      return id;

    } catch (std::exception& e) {
      Error(g_logger, "Failed to initialized backend instance: {}", e.what());
      return uid();
    } catch (...) {
      Error(g_logger, "Failed to initialized backend instance");
      return uid();
    }
  }

  void backend::deinit(uid id) noexcept
  {
    try {
      auto lck = _lock();

      if (_find_instance(id) == m_instances.end()) {
        Error(g_logger, "Failed to deinit backend: Invalid ID");
        return;
      }

      _destroy_instance(id);
      Info(g_logger, "Deinitialized backend: id={}", to_string(id));

    } catch (std::exception& e) {
      Error(g_logger, "Failed to deinit backend: {}", e.what());
      return;
    } catch (...) {
      Error(g_logger, "Failed to deinit backend");
      return;
    }
  }

  bool backend::update(uid id, const scene_config& config) noexcept
  {
    try {
      auto lck = _lock();

      auto iter = _find_instance(id);

      if (iter == m_instances.end()) {
        Error(g_logger, "Failed to update scene config: Invalid ID");
        return false;
      }

      auto fbm = std::make_unique<frame_buffer_manager>(
        config.width(), config.height(), config.frame_buffer_format());
      iter->fb_manager = std::move(fbm);
      iter->config     = config;

      Info(g_logger, "Updated scene config of instance {}", to_string(id));
      Info(
        g_logger,
        "Current scene config: width={}, height={}, framerate={}, "
        "samplerate={}",
        config.width(),
        config.height(),
        config.frame_rate(),
        config.sample_rate());

      return true;

    } catch (std::exception& e) {
      Error(g_logger, "Failed to update scene cofnig: {}", e.what());
      return false;
    } catch (...) {
      Error(g_logger, "Failed to update scene cofnig");
      return false;
    }
  }

  auto backend::get_config(uid id) noexcept -> object_ptr<SceneConfig>
  {
    try {
      auto lck = _lock();

      auto iter = _find_instance(id);
      if (iter == m_instances.end()) {
        Error(g_logger, "Failed to get scene config: Invalid ID");
        return nullptr;
      }

      return make_object<SceneConfig>(iter->config);

    } catch (std::exception& e) {
      Error(g_logger, "Failed to get scene config: {}", e.what());
      return nullptr;
    } catch (...) {
      Error(g_logger, "Failed to get scene config");
      return nullptr;
    }
  }

  auto backend::get_binds(uid id) noexcept -> object_ptr<BackendBindInfoList>
  {
    try {
      auto lck = _lock();

      auto iter = _find_instance(id);

      if (iter == m_instances.end()) {
        Error(g_logger, "Failed to get bindings: Invalid ID");
        return nullptr;
      }

      instance& inst = *iter;

      std::vector<object_ptr<BackendBindInfo>> binds;

      // add backends impl
      {
        // clang-format off

        binds.push_back(make_object<BackendBindInfo>(get_bind_info<node::FrameBuffer, tags::default_render>(inst.fb_manager.get())));
        binds.push_back(make_object<BackendBindInfo>(get_bind_info<node::BlendOpSrc, tags::default_render>()));
        binds.push_back(make_object<BackendBindInfo>(get_bind_info<node::BlendOpDst, tags::default_render>()));
        binds.push_back(make_object<BackendBindInfo>(get_bind_info<node::BlendOpOver, tags::default_render>()));
        binds.push_back(make_object<BackendBindInfo>(get_bind_info<node::BlendOpIn, tags::default_render>()));
        binds.push_back(make_object<BackendBindInfo>(get_bind_info<node::BlendOpOut, tags::default_render>()));
        binds.push_back(make_object<BackendBindInfo>(get_bind_info<node::BlendOpAdd, tags::default_render>()));
        binds.push_back(make_object<BackendBindInfo>(get_bind_info<node::LayerCompositor, tags::default_render>()));
        binds.push_back(make_object<BackendBindInfo>(get_bind_info<node::Frame, tags::default_render>()));
        binds.push_back(make_object<BackendBindInfo>(get_bind_info<node::LayerImageOutput, tags::default_render>()));
        binds.push_back(make_object<BackendBindInfo>(get_bind_info<node::KeyframeDataInt, tags::default_render>()));
        binds.push_back(make_object<BackendBindInfo>(get_bind_info<node::KeyframeDataFloat, tags::default_render>()));
        binds.push_back(make_object<BackendBindInfo>(get_bind_info<node::KeyframeDataBool, tags::default_render>()));
        binds.push_back(make_object<BackendBindInfo>(get_bind_info<node::KeyframeEvaluatorInt, tags::default_render>()));
        binds.push_back(make_object<BackendBindInfo>(get_bind_info<node::KeyframeEvaluatorFloat , tags::default_render>()));
        binds.push_back(make_object<BackendBindInfo>(get_bind_info<node::KeyframeEvaluatorBool, tags::default_render>()));
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

    } catch (std::exception& e) {
      Error(g_logger, "Failed to get bindings: {}", e.what());
      return nullptr;
    } catch (...) {
      Error(g_logger, "Failed to get bindings");
      return nullptr;
    }
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
    auto lck = _lock();
    return m_backend_info;
  }

} // namespace yave::backend::default_render