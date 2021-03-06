//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/config/config.hpp>
#include <yave/lib/string/string.hpp>
#include <yave/lib/vector/vector.hpp>
#include <yave/node/core/node_definition.hpp>
#include <yave/rts/box.hpp>

namespace yave {

  /// node_definition for module interface.
  /// Only for communication with modules.
  /// Keep data types as binary compatible as possible.
  struct external_module_node_definition
  {
    external_module_node_definition(const node_definition& def)
    {
      m_qualified_name = def.qualified_name();
      m_instance       = def.instance();
      m_output         = def.output_socket();
      m_description    = def.description();
    }

    /// Get name
    [[nodiscard]] auto qualified_name() const -> std::string
    {
      return m_qualified_name;
    }

    /// Get output socket index
    [[nodiscard]] auto output_socket() const -> uint64_t
    {
      return m_output;
    }

    /// Get description
    [[nodiscard]] auto description() const -> std::string
    {
      return m_description;
    }

    /// Get instance generator function
    [[nodiscard]] auto& instance() const
    {
      return m_instance;
    }

    /// Convert to yave::bind info
    [[nodiscard]] auto node_definition() const -> node_definition
    {
      return {qualified_name(), output_socket(), instance(), description()};
    }

  private:
    string m_qualified_name;             // 16
    string m_description;                // 16
    uint64_t m_output;                   // 8
    object_ptr<const Object> m_instance; // 8
  };

  static_assert(sizeof(external_module_node_definition) == 48);

  /// ModuleNodeDefinition
  using ExternalModuleNodeDefinition = Box<external_module_node_definition>;

  /// List of node definition
  struct external_module_node_definition_list
  {
    vector<object_ptr<ExternalModuleNodeDefinition>> list; // 16
  };

  static_assert(sizeof(external_module_node_definition_list) == 16);

  /// ModuleNodeDefinitionList
  using ExternalModuleNodeDefinitionList =
    Box<external_module_node_definition_list>;
} // namespace yave

YAVE_DECL_TYPE(
  yave::ExternalModuleNodeDefinition,
  "e6bdef71-b50f-4591-a6fc-ee99aa78ba4c");
YAVE_DECL_TYPE(
  yave::ExternalModuleNodeDefinitionList,
  "a292b66b-e8c2-4cae-a02e-3de38fa38cb7");