//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/core/config.hpp>
#include <yave/rts/eval.hpp>
#include <yave/obj/primitive/primitive.hpp>

namespace yave {

  /// Node definition provided by backend.
  class node_definition
  {
  public:
    /// Construct node definition.
    /// \param full_name fully qualified name of bind (should match to
    /// target node_info::qualified_name())
    /// \param output_socket name of output socket
    /// \param instance A non-null managed pointer to a closure object
    /// \throws std::invalid_argument when arguments are invalid.
    node_definition(
      std::string full_name,
      size_t output_socket,
      object_ptr<const Object> instance)
      : m_full_name {std::move(full_name)}
      , m_os {std::move(output_socket)}
      , m_instance {std::move(instance)}
    {
      if (!m_instance)
        throw std::invalid_argument("instance is null");
    }

    [[nodiscard]] auto& full_name() const
    {
      return m_full_name;
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
    std::string m_full_name;
    /// output socket index
    size_t m_os;
    /// instance getter
    object_ptr<const Object> m_instance;
  };
} // namespace yave