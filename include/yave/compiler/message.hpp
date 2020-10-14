//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/support/id.hpp>
#include <yave/node/core/node_handle.hpp>
#include <yave/node/core/socket_handle.hpp>

#include <variant>
#include <string>
#include <optional>
#include <memory>

namespace yave {
  class structured_node_graph;
}

namespace yave::compiler {

  enum class message_kind
  {
    info,
    warning,
    error,
  };

  enum class message_category
  {
    other,
    parse,
    type,
  };

  template <message_kind K, message_category C>
  struct message_base
  {
    auto kind() const
    {
      return K;
    }

    auto category() const
    {
      return C;
    }
  };

  template <message_category Category>
  using message_info = message_base<message_kind::info, Category>;

  template <message_category Category>
  using message_warning = message_base<message_kind::warning, Category>;

  template <message_category Category>
  using message_error = message_base<message_kind::error, Category>;

  /// Internal compiler error
  struct internal_compile_error : message_error<message_category::other>
  {
    internal_compile_error(std::string str)
      : m_str {std::move(str)}
    {
    }

    [[nodiscard]] auto text() const
    {
      return std::string("internal compile error: " + m_str);
    }

  private:
    std::string m_str;
  };

  /// Unexpected parse error
  struct unexpected_parse_error : message_error<message_category::parse>
  {
    unexpected_parse_error(std::string str)
      : m_str {std::move(str)}
    {
    }

    [[nodiscard]] auto text() const -> std::string;

  private:
    std::string m_str;
  };

  /// Missing input connection to socket
  struct missing_input : message_error<message_category::parse>
  {
    missing_input(const node_handle& node, const socket_handle& socket)
      : m_node_id {node.id()}
      , m_socket_id {socket.id()}
    {
    }

    /// Error message
    [[nodiscard]] auto text() const -> std::string;

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
  struct missing_output : message_error<message_category::parse>
  {
    missing_output(const node_handle& node, const socket_handle& socket)
      : m_node_id {node.id()}
      , m_socket_id {socket.id()}
    {
    }

    /// Error message
    [[nodiscard]] auto text() const -> std::string;

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
  struct has_default_argument : message_info<message_category::parse>
  {
    has_default_argument(const node_handle& node, const socket_handle& socket)
      : m_node_id {node.id()}
      , m_socket_id {socket.id()}
    {
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
  struct has_input_connection : message_info<message_category::parse>
  {
    has_input_connection(const node_handle& node, const socket_handle& socket)
      : m_node_id {node.id()}
      , m_socket_id {socket.id()}
    {
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
  struct has_output_connection : message_info<message_category::parse>
  {
    has_output_connection(const node_handle& node, const socket_handle& socket)
      : m_node_id {node.id()}
      , m_socket_id {socket.id()}
    {
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
  struct is_lambda_node : message_info<message_category::parse>
  {
    is_lambda_node(const node_handle& node)
      : m_node_id {node.id()}
    {
    }

    /// Get node ID
    [[nodiscard]] auto node_id() const -> const auto&
    {
      return m_node_id;
    }

  private:
    uid m_node_id;
  };

  struct unexpected_type_error : message_error<message_category::type>
  {
    unexpected_type_error(std::string str)
      : m_str {std::move(str)}
    {
    }

    [[nodiscard]] auto text() const -> std::string;

  private:
    std::string m_str;
  };

  /// No valid overloading error
  struct no_valid_overloading : message_error<message_category::type>
  {
    no_valid_overloading(const socket_handle& socket)
      : m_socket_id {socket.id()}
    {
    }

    /// Get error message
    [[nodiscard]] auto text() const -> std::string;

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
  struct type_missmatch : message_error<message_category::type>
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

    /// Get error message
    [[nodiscard]] auto text() const -> std::string;

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
  struct unsolvable_constraints : message_error<message_category::type>
  {
    unsolvable_constraints(
      const socket_handle& lhs_socket,
      object_ptr<const Type> lhs_type,
      const socket_handle& rhs_socket,
      object_ptr<const Type> rhs_type)
      : m_lhs_id {lhs_socket.id()}
      , m_rhs_id {rhs_socket.id()}
      , m_lhs_type {std::move(lhs_type)}
      , m_rhs_type {std::move(rhs_type)}
    {
    }

    /// Get error message
    [[nodiscard]] auto text() const -> std::string;

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

  struct invalid_output_type : message_error<message_category::other>
  {
    invalid_output_type(
      object_ptr<const Type> expected,
      object_ptr<const Type> provided)
      : m_expected {std::move(expected)}
      , m_provided {std::move(provided)}
    {
    }

    /// Get error message
    [[nodiscard]] auto text() const -> std::string;

    auto& expected_type() const
    {
      return m_expected;
    }

    auto& provided_type() const
    {
      return m_expected;
    }

  private:
    object_ptr<const Type> m_expected;
    object_ptr<const Type> m_provided;
  };

  /// compile message
  using message = std::variant<
    internal_compile_error,
    // parse
    unexpected_parse_error,
    missing_input,
    missing_output,
    is_lambda_node,
    has_default_argument,
    has_input_connection,
    has_output_connection,
    // type
    unexpected_type_error,
    no_valid_overloading,
    type_missmatch,
    unsolvable_constraints,
    // verify
    invalid_output_type>;

  /// get kind of message
  [[nodiscard]] inline auto kind(const message& msg)
  {
    return std::visit([](auto&& x) { return x.kind(); }, msg);
  }

  /// get category of message
  [[nodiscard]] inline auto category(const message& msg)
  {
    return std::visit([](auto&& x) { return x.category(); }, msg);
  }

  // clang-format off
  // Workaround for VS2019 Preview 16.8: requires-expression is broken
  template <class T>
  concept _msg_has_text = requires(T x) { x.text(); };
  template <class T>
  concept _msg_has_node_id = requires(T x) { x.node_id(); };
  template <class T>
  concept _msg_has_socket_id = requires(T x) { x.socket_id(); };
  // clang-format on

  /// get text message
  [[nodiscard]] inline auto text(const message& msg)
  {
    return std::visit(
      [](auto&& x) -> std::optional<std::string> {
        if constexpr (_msg_has_text<decltype(x)>)
          return x.text();
        return std::nullopt;
      },
      msg);
  }

  /// get node location
  [[nodiscard]] inline auto node_id(const message& msg)
  {
    return std::visit(
      [](auto&& x) -> std::optional<uid> {
        if constexpr (_msg_has_node_id<decltype(x)>)
          return x.node_id();
        return std::nullopt;
      },
      msg);
  }

  /// get socket location
  [[nodiscard]] inline auto socket_id(const message& msg)
  {
    return std::visit(
      [](auto&& x) -> std::optional<uid> {
        if constexpr (_msg_has_socket_id<decltype(x)>)
          return x.socket_id();
        return std::nullopt;
      },
      msg);
  }

  /// Container of messages
  class message_map
  {
    class impl;
    std::unique_ptr<impl> m_pimpl;

  public:
    message_map();
    ~message_map() noexcept;
    message_map(message_map&&) noexcept;
    message_map& operator=(message_map&&) noexcept;

  public:
    /// add message
    void add(message msg);

  public:
    /// contains error?
    [[nodiscard]] bool has_error() const;

  public:
    /// filter messages
    [[nodiscard]] auto get_errors() const -> std::vector<message>;
    [[nodiscard]] auto get_warnings() const -> std::vector<message>;
    [[nodiscard]] auto get_infos() const -> std::vector<message>;

    /// get all messages
    auto get_results() const -> std::vector<message>;

    /// get reuslts associated to node.
    /// \param ng reference node graph
    /// \param n  target node
    auto get_results(const structured_node_graph& ng, const node_handle& n)
      const -> std::vector<message>;

    /// get reuslt associated to socket.
    /// \param ng referenec node graph
    /// \param s target socket
    auto get_results(const structured_node_graph& ng, const socket_handle& s)
      const -> std::vector<message>;
  };

} // namespace yave::compiler
