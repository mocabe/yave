//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/rts.hpp>
#include <yave/node/core/node_handle.hpp>

#include <variant>

namespace yave {

  /// parse reuslt type
  enum class parse_result_type
  {
    error   = 0,
    warning = 2,
    info    = 1,
  };

  namespace parse_results {

    /// for unexpected errors
    struct unexpected_error
    {
      unexpected_error(const std::string& msg)
        : m_msg {msg}
      {
      }

      /// Reulst type
      [[nodiscard]] auto type() const
      {
        return parse_result_type::error;
      }

      /// Error message
      [[nodiscard]] auto message() const -> std::string;

    private:
      std::string m_msg;
    };

    /// Missing input connection to socket
    struct missing_input
    {
      missing_input(const uid& node_id, const uid& socket_id)
        : m_node_id {node_id}
        , m_socket_id {socket_id}
      {
      }

      /// Reulst type
      [[nodiscard]] auto type() const
      {
        return parse_result_type::error;
      }

      /// Error message
      [[nodiscard]] auto message() const -> std::string;

      /// Get node ID
      [[nodiscard]] auto node_id() const -> const auto&
      {
        return m_node_id;
      }

      /// Get socket ID
      [[nodiscard]] auto socket_id() const -> const auto&
      {
        return m_socket_id;
      }

    private:
      uid m_node_id;
      uid m_socket_id;
    };

    /// Missing output connection for active node group output
    struct missing_output
    {
      missing_output(const uid& node_id, const uid& socket_id)
        : m_node_id {node_id}
        , m_socket_id {socket_id}
      {
      }

      /// Reulst type
      [[nodiscard]] auto type() const
      {
        return parse_result_type::error;
      }

      /// Error message
      [[nodiscard]] auto message() const -> std::string;

      /// Get node ID
      [[nodiscard]] auto node_id() const -> const auto&
      {
        return m_node_id;
      }

      /// Get socket ID
      [[nodiscard]] auto socket_id() const -> const auto&
      {
        return m_socket_id;
      }

    private:
      uid m_node_id;
      uid m_socket_id;
    };

    /// Socket has argument
    struct has_default_argument
    {
      has_default_argument(uid node_id, uid socket_id)
        : m_node_id {node_id}
        , m_socket_id {socket_id}
      {
      }

      /// Reulst type
      [[nodiscard]] auto type() const
      {
        return parse_result_type::info;
      }

      /// Get node ID
      [[nodiscard]] auto node_id() const -> const auto&
      {
        return m_node_id;
      }

      /// Get socket ID
      [[nodiscard]] auto socket_id() const -> const auto&
      {
        return m_socket_id;
      }

    private:
      uid m_node_id;
      uid m_socket_id;
    };

    /// Node is lambda mode
    struct is_lambda_node
    {
      is_lambda_node(uid node_id)
        : m_node_id {node_id}
      {
      }

      /// Reulst type
      [[nodiscard]] auto type() const
      {
        return parse_result_type::info;
      }

      /// Get node ID
      [[nodiscard]] auto node_id() const -> const auto&
      {
        return m_node_id;
      }

    private:
      uid m_node_id;
    };

    /// node call
    struct is_node_call
    {
      is_node_call(uid node_id)
        : m_node_id {node_id}
      {
      }

      /// Reulst type
      [[nodiscard]] auto type() const
      {
        return parse_result_type::info;
      }

      /// Get node ID
      [[nodiscard]] auto node_id() const -> const auto&
      {
        return m_node_id;
      }

    private:
      uid m_node_id;
    };

    /// node definition
    struct is_node_definition
    {
      is_node_definition(uid node_id)
        : m_node_id {node_id}
      {
      }

      /// Reulst type
      [[nodiscard]] auto type() const
      {
        return parse_result_type::info;
      }

      /// Get node ID
      [[nodiscard]] auto node_id() const -> const auto&
      {
        return m_node_id;
      }

    private:
      uid m_node_id;
    };

    using parse_result = std::variant<
      unexpected_error,
      missing_input,
      missing_output,
      is_lambda_node,
      has_default_argument,
      is_node_call,
      is_node_definition>;

  } // namespace parse_results

  /// parser result
  using parse_results::parse_result;

  /// get message from parse result
  [[nodiscard]] inline auto message(const parse_result& r)
  {
    return std::visit(
      [](auto&& x) {
        if constexpr (requires { x.message(); }) {
          return x.message();
        }
        return std::string();
      },
      r);
  }

} // namespace yave