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
      m_name        = decl.name();
      m_description = decl.description();
      m_namespace   = decl.name_space();

      std::vector<uint64_t> idx;
      std::vector<object_ptr<Object>> dat;
      for (auto&& arg : decl.default_arg()) {
        idx.push_back(arg.first);
        dat.push_back(arg.second);
      }
      m_default_arg_idx  = idx;
      m_default_arg_data = dat;

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

    auto description() const -> std::string
    {
      return m_description;
    }

    auto name_space() const -> std::string
    {
      return m_namespace;
    }

    /// Get default arguments
    auto default_arg() const
      -> std::vector<std::pair<size_t, object_ptr<Object>>>
    {
      std::vector<std::pair<size_t, object_ptr<Object>>> ret;
      assert(m_default_arg_idx.size() == m_default_arg_data.size());

      for (size_t i = 0; i < m_default_arg_idx.size(); ++i) {
        ret.emplace_back(m_default_arg_idx[i], m_default_arg_data[i]);
      }
      return ret;
    }

    /// Get node declaration
    auto node_declaration() const -> node_declaration
    {
      return {name(),
              description(),
              name_space(),
              input_sockets(),
              output_sockets(),
              default_arg()};
    }

  private:
    string m_name;                                 // 16
    string m_description;                          // 16
    string m_namespace;                            // 16
    vector<string> m_iss;                          // 16
    vector<string> m_oss;                          // 16
    vector<uint64_t> m_default_arg_idx;            // 16
    vector<object_ptr<Object>> m_default_arg_data; // 16
  };

  static_assert(sizeof(external_module_node_declaration) == 112);

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