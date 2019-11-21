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
      assert(m_node_type != node_type::composite);
      assert(m_type_class);
    }

    /// composite noed
    node_declaration(
      std::string name,
      std::vector<std::string> iss,
      std::vector<std::string> oss,
      std::function<node_handle(managed_node_graph&, const node_handle&)> func,
      std::string category = "",
      std::string module   = "")
      : m_name {std::move(name)}
      , m_iss {std::move(iss)}
      , m_oss {std::move(oss)}
      , m_composite_func {std::move(func)}
      , m_category {category}
      , m_module {module}
    {
      m_node_type = node_type::composite;
      assert(m_composite_func);
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

    [[nodiscard]] auto get_type_class() const -> const auto&
    {
      assert(m_node_type != node_type::interface);
      return m_type_class;
    }

    [[nodiscard]] auto get_composite_func() const -> const auto&
    {
      assert(m_node_type == node_type::interface);
      return m_composite_func;
    }

    [[nodiscard]] auto category() const -> const auto&
    {
      return m_category;
    }

    [[nodiscard]] auto module() const -> const auto&
    {
      return m_module;
    }

    [[nodiscard]] bool is_composite() const
    {
      return m_node_type == node_type::composite;
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
    /// for composite node
    std::function<node_handle(managed_node_graph&, const node_handle&)>
      m_composite_func;

  private:
    std::string m_category;
    std::string m_module;
  };
}