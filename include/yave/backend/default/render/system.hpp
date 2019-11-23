//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/backend/default/config.hpp>
#include <yave/backend/backend_info.hpp>

#include <yave/backend/default/render/frame_buffer_constructor.hpp>

#include <memory>
#include <algorithm>

namespace yave::backend::default_render {

  /// default_render backend
  class backend
  {
  public:
    /// Ctor
    backend();

    /// initialize
    [[nodiscard]] auto init(const scene_config& config) noexcept -> uid;

    /// deinit
    void deinit(uid id) noexcept;

    /// update scene config
    [[nodiscard]] bool update(uid id, const scene_config& config) noexcept;

    /// get current scene config
    [[nodiscard]] auto get_scene_config(uid id) noexcept
      -> object_ptr<SceneConfig>;

    /// get declaration list
    [[nodiscard]] auto get_node_declarations(uid id) noexcept
      -> object_ptr<BackendNodeDeclarationList>;

    /// get definition list
    [[nodiscard]] auto get_node_definitions(uid id) noexcept
      -> object_ptr<BackendNodeDefinitionList>;

    /// name of backend
    [[nodiscard]] auto name() const noexcept -> std::string;

    /// backend ID
    [[nodiscard]] auto backend_id() const noexcept -> const uuid&;

    /// backend
    [[nodiscard]] auto get_backend_info() const noexcept
      -> object_ptr<const BackendInfo>;

  private:
    struct instance;
    std::vector<instance> m_instances;

  private: /* heleprs */
    [[nodiscard]] auto _lock() const -> std::unique_lock<std::mutex>;
    [[nodiscard]] auto _find_instance(const uid& id);
    [[nodiscard]] uid _create_instance(const scene_config& config);
    void _destroy_instance(const uid& id);

  private:
    object_ptr<const BackendInfo> m_backend_info;

  private:
    mutable std::mutex m_mtx;
  };

} // namespace yave::backend::default_render