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
    /// \param type_classes type classes for each output
    /// \param node_type type of node
    /// \requires oss.size() == type_classes.size().
    /// \requires number of arguments of each type class should match the number
    /// of inputs. for interface nodes, types will be ignored.
    node_declaration(
      std::string name,
      std::vector<std::string> iss,
      std::vector<std::string> oss,
      std::vector<object_ptr<const Type>> type_classes,
      node_type node_type)
      : m_name {std::move(name)}
      , m_iss {std::move(iss)}
      , m_oss {std::move(oss)}
      , m_node_type {std::move(node_type)}
      , m_type_classes {std::move(type_classes)}
    {
      if (m_node_type == node_type::interface)
        m_type_classes = {m_oss.size(), object_type<Undefined>()};

      if (m_type_classes.size() != m_oss.size())
        throw std::runtime_error("Invalid number of type classes");

      for (auto&& tc : m_type_classes) {

        if (!tc)
          throw std::runtime_error("Invalid type class");

        // inputs + demand + return type
        if (flatten(tc).size() != m_iss.size() + 2)
          throw std::runtime_error("Invalid number of inputs in type class: ");
      }
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

    [[nodiscard]] auto node_type() const -> const auto&
    {
      return m_node_type;
    }

    [[nodiscard]] auto type_classes() const -> const auto&
    {
      assert(m_node_type != node_type::interface);
      return m_type_classes;
    }

  private:
    std::string m_name;
    std::vector<std::string> m_iss;
    std::vector<std::string> m_oss;
    yave::node_type m_node_type;

  private:
    /// for non-composite node
    std::vector<object_ptr<const Type>> m_type_classes;
  };
}