//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/dynamic_typing.hpp>
#include <yave/node/core/node_handle.hpp>
#include <functional>

#include <functional>

namespace yave {

  class managed_node_graph;

  /// Node declaration
  class node_declaration
  {
  public:
    /// normal node
    /// \param name Name of this node
    /// \param iss input socket names
    /// \param oss output socket names
    /// \param type_classe type class for this node
    /// \param default_data default data value of this node. Can be nullptr when
    /// no data value required.
    /// \requires oss.size() == type_classes.size().
    /// \requires number of arguments of each type class should match the number
    /// of inputs. for interface nodes, types will be ignored.
    node_declaration(
      std::string name,
      std::vector<std::string> iss,
      std::vector<std::string> oss,
      object_ptr<const Type> type_class,
      object_ptr<const Object> default_data = nullptr)
      : m_name {std::move(name)}
      , m_iss {std::move(iss)}
      , m_oss {std::move(oss)}
      , m_type_class {std::move(type_class)}
      , m_default_data {std::move(default_data)}
    {
      if (!m_type_class)
        throw std::runtime_error("Invalid type class");

      // inputs + demand + return type
      if (flatten(m_type_class).size() != m_iss.size() + 2)
        throw std::runtime_error("Invalid number of inputs in type class: ");
    }

    [[nodiscard]] auto name() const -> const auto&
    {
      return m_name;
    }

    [[nodiscard]] auto input_sockets() const -> const auto&
    {
      return m_iss;
    }

    [[nodiscard]] auto output_sockets() const -> const auto&
    {
      return m_oss;
    }

    [[nodiscard]] auto type_class() const -> const auto&
    {
      return m_type_class;
    }

    /// \note Can return nullptr!
    [[nodiscard]] auto default_data() const -> const auto&
    {
      return m_default_data;
    }

  private:
    std::string m_name;
    std::vector<std::string> m_iss;
    std::vector<std::string> m_oss;

  private:
    object_ptr<const Type> m_type_class;

  private:
    object_ptr<const Object> m_default_data;
  };
}