//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/core/node_declaration.hpp>
#include <yave/node/core/structured_node_graph.hpp>
#include <yave/node/core/properties.hpp>
#include <yave/support/overloaded.hpp>

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
      return std::string(sv.substr(pos + 1, sv.npos));
    }

    /// get path from full path
    auto get_node_path_component(const std::string& full_name)
    {
      std::string_view sv = full_name;
      auto pos            = sv.find_last_of('.');
      return std::string(sv.substr(0, pos));
    }

  } // namespace

  function_node_declaration::function_node_declaration(
    std::string full_name,
    std::string description,
    node_declaration_visibility visibility,
    std::vector<std::string> iss,
    std::vector<std::string> oss,
    std::vector<node_declaration_default_argument> default_arg)
    : m_name {std::move(full_name)}
    , m_dsc {std::move(description)}
    , m_vis {std::move(visibility)}
    , m_iss {std::move(iss)}
    , m_oss {std::move(oss)}
    , m_defargs {std::move(default_arg)}
  {
    if (!valid_node_path(m_name))
      throw std::invalid_argument("invalid node path: " + m_name);

    for (auto&& s : m_iss)
      if (!valid_socket_name(s))
        throw std::invalid_argument("invalid socket name: " + s);

    for (auto&& s : m_oss)
      if (!valid_socket_name(s))
        throw std::invalid_argument("invalid socket name: " + s);

    std::sort(m_defargs.begin(), m_defargs.end(), [](auto&& lhs, auto&& rhs) {
      return lhs.idx < rhs.idx;
    });

    auto it = std::unique(
      m_defargs.begin(), m_defargs.end(), [](auto&& lhs, auto&& rhs) {
        return lhs.idx == rhs.idx;
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
    initializer_func_t init_func,
    std::vector<node_declaration_default_argument> args)
    : m_name {std::move(full_name)}
    , m_dsc {std::move(description)}
    , m_vis {std::move(visibility)}
    , m_iss {std::move(iss)}
    , m_oss {std::move(oss)}
    , m_func {std::move(init_func)}
    , m_defargs {std::move(args)}
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
    expand_func_t expand_func)
    : m_name {std::move(full_name)}
    , m_dsc {std::move(description)}
    , m_vis {std::move(visibility)}
    , m_iss {std::move(iss)}
    , m_oss {std::move(oss)}
    , m_expand_func {std::move(expand_func)}
  {
    if (!valid_node_path(m_name))
      throw std::invalid_argument("invalid node path");

    for (auto&& s : m_iss)
      if (!valid_socket_name(s))
        throw std::invalid_argument("invalid socket name");

    for (auto&& s : m_oss)
      if (!valid_socket_name(s))
        throw std::invalid_argument("invalid socket name");

    if (!m_expand_func)
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

  auto create_declaration(
    structured_node_graph& ng,
    const std::shared_ptr<const node_declaration>& decl) -> node_handle
  {
    return decl->visit( //
      overloaded        //
      {[&](const composed_node_declaration& d) {
         auto n = ng.create_group(
           d.node_path(), d.node_name(), d.input_sockets(), d.output_sockets());

         if (n) {

           // init composed group
           if (!d.init_composed(ng, n)) {
             ng.destroy(n);
             return node_handle();
           }

           // set default args (init time only)
           for (auto&& [idx, arg] : d.default_args()) {
             set_arg(ng, ng.input_sockets(n)[idx], arg.clone());
           }
         }
         return n;
       },
       [&](const function_node_declaration& d) {
         auto n = ng.create_function(
           d.node_path(), d.node_name(), d.input_sockets(), d.output_sockets());

         if (n) {
           // set default args
           for (auto&& [idx, arg] : d.default_args()) {
             set_arg(ng, ng.input_sockets(n)[idx], arg.clone());
           }
         }
         return n;
       },
       [&](const macro_node_declaration& d) {
         auto n = ng.create_macro(
           d.node_path(), d.node_name(), d.input_sockets(), d.output_sockets());
         return n;
       }});
  }
} // namespace yave