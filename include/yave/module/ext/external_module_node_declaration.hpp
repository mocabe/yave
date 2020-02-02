//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/config/config.hpp>
#include <yave/lib/string/string.hpp>
#include <yave/lib/vector/vector.hpp>
#include <yave/node/core/node_declaration.hpp>
#include <yave/rts/box.hpp>

namespace yave {

  /// node_declaration for module interface.
  /// Only for communication with modules.
  /// TODO: support composite node.
  struct external_module_node_declaration
  {
    external_module_node_declaration(const node_declaration& decl)
    {
      m_name         = decl.name();
      m_type_class   = decl.type_class();
      m_default_data = decl.default_data();

      std::vector<string> iss;
      for (auto&& s : decl.input_sockets()) {
        iss.push_back(s);
      }
      m_iss = std::move(iss);

      std::vector<string> oss;
      for (auto&& s : decl.output_sockets()) {
        oss.push_back(s);
      }
      m_oss = std::move(oss);
    }

    /// Get name
    auto name() const -> std::string
    {
      return m_name;
    }

    /// Get input sockets
    auto input_sockets() const -> std::vector<std::string>
    {
      std::vector<std::string> ret;
      for (auto&& s : m_iss) {
        ret.emplace_back(s.c_str());
      }
      return ret;
    }

    /// Get output sockets
    auto output_sockets() const -> std::vector<std::string>
    {
      std::vector<std::string> ret;
      for (auto&& s : m_oss) {
        ret.emplace_back(s.c_str());
      }
      return ret;
    }

    /// Get type class
    auto type_class() const -> object_ptr<const Type>
    {
      return m_type_class;
    }

    /// Get default data value
    /// \note Can return nullptr!
    auto default_data() const -> object_ptr<const Object>
    {
      return m_default_data;
    }

    /// Get node declaration
    auto node_declaration() const -> node_declaration
    {
      return {name(),
              input_sockets(),
              output_sockets(),
              type_class(),
              default_data()};
    }

  private:
    string m_name;                           // 16
    vector<string> m_iss;                    // 16
    vector<string> m_oss;                    // 16
    object_ptr<const Type> m_type_class;     // 8
    object_ptr<const Object> m_default_data; // 8
  };

  static_assert(sizeof(external_module_node_declaration) == 64);

  /// ModuleNodeDeclaration
  using ExternalModuleNodeDeclaration = Box<external_module_node_declaration>;

  /// List of node declaration
  struct external_module_node_declaration_list
  {
    vector<object_ptr<ExternalModuleNodeDeclaration>> list; // 16
  };

  static_assert(sizeof(external_module_node_declaration_list) == 16);

  /// ModuleNodeDeclarationList
  using ExternalModuleNodeDeclarationList =
    Box<external_module_node_declaration_list>;

} // namespace yave

YAVE_DECL_TYPE(
  yave::ExternalModuleNodeDeclaration,
  "6e5d0ee-a903-4881-92e4-451ad415db96");
YAVE_DECL_TYPE(
  yave::ExternalModuleNodeDeclarationList,
  "aa67c4c0-4419-406e-9080-a029bf6b291c");