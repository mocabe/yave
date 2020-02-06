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
    /// \param description description of this node declaration
    /// \param default_arg socket default values
    node_declaration(
      std::string name,
      std::vector<std::string> iss,
      std::vector<std::string> oss,
      std::string description,
      std::vector<std::pair<size_t, object_ptr<Object>>> default_arg = {})
      : m_name {std::move(name)}
      , m_iss {std::move(iss)}
      , m_oss {std::move(oss)}
      , m_description {std::move(description)}
      , m_default_values {std::move(default_arg)}
    {
      std::sort(
        m_default_values.begin(),
        m_default_values.end(),
        [](auto&& lhs, auto&& rhs) { return lhs.first < rhs.first; });

      auto it = std::unique(
        m_default_values.begin(),
        m_default_values.end(),
        [](auto&& lhs, auto&& rhs) { return lhs.first == rhs.first; });

      if (it != m_default_values.end())
        throw std::runtime_error("Default value should be unique");

      for (auto&& [idx, defval] : m_default_values) {
        if (idx >= m_iss.size())
          throw std::invalid_argument("Invalid index for default value");
        if (!defval)
          throw std::invalid_argument("Null default value");
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

    [[nodiscard]] auto description() const -> const auto&
    {
      return m_description;
    }

    [[nodiscard]] auto default_arg() const -> const auto&
    {
      return m_default_values;
    }

  private:
    std::string m_name;
    std::vector<std::string> m_iss;
    std::vector<std::string> m_oss;
    std::string m_description;
    std::vector<std::pair<size_t, object_ptr<Object>>> m_default_values;
  };
}