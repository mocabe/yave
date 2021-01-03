//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/support/id.hpp>
#include <yave/node/core/node_handle.hpp>
#include <yave/node/core/socket_handle.hpp>
#include <yave/lib/util/variant_mixin.hpp>

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
      return "internal compile error: " + m_str;
    }

    [[nodiscard]] auto pretty_print(const structured_node_graph&) const
    {
      return text();
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

    [[nodiscard]] auto text() const -> std::string
    {
      return "Unexpected parse error occured: " + m_str;
    }

    [[nodiscard]] auto pretty_print(const structured_node_graph&) const
      -> std::string
    {
      return text();
    }

  private:
    std::string m_str;
  };

  /// Missing input connection to socket
  struct missing_input : message_error<message_category::parse>
  {
    missing_input(const uid& nid, const uid& sid)
      : m_node_id {nid}
      , m_socket_id {sid}
    {
    }

    [[nodiscard]] auto text() const -> std::string
    {
      return "Missing input connection";
    }

    [[nodiscard]] auto pretty_print(const structured_node_graph&) const
      -> std::string;

    [[nodiscard]] auto node_id() const -> const auto&
    {
      return m_node_id;
    }

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
    missing_output(const uid& nid, const uid& sid)
      : m_node_id {nid}
      , m_socket_id {sid}
    {
    }

    [[nodiscard]] auto text() const -> std::string
    {
      return "Missing output connection";
    }

    [[nodiscard]] auto pretty_print(const structured_node_graph&) const
      -> std::string;

    [[nodiscard]] auto node_id() const -> const auto&
    {
      return m_node_id;
    }

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
    has_default_argument(const uid& nid, const uid& sid)
      : m_node_id {nid}
      , m_socket_id {sid}
    {
    }

    [[nodiscard]] auto node_id() const -> const auto&
    {
      return m_node_id;
    }

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
    has_input_connection(const uid& nid, const uid& sid)
      : m_node_id {nid}
      , m_socket_id {sid}
    {
    }

    [[nodiscard]] auto node_id() const -> const auto&
    {
      return m_node_id;
    }

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
    has_output_connection(const uid& nid, const uid& sid)
      : m_node_id {nid}
      , m_socket_id {sid}
    {
    }

    [[nodiscard]] auto node_id() const -> const auto&
    {
      return m_node_id;
    }

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
    is_lambda_node(const uid& nid)
      : m_node_id {nid}
    {
    }

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

    [[nodiscard]] auto text() const -> std::string
    {
      return "Unexpected type error: " + m_str;
    }

    [[nodiscard]] auto pretty_print(const structured_node_graph&) const
      -> std::string
    {
      return text();
    }

  private:
    std::string m_str;
  };

  /// No valid overloading error
  struct no_valid_overloading : message_error<message_category::type>
  {
    no_valid_overloading(const uid& sid)
      : m_socket_id {sid}
    {
    }

    [[nodiscard]] auto text() const -> std::string
    {
      return "No valid orverloading";
    }

    [[nodiscard]] auto pretty_print(const structured_node_graph&) const
      -> std::string;

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
      const uid& sid_expected,
      const uid& sid_provided,
      object_ptr<const Type> expected,
      object_ptr<const Type> provided)
      : m_socket_expected_id {sid_expected}
      , m_socket_provided_id {sid_provided}
      , m_expected {std::move(expected)}
      , m_provided {std::move(provided)}
    {
    }

    [[nodiscard]] auto text() const -> std::string
    {
      return "Type missmatch";
    }

    [[nodiscard]] auto pretty_print(const structured_node_graph&) const
      -> std::string;

    [[nodiscard]] auto& socket_id_expected() const
    {
      return m_socket_expected_id;
    }

    [[nodiscard]] auto& socket_id_provided() const
    {
      return m_socket_expected_id;
    }

    [[nodiscard]] auto& expected_type() const
    {
      return m_expected;
    }

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
      const uid& lhs_sid,
      const uid& rhs_sid,
      object_ptr<const Type> lhs_type,
      object_ptr<const Type> rhs_type)
      : m_lhs_id {lhs_sid}
      , m_rhs_id {rhs_sid}
      , m_lhs_type {std::move(lhs_type)}
      , m_rhs_type {std::move(rhs_type)}
    {
    }

    [[nodiscard]] auto text() const -> std::string
    {
      return "Unsolvable constraint";
    }

    [[nodiscard]] auto pretty_print(const structured_node_graph&) const
      -> std::string;

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

    [[nodiscard]] auto text() const -> std::string
    {
      return "Invalid program output";
    }

    [[nodiscard]] auto pretty_print(const structured_node_graph&) const
      -> std::string;

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

  // clang-format off
  // Workaround for VS2019 Preview 16.8: requires-expression is broken
  template <class T>
  concept _msg_has_text = requires(T x) { x.text(); };
  template <class T>
  concept _msg_has_node_id = requires(T x) { x.node_id(); };
  template <class T>
  concept _msg_has_socket_id = requires(T x) { x.socket_id(); };
  template <class T> 
  concept _msg_has_pretty_print = requires(T x, const structured_node_graph& ng) { x.pretty_print(ng); };
  // clang-format on

  /// compiler message
  struct message : variant_mixin<
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
                     invalid_output_type>
  {
    using variant_mixin::variant_mixin;

    /// get kind of message
    [[nodiscard]] auto kind() const
    {
      return visit([](auto&& x) { return x.kind(); });
    }

    /// get category of message
    [[nodiscard]] auto category() const
    {
      return visit([](auto&& x) { return x.category(); });
    }

    /// get text message
    [[nodiscard]] auto get_text() const
    {
      return visit([](auto&& x) -> std::optional<std::string> {
        if constexpr (_msg_has_text<decltype(x)>)
          return x.text();
        return std::nullopt;
      });
    }

    /// get node location
    [[nodiscard]] auto get_node_id() const
    {
      return visit([](auto&& x) -> std::optional<uid> {
        if constexpr (_msg_has_node_id<decltype(x)>)
          return x.node_id();
        return std::nullopt;
      });
    }

    /// get socket location
    [[nodiscard]] auto get_socket_id() const
    {
      return visit([](auto&& x) -> std::optional<uid> {
        if constexpr (_msg_has_socket_id<decltype(x)>)
          return x.socket_id();
        return std::nullopt;
      });
    }

    [[nodiscard]] auto pretty_print(const structured_node_graph& ng)
    {
      return visit([&](auto&& x) -> std::optional<std::string> {
        if constexpr (_msg_has_pretty_print<decltype(x)>)
          return x.pretty_print(ng);
        return std::nullopt;
      });
    }
  };

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
