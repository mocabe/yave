//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/core/rts.hpp>
#include <yave/node/core/bind_info.hpp>
#include <yave/node/core/node_handle.hpp>
#include <yave/support/error.hpp>

#include <vector>
#include <memory>

namespace yave {

  namespace parse_errors {

    struct no_sufficient_input : error_info<no_sufficient_input>
    {
      no_sufficient_input(const node_handle& node)
        : m_node {node}
      {
      }

      /// Error message.
      [[nodiscard]] virtual std::string message() const override;

      /// Get node.
      [[nodiscard]] const node_handle& node() const
      {
        return m_node;
      }

    private:
      node_handle m_node;
    };

    /// No valid overloading error.
    struct no_valid_overloading : error_info<no_valid_overloading>
    {
      no_valid_overloading(
        const node_handle& node,
        const std::vector<std::shared_ptr<const bind_info>>& binds)
        : m_node {node}
        , m_binds {binds}
      {
      }

      /// Get error message.
      [[nodiscard]] virtual std::string message() const override;

      /// Get node.
      [[nodiscard]] const node_handle& node() const
      {
        return m_node;
      }

      /// Get overloading bindings.
      [[nodiscard]] const std::vector<std::shared_ptr<const bind_info>>
        binds() const
      {
        return m_binds;
      }

    private:
      node_handle m_node;
      std::vector<std::shared_ptr<const bind_info>> m_binds;
    };

    /// Ambiguous overloading error.
    struct ambiguous_overloading : error_info<ambiguous_overloading>
    {
      ambiguous_overloading(
        const node_handle& node,
        const std::vector<std::shared_ptr<const bind_info>>& binds)
        : m_node {node}
        , m_binds {binds}
      {
      }

      /// Get error message.
      [[nodiscard]] virtual std::string message() const override;

      /// Get node.
      [[nodiscard]] const node_handle& node() const
      {
        return m_node;
      }

      /// Get overloading bindings.
      [[nodiscard]] const std::vector<std::shared_ptr<const bind_info>>&
        binds() const
      {
        return m_binds;
      }

    private:
      node_handle m_node;
      std::vector<std::shared_ptr<const bind_info>> m_binds;
    };

    /// Type missmatch error.
    struct type_missmatch : error_info<type_missmatch>
    {
      type_missmatch(
        const node_handle& node,
        const std::string& socket,
        const object_ptr<const Type> expected,
        const object_ptr<const Type>& provided)
        : m_node {node}
        , m_socket {socket}
        , m_expected {expected}
        , m_provided {provided}
      {
      }

      /// Get error message.
      [[nodiscard]] virtual std::string message() const override;

      /// Get node.
      [[nodiscard]] const node_handle& node() const
      {
        return m_node;
      }

      /// Get socket.
      [[nodiscard]] const std::string& socket() const
      {
        return m_socket;
      }

      /// Get expected type.
      [[nodiscard]] object_ptr<const Type> expected() const
      {
        return m_expected;
      }

      /// Get provided type.
      [[nodiscard]] object_ptr<const Type> provided() const
      {
        return m_provided;
      }

    private:
      node_handle m_node;
      std::string m_socket;
      object_ptr<const Type> m_expected;
      object_ptr<const Type> m_provided;
    };

  } // namespace parse_errors
} // namespace yave
