//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/core/config.hpp>
#include <yave/lib/string/string.hpp>
#include <yave/lib/vector/vector.hpp>
#include <yave/obj/scene/scene_config.hpp>
#include <yave/node/core/node_declaration.hpp>
#include <yave/node/core/node_definition.hpp>
#include <yave/rts/box.hpp>
#include <yave/support/id.hpp>

#include <yave/module/ext/external_module_node_definition.hpp>
#include <yave/module/ext/external_module_node_declaration.hpp>

namespace yave {

  /// module info
  struct external_module_info
  {
    // clang-format off
    external_module_info(
      const string& name,
      const uuid& module_id,
      void* handle,
      auto (*fp_init)(void* handle, const scene_config& config) noexcept -> uid,
      void (*fp_deinit)(void* handle, uid instance) noexcept,
      bool (*fp_update)(void* handle, uid instance, const scene_config& config) noexcept,
      auto (*fp_get_config)(void* handle, uid instance) noexcept -> object_ptr<SceneConfig>,
      auto (*fp_get_declarations)(void* handle, uid instance) noexcept -> object_ptr<ExternalModuleNodeDeclarationList>,
      auto (*fp_get_definitions)(void* handle, uid instance) noexcept -> object_ptr<ExternalModuleNodeDefinitionList>)
      : m_name {name}
      , m_module_id {module_id}
      , m_handle {handle}
      , m_fp_init {fp_init}
      , m_fp_deinit {fp_deinit}
      , m_fp_update {fp_update}
      , m_fp_get_config {fp_get_config}
      , m_fp_get_declarations {fp_get_declarations}
      , m_fp_get_definitions {fp_get_definitions}
    {
    }
    // clang-format on

    /// Initialize new module instance
    [[nodiscard]] auto init(const scene_config& config) const noexcept -> uid
    {
      return m_fp_init(m_handle, config);
    }

    /// Deinitialize module instance
    void deinit(uid instance) const noexcept
    {
      return m_fp_deinit(m_handle, instance);
    }

    /// Update module's scene config
    [[nodiscard]] bool update(uid instance, const scene_config& config) const
      noexcept
    {
      return m_fp_update(m_handle, instance, config);
    }

    /// Get current scene config
    [[nodiscard]] auto get_scene_config(uid instance) const noexcept
      -> object_ptr<SceneConfig>
    {
      return m_fp_get_config(m_handle, instance);
    }

    /// Get list of declarations
    [[nodiscard]] auto get_node_declarations(uid instance) const noexcept
      -> object_ptr<ExternalModuleNodeDeclarationList>
    {
      return m_fp_get_declarations(m_handle, instance);
    }

    /// Get list of definitions
    [[nodiscard]] auto get_node_definitions(uid instance) const noexcept
      -> object_ptr<ExternalModuleNodeDefinitionList>
    {
      return m_fp_get_definitions(m_handle, instance);
    }

    /// Get name of module
    [[nodiscard]] auto name() const -> std::string
    {
      return m_name;
    }

    /// Get UUID of module
    [[nodiscard]] auto module_id() const -> const uuid&
    {
      return m_module_id;
    }

  private:
    /// Name of this module
    string m_name;
    /// UUID of module
    uuid m_module_id;

  private:
    /// Handle of module manager
    void* m_handle;

    /// Initialize new module instance.
    /// \note Should return ID 0 on error, otherwise return non-zero instance
    /// ID.
    /// \note Should initialize frame buffer pool property to match scene
    /// config.
    auto (*m_fp_init)(void* handle, const scene_config& config) noexcept -> uid;

    /// Deinitialize module instance.
    /// \note Should ignore when given instance ID is invalid.
    /// \note Should not deallocate objects allocated from instance if they
    /// still have reference counts left, but any operation on these objects
    /// may result in undefined behaviour after this function.
    void (*m_fp_deinit)(void* handle, uid instance) noexcept;

    /// Update scene config.
    /// \note Should return fasle on error or invalid ID.
    /// \note Should property set or rebuild buffer managers to match new scene
    /// config.
    /// \note Any operations on object which are created before this function
    /// may result in undefined behaviour after this function succeeded. Caller
    /// should acquire new module bindings through get_binds() function.
    bool (*m_fp_update)(
      void* handle,
      uid instance,
      const scene_config& config) noexcept;

    /// Get current scene config.
    /// \note Should return nullptr on fail, otherwise return current scene
    /// config property.
    auto (*m_fp_get_config)(void* handle, uid instance) noexcept
      -> object_ptr<SceneConfig>;

    /// Get list of module declarations.
    /// \note Should return nullptr on fail, otherwise return set of
    /// declarations required to compile scene graph.
    auto (*m_fp_get_declarations)(void* handle, uid instance) noexcept
      -> object_ptr<ExternalModuleNodeDeclarationList>;

    /// Get list of module definitions.
    /// \note Should return nullptr on fail, otherwise return set of definitions
    /// required to compile scene graph.
    auto (*m_fp_get_definitions)(void* handle, uid instance) noexcept
      -> object_ptr<ExternalModuleNodeDefinitionList>;
  };

  /// ModuleInfo
  using ExternalModuleInfo = Box<external_module_info>;

} // namespace yave

YAVE_DECL_TYPE(
  yave::ExternalModuleInfo,
  "85023810-49d6-4bc8-b9fc-22415164934c");