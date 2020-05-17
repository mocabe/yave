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
      no_valid_overloading(const socket_handle& socket)
        : m_socket {socket}
      {
      }

      /// Get error message
      [[nodiscard]] auto message() const -> std::string override;

      /// Get socket
      [[nodiscard]] auto socket() const -> const socket_handle&
      {
        return m_socket;
      }

    private:
      /// src socket
      socket_handle m_socket;
    };

    /// Type missmatch error
    struct type_missmatch : error_info<type_missmatch>
    {
      type_missmatch(
        const socket_handle& s_expected,
        const socket_handle& s_provided,
        const object_ptr<const Type> expected,
        const object_ptr<const Type>& provided)
        : m_socket_expected {s_expected}
        , m_socket_provided {s_provided}
        , m_expected {expected}
        , m_provided {provided}
      {
      }

      /// Get error message
      [[nodiscard]] auto message() const -> std::string override;

      /// Get socket
      [[nodiscard]] auto& socket_expected() const
      {
        return m_socket_expected;
      }

      /// Get socket
      [[nodiscard]] auto& socket_provided() const
      {
        return m_socket_expected;
      }

      /// Get expected type
      [[nodiscard]] auto& expected() const
      {
        return m_expected;
      }

      /// Get provided type
      [[nodiscard]] auto& provided() const
      {
        return m_provided;
      }

    private:
      /// src socket
      socket_handle m_socket_expected;
      /// src socket
      socket_handle m_socket_provided;
      /// expected type
      object_ptr<const Type> m_expected;
      /// provided type
      object_ptr<const Type> m_provided;
    };

    /// Unexpected compile error
    struct unexpected_error : error_info<unexpected_error>
    {
      unexpected_error(const socket_handle& socket, const std::string& msg)
        : m_socket {socket}
        , m_msg {msg}
      {
      }

      /// get message
      [[nodiscard]] auto message() const -> std::string override;

      /// get socket
      [[nodiscard]] auto socket() const -> const socket_handle&
      {
        return m_socket;
      }

    private:
      /// src socket
      socket_handle m_socket;
      /// message
      std::string m_msg;
    };

  } // namespace compile_error
} // namespace yave
