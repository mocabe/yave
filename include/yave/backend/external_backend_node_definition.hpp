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

  /// node_definition for backend interface.
  /// Only for communication with backends.
  /// Keep data types as binary compatible as possible.
  struct external_backend_node_definition
  {
    external_backend_node_definition(const node_definition& def)
    {
      m_name          = def.name();
      m_output        = def.output_socket();
      m_instanec_func = def.instance_getter();
      m_description   = def.description();
    }

    /// Get name
    [[nodiscard]] auto name() const -> std::string
    {
      return m_name;
    }

    /// Get output sockets
    [[nodiscard]] auto output_socket() const -> std::string
    {
      return m_output;
    }

    /// Get description
    [[nodiscard]] auto description() const -> std::string
    {
      return m_description;
    }

    /// Get instance generator function
    [[nodiscard]] auto instance_getter() const
      -> const object_ptr<const Object>&
    {
      return m_instanec_func;
    }

    /// Convert to yave::bind info
    [[nodiscard]] auto node_definition() const -> node_definition
    {
      return {name(), output_socket(), instance_getter(), description()};
    }

  private:
    string m_name;                            // 16
    string m_output;                          // 16
    string m_description;                     // 16
    object_ptr<const Object> m_instanec_func; // 8
  };

  static_assert(sizeof(external_backend_node_definition) == 56);

  /// BackendNodeDefinition
  using ExternalBackendNodeDefinition = Box<external_backend_node_definition>;

  /// List of node definition
  struct external_backend_node_definition_list
  {
    vector<object_ptr<ExternalBackendNodeDefinition>> list; // 16
  };

  static_assert(sizeof(external_backend_node_definition_list) == 16);

  /// BackendNodeDefinitionList
  using ExternalBackendNodeDefinitionList =
    Box<external_backend_node_definition_list>;
}

YAVE_DECL_TYPE(
  yave::ExternalBackendNodeDefinition,
  "e6bdef71-b50f-4591-a6fc-ee99aa78ba4c");
YAVE_DECL_TYPE(
  yave::ExternalBackendNodeDefinitionList,
  "a292b66b-e8c2-4cae-a02e-3de38fa38cb7");