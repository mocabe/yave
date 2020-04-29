//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/config/config.hpp>
#include <yave/rts/eval.hpp>
#include <yave/obj/primitive/primitive.hpp>

namespace yave {

  /// Node definition provided by backend.
  class node_definition
  {
  public:
    /// Construct node definition.
    /// \param qualified_name fully qualified name of bind (should match to
    /// target node_info::qualified_name())
    /// \param output_socket name of output socket
    /// \param description Description of this bind.
    /// \param inst_getter A non-null managed pointer to a closure object
    /// \throws std::invalid_argument when arguments are invalid.
    node_definition(
      std::string qualified_name,
      size_t output_socket,
      object_ptr<const Object> instance,
      std::string description)
      : m_qualified_name {std::move(qualified_name)}
      , m_os {std::move(output_socket)}
      , m_instance {std::move(instance)}
      , m_description {std::move(description)}
    {
      // null
      if (!m_instance)
        throw std::invalid_argument("instance is null");
    }

    [[nodiscard]] auto& qualified_name() const
    {
      return m_qualified_name;
    }

    [[nodiscard]] auto& description() const
    {
      return m_description;
    }

    [[nodiscard]] auto& instance() const
    {
      return m_instance;
    }

    [[nodiscard]] auto& output_socket() const
    {
      return m_os;
    }

  private:
    /// name of name
    std::string m_qualified_name;
    /// output socket index
    size_t m_os;
    /// instance getter
    object_ptr<const Object> m_instance;
    /// instance description
    std::string m_description;
  };
}