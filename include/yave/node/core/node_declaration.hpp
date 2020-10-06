//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/dynamic_typing.hpp>
#include <yave/node/core/node_handle.hpp>

#include <functional>

namespace yave {

  class structured_node_graph;

  /// Node declaration
  class node_declaration
  {
  public:
    /// normal node
    /// \param full_name Fully qualified name of this node. (ex.'Std.Num.Float')
    /// \param description description of this node declaration
    /// \param iss input socket names
    /// \param oss output socket names
    /// \param default_arg socket default values
    node_declaration(
      std::string full_name,
      std::string description,
      std::vector<std::string> iss,
      std::vector<std::string> oss,
      std::vector<std::pair<size_t, object_ptr<Object>>> default_arg = {})
      : m_name {std::move(full_name)}
      , m_description {std::move(description)}
      , m_iss {std::move(iss)}
      , m_oss {std::move(oss)}
      , m_default_values {std::move(default_arg)}
      , m_initializer {}
    {
      _validate();
    }

    /// generated function
    /// \param full_name Fully qualified name of this node
    /// \param description description of this node declaration
    /// \param iss input socket names
    /// \param oss output socket names
    /// \param initializer initializer function
    node_declaration(
      std::string full_name,
      std::string description,
      std::vector<std::string> iss,
      std::vector<std::string> oss,
      std::function<node_handle(structured_node_graph&, const node_handle&)>
        initializer)
      : m_name {std::move(full_name)}
      , m_description {std::move(description)}
      , m_iss {std::move(iss)}
      , m_oss {std::move(oss)}
      , m_default_values {}
      , m_initializer {std::move(initializer)}
    {
      _validate();
    }

    [[nodiscard]] auto& description() const
    {
      return m_description;
    }

    [[nodiscard]] auto& input_sockets() const
    {
      return m_iss;
    }

    [[nodiscard]] auto& output_sockets() const
    {
      return m_oss;
    }

    [[nodiscard]] auto& default_arg() const
    {
      return m_default_values;
    }

    [[nodiscard]] auto& initializer() const
    {
      return m_initializer;
    }

    [[nodiscard]] auto& full_name() const
    {
      return m_name;
    }

    // Get path to name (ex. A.B.C -> 'A.B')
    [[nodiscard]] auto node_path() const -> std::string;
    // Get name of node (ex. A.B.C -> 'C')
    [[nodiscard]] auto node_name() const -> std::string;

  private:
    void _validate();

  private:
    std::string m_name;
    std::string m_description;
    std::vector<std::string> m_iss;
    std::vector<std::string> m_oss;
    std::vector<std::pair<size_t, object_ptr<Object>>> m_default_values;
    std::function<node_handle(structured_node_graph&, const node_handle&)>
      m_initializer;
  };

} // namespace yave