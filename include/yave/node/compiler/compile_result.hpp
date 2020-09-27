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

  enum class compile_result_type
  {
    error,
    info,
    warning,
  };

  namespace compile_results {

    struct result_base
    {
      auto node_id() const
      {
        return uid();
      }

      auto socket_id() const
      {
        return uid();
      }

      auto message() const
      {
        return std::string();
      }
    };

    /// Unexpected compile error
    struct unexpected_error : result_base
    {
      unexpected_error(
        const std::string& msg,
        const node_handle& node     = {},
        const socket_handle& socket = {})
        : m_node_id {node.id()}
        , m_socket_id {socket.id()}
        , m_msg {msg}
      {
      }

      [[nodiscard]] auto type() const
      {
        return compile_result_type::error;
      }

      /// get message
      [[nodiscard]] auto message() const -> std::string;

      /// get node
      [[nodiscard]] auto& node_id() const
      {
        return m_node_id;
      }

      /// get socket
      [[nodiscard]] auto& socket_id() const
      {
        return m_socket_id;
      }

    private:
      /// src node
      uid m_node_id;
      /// src socket
      uid m_socket_id;
      /// message
      std::string m_msg;
    };

    /// No valid overloading error
    struct no_valid_overloading : result_base
    {
      no_valid_overloading(const socket_handle& socket)
        : m_socket_id {socket.id()}
      {
      }

      [[nodiscard]] auto type() const
      {
        return compile_result_type::error;
      }

      /// Get error message
      [[nodiscard]] auto message() const -> std::string;

      /// Get socket
      [[nodiscard]] auto& socket_id() const
      {
        return m_socket_id;
      }

    private:
      /// src socket
      uid m_socket_id;
    };

    /// Type missmatch error
    struct type_missmatch : result_base
    {
      type_missmatch(
        const socket_handle& s_expected,
        object_ptr<const Type> expected,
        const socket_handle& s_provided,
        object_ptr<const Type> provided)
        : m_socket_expected_id {s_expected.id()}
        , m_socket_provided_id {s_provided.id()}
        , m_expected {std::move(expected)}
        , m_provided {std::move(provided)}
      {
      }

      [[nodiscard]] auto type() const
      {
        return compile_result_type::error;
      }

      /// Get error message
      [[nodiscard]] auto message() const -> std::string;

      /// Get socket
      [[nodiscard]] auto& socket_id_expected() const
      {
        return m_socket_expected_id;
      }

      /// Get socket
      [[nodiscard]] auto& socket_id_provided() const
      {
        return m_socket_expected_id;
      }

      /// Get expected type
      [[nodiscard]] auto& expected_type() const
      {
        return m_expected;
      }

      /// Get provided type
      [[nodiscard]] auto& provided_type() const
      {
        return m_provided;
      }

    private:
      /// src socket
      uid m_socket_expected_id;
      /// src socket
      uid m_socket_provided_id;
      /// expected type
      object_ptr<const Type> m_expected;
      /// provided type
      object_ptr<const Type> m_provided;
    };

    /// Type constraint error
    struct unsolvable_constraints : result_base
    {
      unsolvable_constraints(
        const socket_handle& lhs_socket,
        object_ptr<const Type> lhs_type,
        const socket_handle& rhs_socket,
        object_ptr<const Type> rhs_type)
        : m_lhs_id {lhs_socket.id()}
        , m_lhs_type {std::move(lhs_type)}
        , m_rhs_id {rhs_socket.id()}
        , m_rhs_type {std::move(rhs_type)}
      {
      }

      [[nodiscard]] auto type() const
      {
        return compile_result_type::error;
      }

      /// Get error message
      [[nodiscard]] auto message() const -> std::string;

      /// socket of t1
      [[nodiscard]] auto& socket_id_lhs() const
      {
        return m_lhs_id;
      }

      /// socket of t2
      [[nodiscard]] auto& socket_id_rhs() const
      {
        return m_rhs_id;
      }

      /// type1
      [[nodiscard]] auto& lhs_type() const
      {
        return m_lhs_type;
      }

      /// type2
      [[nodiscard]] auto& rhs_type() const
      {
        return m_rhs_type;
      }

    private:
      /// src socket
      uid m_lhs_id;
      /// src socket
      uid m_rhs_id;
      /// lhs type
      object_ptr<const Type> m_lhs_type;
      /// rhs type
      object_ptr<const Type> m_rhs_type;
    };

    using compile_result = std::variant< //
      unexpected_error,
      no_valid_overloading,
      type_missmatch,
      unsolvable_constraints>;

  } // namespace compile_results

  using compile_results::compile_result;

  // get type
  [[nodiscard]] inline auto type(const compile_result& r) -> compile_result_type
  {
    return std::visit([](auto&& x) { return x.type(); }, r);
  }

  // get message
  [[nodiscard]] inline auto message(const compile_result& r) -> std::string
  {
    return std::visit([](auto&& x) { return x.message(); }, r);
  }

  // get node id
  [[nodiscard]] inline auto node_id(const compile_result& r) -> uid
  {
    return std::visit([](auto&& x) { return x.node_id(); }, r);
  }

  // get socket id
  [[nodiscard]] inline auto socket_id(const compile_result& r) -> uid
  {
    return std::visit([](auto&& x) { return x.socket_id(); }, r);
  }

} // namespace yave