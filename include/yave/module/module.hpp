//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/core/config.hpp>
#include <yave/node/core/node_declaration.hpp>
#include <yave/node/core/node_definition.hpp>
#include <yave/obj/scene/scene_config.hpp>
#include <yave/core/id.hpp>
#include <yave/core/uuid.hpp>

#include <memory>
#include <vector>
#include <string>

namespace yave {

  /// module instance base
  class module
  {
  public:
    virtual ~module() = default;

    /// Initialize module.
    /// \param config initial scene config
    virtual void init(const scene_config& config) = 0;
    /// Shutdown module
    virtual void deinit() = 0;
    /// Update module scene config
    virtual void update(const scene_config& config) = 0;
    /// Initialized?
    [[nodiscard]] virtual bool initialized() const = 0;
    /// Get additional declaration list
    [[nodiscard]] virtual auto get_node_declarations() const
      -> std::vector<node_declaration> = 0;
    /// Get additional definition list
    [[nodiscard]] virtual auto get_node_definitions() const
      -> std::vector<node_definition> = 0;
    /// Get current scene_config of module
    [[nodiscard]] virtual auto get_scene_config() const -> scene_config = 0;
    /// Get instance ID
    [[nodiscard]] virtual auto instance_id() const -> uid = 0;
    /// Get name of module, including namespace
    [[nodiscard]] virtual auto name() const -> std::string = 0;
    /// Get namespace UUID of module
    [[nodiscard]] virtual auto module_id() const -> uuid = 0;
  };

} // namespace yave