//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/node_definition.hpp>
#include <yave/node/core/node_handle.hpp>
#include <yave/node/core/socket_handle.hpp>
#include <yave/support/error.hpp>

#include <vector>
#include <memory>

namespace yave {

  namespace compile_error {

    /// No valid overloading error
    struct no_valid_overloading : error_info<no_valid_overloading>
    {
      no_valid_overloading(
        const node_handle& node,
        const socket_handle& socket,
        const std::vector<std::shared_ptr<const node_definition>>& defs)
        : m_node {node}
        , m_socket {socket}
        , m_defs {defs}
      {
      }

      /// Get error message
      [[nodiscard]] auto message() const -> std::string override;

      /// Get node ID
      [[nodiscard]] auto node() const -> const node_handle&
      {
        return m_node;
      }

      /// Get socket
      [[nodiscard]] auto socket() const -> const socket_handle&
      {
        return m_socket;
      }

      /// Get overloading bindings
      [[nodiscard]] auto definitions() const
        -> const std::vector<std::shared_ptr<const node_definition>>&
      {
        return m_defs;
      }

    private:
      node_handle m_node;
      socket_handle m_socket;
      std::vector<std::shared_ptr<const node_definition>> m_defs;
    };

    /// Ambiguous overloading error
    struct ambiguous_overloading : error_info<ambiguous_overloading>
    {
      ambiguous_overloading(
        const node_handle& node,
        const socket_handle& socket,
        const std::vector<std::shared_ptr<const node_definition>>& defs)
        : m_node {node}
        , m_socket {socket}
        , m_defs {defs}
      {
      }

      /// Get error message
      [[nodiscard]] auto message() const -> std::string override;

      /// Get node ID
      [[nodiscard]] auto id() const -> const node_handle&
      {
        return m_node;
      }

      /// Get socket
      [[nodiscard]] auto socket() const -> const socket_handle&
      {
        return m_socket;
      }

      /// Get overloading bindings
      [[nodiscard]] auto definitions() const
        -> const std::vector<std::shared_ptr<const node_definition>>&
      {
        return m_defs;
      }

    private:
      node_handle m_node;
      socket_handle m_socket;
      std::vector<std::shared_ptr<const node_definition>> m_defs;
    };

    /// Type missmatch error
    struct type_missmatch : error_info<type_missmatch>
    {
      type_missmatch(
        const node_handle& node,
        const socket_handle& socket,
        const object_ptr<const Type> expected,
        const object_ptr<const Type>& provided)
        : m_node {node}
        , m_socket {socket}
        , m_expected {expected}
        , m_provided {provided}
      {
      }

      /// Get error message
      [[nodiscard]] auto message() const -> std::string override;

      /// Get node ID
      [[nodiscard]] auto id() const -> const node_handle&
      {
        return m_node;
      }

      /// Get socket
      [[nodiscard]] auto socket() const -> const socket_handle&
      {
        return m_socket;
      }

      /// Get expected type
      [[nodiscard]] auto expected() const -> const object_ptr<const Type>&
      {
        return m_expected;
      }

      /// Get provided type
      [[nodiscard]] auto provided() const -> const object_ptr<const Type>&
      {
        return m_provided;
      }

    private:
      node_handle m_node;
      socket_handle m_socket;
      object_ptr<const Type> m_expected;
      object_ptr<const Type> m_provided;
    };

    /// Unexpected compile error
    struct unexpected_error : error_info<unexpected_error>
    {
      unexpected_error(const std::string& msg)
        : m_msg {msg}
      {
      }

      [[nodiscard]] auto message() const -> std::string override;

    private:
      std::string m_msg;
    };

  } // namespace compile_error
} // namespace yave
