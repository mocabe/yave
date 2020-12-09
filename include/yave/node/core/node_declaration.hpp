//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/dynamic_typing.hpp>
#include <yave/node/core/node_handle.hpp>
#include <yave/node/core/socket_handle.hpp>
#include <yave/lib/util/variant_mixin.hpp>
#include <yave/obj/node/argument.hpp>

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

  /// default argument data
  struct node_declaration_default_argument
  {
    /// index of input socket
    size_t idx;
    /// node argument data
    object_ptr<NodeArgument> arg;
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
      std::vector<node_declaration_default_argument> default_arg = {});

    function_node_declaration(const function_node_declaration&)     = default;
    function_node_declaration(function_node_declaration&&) noexcept = default;

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
    std::vector<node_declaration_default_argument> m_defargs;
  };

  /// node declaration for composed function
  class composed_node_declaration
  {
  public:
    /// callback to initialize composed node declaration.
    /// this callback fills pre-initialized node gruop given as parameter.
    /// should not throw exception
    using initializer_func_t =
      std::function<bool(structured_node_graph&, node_handle)>;

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
      initializer_func_t init_func,
      std::vector<node_declaration_default_argument> defargs = {});

    composed_node_declaration(const composed_node_declaration&)     = default;
    composed_node_declaration(composed_node_declaration&&) noexcept = default;

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

    /// call initializer func
    [[nodiscard]] bool init_composed(structured_node_graph& g, node_handle n)
      const noexcept
    {
      return m_func(g, n);
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
    initializer_func_t m_func;
    std::vector<node_declaration_default_argument> m_defargs;
  };

  /// node declaration for macro definitions
  class macro_node_declaration
  {
  public:
    /// callback will be called when macro node is expanded.
    /// should not throw exception
    using expand_func_t =
      std::function<node_handle(structured_node_graph&, const node_handle&)>;

    /// \param full_name full name of node
    /// \param description description of node
    /// \param visibility visibility of node
    /// \param iss input sockets
    /// \param oss output sockets
    /// \param expand_func macro function object
    macro_node_declaration(
      std::string full_name,
      std::string description,
      node_declaration_visibility visibility,
      std::vector<std::string> iss,
      std::vector<std::string> oss,
      expand_func_t expand_func);

    macro_node_declaration(const macro_node_declaration&)     = default;
    macro_node_declaration(macro_node_declaration&&) noexcept = default;

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
    auto macro_on_expand(structured_node_graph& g, node_handle n) const noexcept
      -> node_handle
    {
      return m_expand_func(g, n);
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
    expand_func_t m_expand_func;
  };

  /// node declaration
  struct node_declaration : variant_mixin<
                              function_node_declaration,
                              composed_node_declaration,
                              macro_node_declaration>
  {
    using variant_mixin::variant_mixin;

    /// get full name of declaration
    [[nodiscard]] auto full_name() const -> const std::string&
    {
      return visit([](auto& d) -> auto& { return d.full_name(); });
    }

    /// get name component of node declaration
    [[nodiscard]] auto node_name() const -> std::string
    {
      return visit([](auto& d) { return d.node_name(); });
    }

    /// get path component of node declaration
    [[nodiscard]] auto node_path() const -> std::string
    {
      return visit([](auto& d) { return d.node_path(); });
    }

    /// get description
    [[nodiscard]] auto description() const -> const std::string&
    {
      return visit([](auto& d) -> auto& { return d.description(); });
    }

    /// get input sockets
    [[nodiscard]] auto input_sockets() const -> const std::vector<std::string>&
    {
      return visit([](auto& d) -> auto& { return d.input_sockets(); });
    }

    /// get output sockets
    [[nodiscard]] auto output_sockets() const -> const std::vector<std::string>&
    {
      return visit([](auto& d) -> auto& { return d.output_sockets(); });
    }

    /// get node visibility
    [[nodiscard]] auto visibility() const -> node_declaration_visibility
    {
      return visit([](auto& d) { return d.visibility(); });
    }

    /// public?
    [[nodiscard]] auto is_public() const
    {
      return visibility() == node_declaration_visibility::_public;
    }

    /// private?
    [[nodiscard]] auto is_private() const
    {
      return visibility() == node_declaration_visibility::_private;
    }
  };

  /// Create new declaration
  /// \param ng node graph
  /// \param decl new function declaration
  /// \note decl should have unique name, otherwise will fail
  /// \returns null handle when failed
  [[nodiscard]] auto create_declaration(
    structured_node_graph& ng,
    const std::shared_ptr<const node_declaration>& decl) -> node_handle;

} // namespace yave