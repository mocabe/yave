//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/core/node_declaration.hpp>
#include <yave/node/core/structured_node_graph.hpp>

#include <regex>
#include <string_view>

namespace yave {

  namespace {

    /// validate node path
    bool valid_node_path(const std::string& path)
    {
      static const auto re = std::regex(structured_node_graph::path_name_regex);
      return std::regex_match(path, re);
    }

    /// validate socket name
    bool valid_socket_name(const std::string& name)
    {
      static const auto re =
        std::regex(structured_node_graph::socket_name_regex);
      return std::regex_match(name, re);
    }

    /// get name from full path
    auto get_node_name_component(const std::string& full_name)
    {
      std::string_view sv = full_name;
      auto pos            = sv.find_last_of('.');
      return std::string(sv.substr(0, pos));
    }

    /// get path from full path
    auto get_node_path_component(const std::string& full_name)
    {
      std::string_view sv = full_name;
      auto pos            = sv.find_last_of('.');
      return std::string(sv.substr(pos + 1, sv.npos));
    }

  } // namespace

  auto get_full_name(const node_declaration& decl) -> const std::string&
  {
    return std::visit(
      [](auto& d) -> auto& { return d.full_name(); }, decl);
  }

  auto get_node_name(const node_declaration& decl) -> std::string
  {
    return std::visit([](auto& d) { return d.node_name(); }, decl);
  }

  auto get_node_path(const node_declaration& decl) -> std::string
  {
    return std::visit([](auto& d) { return d.node_path(); }, decl);
  }

  auto get_description(const node_declaration& decl) -> const std::string&
  {
    return std::visit(
      [](auto& d) -> auto& { return d.description(); }, decl);
  }

  auto get_input_sockets(const node_declaration& decl)
    -> const std::vector<std::string>&
  {
    return std::visit(
      [](auto& d) -> auto& { return d.input_sockets(); }, decl);
  }

  auto get_output_sockets(const node_declaration& decl)
    -> const std::vector<std::string>&
  {
    return std::visit(
      [](auto& d) -> auto& { return d.output_sockets(); }, decl);
  }

  auto get_visibility(const node_declaration& decl)
    -> node_declaration_visibility
  {
    return std::visit(
      [](auto& d) -> auto& { return d.visibility(); }, decl);
  }

  function_node_declaration::function_node_declaration(
    std::string full_name,
    std::string description,
    node_declaration_visibility visibility,
    std::vector<std::string> iss,
    std::vector<std::string> oss,
    std::vector<std::pair<size_t, object_ptr<Object>>> default_arg)
    : m_name {std::move(full_name)}
    , m_dsc {std::move(description)}
    , m_vis {std::move(visibility)}
    , m_iss {std::move(iss)}
    , m_oss {std::move(oss)}
    , m_defargs {std::move(default_arg)}
  {
    if (!valid_node_path(m_name))
      throw std::invalid_argument("invalid node path");

    for (auto&& s : m_iss)
      if (!valid_socket_name(s))
        throw std::invalid_argument("invalid socket name");

    for (auto&& s : m_oss)
      if (!valid_socket_name(s))
        throw std::invalid_argument("invalid socket name");

    std::sort(m_defargs.begin(), m_defargs.end(), [](auto&& lhs, auto&& rhs) {
      return lhs.first < rhs.first;
    });

    auto it = std::unique(
      m_defargs.begin(), m_defargs.end(), [](auto&& lhs, auto&& rhs) {
        return lhs.first == rhs.first;
      });

    if (it != m_defargs.end())
      throw std::invalid_argument("default value should be unique");
  }

  auto function_node_declaration::node_name() const -> std::string
  {
    return get_node_name_component(m_name);
  }

  auto function_node_declaration::node_path() const -> std::string
  {
    return get_node_path_component(m_name);
  }

  composed_node_declaration::composed_node_declaration(
    std::string full_name,
    std::string description,
    node_declaration_visibility visibility,
    std::vector<std::string> iss,
    std::vector<std::string> oss,
    initializer_func init_func)
    : m_name {std::move(full_name)}
    , m_dsc {std::move(description)}
    , m_vis {std::move(visibility)}
    , m_iss {std::move(iss)}
    , m_oss {std::move(oss)}
    , m_func {std::move(init_func)}
  {
    if (!valid_node_path(m_name))
      throw std::invalid_argument("invalid node path");

    for (auto&& s : m_iss)
      if (!valid_socket_name(s))
        throw std::invalid_argument("invalid socket name");

    for (auto&& s : m_oss)
      if (!valid_socket_name(s))
        throw std::invalid_argument("invalid socket name");

    if (!m_func)
      throw std::invalid_argument("null initialier function");
  }

  auto composed_node_declaration::node_name() const -> std::string
  {
    return get_node_name_component(m_name);
  }

  auto composed_node_declaration::node_path() const -> std::string
  {
    return get_node_path_component(m_name);
  }

  macro_node_declaration::macro_node_declaration(
    std::string full_name,
    std::string description,
    node_declaration_visibility visibility,
    std::vector<std::string> iss,
    std::vector<std::string> oss,
    std::unique_ptr<abstract_macro_func> macro_func)
    : m_name {std::move(full_name)}
    , m_dsc {std::move(description)}
    , m_vis {std::move(visibility)}
    , m_iss {std::move(iss)}
    , m_oss {std::move(oss)}
    , m_func {std::move(macro_func)}
  {
    if (!valid_node_path(m_name))
      throw std::invalid_argument("invalid node path");

    for (auto&& s : m_iss)
      if (!valid_socket_name(s))
        throw std::invalid_argument("invalid socket name");

    for (auto&& s : m_oss)
      if (!valid_socket_name(s))
        throw std::invalid_argument("invalid socket name");

    if (!m_func)
      throw std::invalid_argument("null macro function");
  }

  auto macro_node_declaration::node_name() const -> std::string
  {
    return get_node_name_component(m_name);
  }

  auto macro_node_declaration::node_path() const -> std::string
  {
    return get_node_path_component(m_name);
  }

} // namespace yave