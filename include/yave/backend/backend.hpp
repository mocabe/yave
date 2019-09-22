//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/config/config.hpp>
#include <yave/backend/backend_info.hpp>
#include <yave/node/support/bind_info_manager.hpp>
#include <yave/obj/scene/scene_config.hpp>
#include <yave/support/id.hpp>

namespace yave {

  /// Backend instance controller.
  /// This class manages communication between backends.
  class backend
  {
  public:
    backend(const object_ptr<const BackendInfo>& info);

    /// Initialize backend
    void init(const scene_config& config);

    /// Shutdown backend
    void deinit();

    /// Update backend scene config
    void update(const scene_config& config);

    /// Initialized?
    [[nodiscard]] bool initialized() const;

    /// Get binding list
    [[nodiscard]] auto get_binds() const -> bind_info_manager;

    /// Get current scene_config of backend
    [[nodiscard]] auto get_config() const -> scene_config;

    /// Get instance ID
    [[nodiscard]] auto instance_id() const -> uid;

    /// Get name of backend
    [[nodiscard]] auto name() const -> std::string;

    /// Get namespace UUID of backend
    [[nodiscard]] auto backend_id() const -> uuid;

  private:
    object_ptr<const BackendInfo> m_backend_info;

  private:
    bool m_initialized;
    uid m_instance_id;
  };
} // namespace yave