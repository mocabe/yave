//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

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
    node_declaration(
      std::string name,
      std::vector<std::string> iss,
      std::vector<std::string> oss,
      node_type node_type,
      object_ptr<const Type> type_class,
      std::string category = "",
      std::string module   = "")
      : m_name {std::move(name)}
      , m_iss {std::move(iss)}
      , m_oss {std::move(oss)}
      , m_node_type {std::move(node_type)}
      , m_type_class {std::move(type_class)}
      , m_category {category}
      , m_module {module}
    {
      assert(m_type_class);

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

    [[nodiscard]] auto category() const -> const auto&
    {
      return m_category;
    }

    [[nodiscard]] auto module() const -> const auto&
    {
      return m_module;
    }

  private:
    std::string m_name;
    std::vector<std::string> m_iss;
    std::vector<std::string> m_oss;
    yave::node_type m_node_type;

  private:
    /// for non-composite node
    object_ptr<const Type> m_type_class;

  private:
    std::string m_category;
    std::string m_module;
  };
}