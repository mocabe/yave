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
    /// \param node_type type of node
    /// \param type_class type class of node
    node_declaration(
      std::string name,
      std::vector<std::string> iss,
      std::vector<std::string> oss,
      node_type node_type,
      object_ptr<const Type> type_class)
      : m_name {std::move(name)}
      , m_iss {std::move(iss)}
      , m_oss {std::move(oss)}
      , m_node_type {std::move(node_type)}
      , m_type_class {std::move(type_class)}
    {
      assert(m_type_class);
      assert(flatten(m_type_class).size() == m_iss.size() + 1);

      if (m_node_type == node_type::interface)
        assert(same_type(m_type_class, object_type<Undefined>()));
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

    [[nodiscard]] auto type_class() const -> const auto&
    {
      assert(m_node_type != node_type::interface);
      return m_type_class;
    }

  private:
    std::string m_name;
    std::vector<std::string> m_iss;
    std::vector<std::string> m_oss;
    yave::node_type m_node_type;

  private:
    /// for non-composite node
    object_ptr<const Type> m_type_class;
  };
}