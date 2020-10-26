//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/dynamic_typing.hpp>
#include <yave/node/core/node_handle.hpp>
#include <yave/node/core/socket_handle.hpp>

#include <functional>
#include <memory>
#include <variant>

namespace yave {

  class structured_node_graph;

  /// visibiliyy of node declaration
  enum class node_declaration_visibility
  {
    /// visible from user
    _public,
    /// not visible from uset
    _private,
  };

  /// node declaration for builtin functions
  class function_node_declaration
  {
  public:
    /// \param full_name full node name including module names
    /// \param description description of node
    /// \param visibility visibility of node
    /// \param iss input sockets
    /// \param oss output sockets
    /// \param default_arg list of default arguments
    function_node_declaration(
      std::string full_name,
      std::string description,
      node_declaration_visibility visibility,
      std::vector<std::string> iss,
      std::vector<std::string> oss,
      std::vector<std::pair<size_t, object_ptr<Object>>> default_arg = {});

    [[nodiscard]] auto& full_name() const
    {
      return m_name;
    }

    [[nodiscard]] auto& description() const
    {
      return m_dsc;
    }

    [[nodiscard]] auto& visibility() const
    {
      return m_vis;
    }

    [[nodiscard]] auto& input_sockets() const
    {
      return m_iss;
    }

    [[nodiscard]] auto& output_sockets() const
    {
      return m_oss;
    }

    [[nodiscard]] auto& default_args() const
    {
      return m_defargs;
    }

    /// get name component from full name
    [[nodiscard]] auto node_name() const -> std::string;
    /// get path component from full name
    [[nodiscard]] auto node_path() const -> std::string;

  private:
    std::string m_name;
    std::string m_dsc;
    node_declaration_visibility m_vis;
    std::vector<std::string> m_iss;
    std::vector<std::string> m_oss;
    std::vector<std::pair<size_t, object_ptr<Object>>> m_defargs;
  };

  /// node declaration for composed function
  class composed_node_declaration
  {
  public:
    /// callback to initialize composed node declaration.
    /// this callback fills pre-initialized node gruop given as parameter.
    using initializer_func =
      std::function<void(structured_node_graph&, node_handle)>;

    /// \param full_name full name of node
    /// \param description description of node
    /// \param visibility visibility of node
    /// \param iss input sockets
    /// \param oss output sockets
    /// \param init_func initializer function
    composed_node_declaration(
      std::string full_name,
      std::string description,
      node_declaration_visibility visibility,
      std::vector<std::string> iss,
      std::vector<std::string> oss,
      initializer_func init_func);

    [[nodiscard]] auto& full_name() const
    {
      return m_name;
    }

    [[nodiscard]] auto& description() const
    {
      return m_dsc;
    }

    [[nodiscard]] auto& visibility() const
    {
      return m_vis;
    }

    [[nodiscard]] auto& input_sockets() const
    {
      return m_iss;
    }

    [[nodiscard]] auto& output_sockets() const
    {
      return m_oss;
    }

    /// call initializer func
    void init_composed(structured_node_graph& g, node_handle n) const
    {
      m_func(g, n);
    }

    /// get name component from full name
    [[nodiscard]] auto node_name() const -> std::string;
    /// get path component from full name
    [[nodiscard]] auto node_path() const -> std::string;

  private:
    std::string m_name;
    std::string m_dsc;
    node_declaration_visibility m_vis;
    std::vector<std::string> m_iss;
    std::vector<std::string> m_oss;
    initializer_func m_func;
  };

  /// node declaration for macro definitions
  class macro_node_declaration
  {
  public:
    /// 'macro's in yave are defined as set of event handlers.
    struct abstract_macro_func
    {
      // clang-format off

      virtual ~abstract_macro_func() noexcept = default;

      /// called when macro node is initialized.
      virtual void on_create(structured_node_graph& g, const node_handle& n) = 0;
      /// called when macro node is destroyed.
      virtual void on_destroy(structured_node_graph& g, const node_handle& n) = 0;
      /// called when new socket is connected to macro node.
      virtual void on_connect(structured_node_graph& g, const socket_handle& s) = 0;
      /// called when socket is disconnected.
      virtual void on_disconnect(structured_node_graph& g, const socket_handle& s) = 0;
      /// called when macro node is expanded
      virtual void on_expand(structured_node_graph& g, const node_handle& n) = 0;

      // clang-format on
    };

    /// \param full_name full name of node
    /// \param description description of node
    /// \param visibility visibility of node
    /// \param iss input sockets
    /// \param oss output sockets
    /// \param macro_func macro function object
    macro_node_declaration(
      std::string full_name,
      std::string description,
      node_declaration_visibility visibility,
      std::vector<std::string> iss,
      std::vector<std::string> oss,
      std::unique_ptr<abstract_macro_func> macro_func);

    [[nodiscard]] auto& full_name() const
    {
      return m_name;
    }

    [[nodiscard]] auto& description() const
    {
      return m_dsc;
    }

    [[nodiscard]] auto& visibility() const
    {
      return m_vis;
    }

    [[nodiscard]] auto& input_sockets() const
    {
      return m_iss;
    }

    [[nodiscard]] auto& output_sockets() const
    {
      return m_oss;
    }

    /// call macro callback
    void macro_on_create(structured_node_graph& g, node_handle n) const
    {
      m_func->on_create(g, n);
    }

    /// call macro callback
    void macro_on_destroy(structured_node_graph& g, node_handle n) const
    {
      m_func->on_destroy(g, n);
    }

    /// call macro callback
    void macro_on_connect(structured_node_graph& g, socket_handle s) const
    {
      m_func->on_connect(g, s);
    }

    /// call macro callback
    void macro_on_disconnect(structured_node_graph& g, socket_handle s) const
    {
      m_func->on_disconnect(g, s);
    }

    /// call macro callback
    void macro_on_expand(structured_node_graph& g, node_handle n) const
    {
      m_func->on_expand(g, n);
    }

    /// get name component from full name
    [[nodiscard]] auto node_name() const -> std::string;
    /// get path component from full name
    [[nodiscard]] auto node_path() const -> std::string;

  private:
    std::string m_name;
    std::string m_dsc;
    node_declaration_visibility m_vis;
    std::vector<std::string> m_iss;
    std::vector<std::string> m_oss;
    std::shared_ptr<abstract_macro_func> m_func;
  };

  /// node declaration
  using node_declaration = std::variant<
    function_node_declaration,
    composed_node_declaration,
    macro_node_declaration>;

  /// get full name of declaration
  [[nodiscard]] auto full_name_of(const node_declaration& decl)
    -> const std::string&;

  /// get name component of node declaration
  [[nodiscard]] auto node_name_of(const node_declaration& decl) -> std::string;

  /// get path component of node declaration
  [[nodiscard]] auto node_path_of(const node_declaration& decl) -> std::string;

  /// get description
  [[nodiscard]] auto description_of(const node_declaration& decl)
    -> const std::string&;

  /// get input sockets
  [[nodiscard]] auto input_sockets_of(const node_declaration& decl)
    -> const std::vector<std::string>&;

  /// get output sockets
  [[nodiscard]] auto output_sockets_of(const node_declaration& decl)
    -> const std::vector<std::string>&;

  /// get node visibility
  [[nodiscard]] auto visibility_of(const node_declaration& decl)
    -> node_declaration_visibility;

} // namespace yave