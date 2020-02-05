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
    /// \param description description of this node declaration
    node_declaration(
      std::string name,
      std::vector<std::string> iss,
      std::vector<std::string> oss,
      std::string description)
      : m_name {std::move(name)}
      , m_iss {std::move(iss)}
      , m_oss {std::move(oss)}
      , m_description {std::move(description)}
    {
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

  private:
    std::string m_name;
    std::vector<std::string> m_iss;
    std::vector<std::string> m_oss;
    std::string m_description;
  };
}