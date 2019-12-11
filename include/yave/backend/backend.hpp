//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/config/config.hpp>
#include <yave/node/core/node_declaration.hpp>
#include <yave/node/core/node_definition.hpp>
#include <yave/obj/scene/scene_config.hpp>
#include <yave/support/id.hpp>

#include <memory>

namespace yave {

  /// backend instance base
  class backend
  {
  public:
    virtual ~backend() = default;

    /// Initialize backend
    virtual void init(const scene_config& config) = 0;
    /// Shutdown backend
    virtual void deinit() = 0;
    /// Update backend scene config
    virtual void update(const scene_config& config) = 0;
    /// Initialized?
    [[nodiscard]] virtual bool initialized() const = 0;
    /// Get additional declaration list
    [[nodiscard]] virtual auto get_node_declarations() const
      -> std::vector<node_declaration> = 0;
    /// Get additional definition list
    [[nodiscard]] virtual auto get_node_definitions() const
      -> std::vector<node_definition> = 0;
    /// Get current scene_config of backend
    [[nodiscard]] virtual auto get_scene_config() const -> scene_config = 0;
    /// Get instance ID
    [[nodiscard]] virtual auto instance_id() const -> uid = 0;
    /// Get name of backend
    [[nodiscard]] virtual auto name() const -> std::string = 0;
    /// Get namespace UUID of backend
    [[nodiscard]] virtual auto backend_id() const -> uuid = 0;
  };

  template <class BackendTag>
  struct backend_traits
  {
    // static auto get_backend() -> std::unique_ptr<backend>
  };

  template <class BackendTag, class... Args>
  [[nodiscard]] auto get_backend(Args&&... args) -> std::unique_ptr<backend>
  {
    return backend_traits<BackendTag>::get_backend(std::forward<Args>(args)...);
  }

} // namespace yave