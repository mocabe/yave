//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/rts.hpp>
#include <yave/node/core/node_handle.hpp>
#include <yave/node/core/socket_handle.hpp>

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

    struct result_base
    {
      auto message() const -> std::string
      {
        return {};
      }

      auto node_id() const -> uid
      {
        return {};
      }

      auto socket_id() const -> uid
      {
        return {};
      }
    };

    /// for unexpected errors
    struct unexpected_error : result_base
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
    struct missing_input : result_base
    {
      missing_input(const node_handle& node, const socket_handle& socket)
        : m_node_id {node.id()}
        , m_socket_id {socket.id()}
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
    struct missing_output : result_base
    {
      missing_output(const node_handle& node, const socket_handle& socket)
        : m_node_id {node.id()}
        , m_socket_id {socket.id()}
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
    struct has_default_argument : result_base
    {
      has_default_argument(const node_handle& node, const socket_handle& socket)
        : m_node_id {node.id()}
        , m_socket_id {socket.id()}
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

    /// Socket has input connection
    struct has_input_connection : result_base
    {
      has_input_connection(const node_handle& node, const socket_handle& socket)
        : m_node_id {node.id()}
        , m_socket_id {socket.id()}
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

    /// Socket has output connection
    struct has_output_connection : result_base
    {
      has_output_connection(
        const node_handle& node,
        const socket_handle& socket)
        : m_node_id {node.id()}
        , m_socket_id {socket.id()}
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
    struct is_lambda_node : result_base
    {
      is_lambda_node(const node_handle& node)
        : m_node_id {node.id()}
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

    using parse_result = std::variant< //
      unexpected_error,
      missing_input,
      missing_output,
      is_lambda_node,
      has_default_argument,
      has_input_connection,
      has_output_connection>;

  } // namespace parse_results

  /// parser result
  using parse_results::parse_result;

  /// get type of parse result
  [[nodiscard]] inline auto type(const parse_result& r)
  {
    return std::visit([](auto&& x) { return x.type(); }, r);
  }

  /// get message from parse result
  [[nodiscard]] inline auto message(const parse_result& r)
  {
    return std::visit([](auto&& x) { return x.message(); }, r);
  }

  /// get node id of parse result
  [[nodiscard]] inline auto node_id(const parse_result& r)
  {
    return std::visit([](auto&& x) { return x.node_id(); }, r);
  }

  /// get socket id of parse result
  [[nodiscard]] inline auto socket_id(const parse_result& r)
  {
    return std::visit([](auto&& x) { return x.socket_id(); }, r);
  }

} // namespace yave