//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/node/core/node_declaration.hpp>

#include <regex>
#include <string_view>

namespace yave {

  auto node_declaration::node_path() const -> std::string
  {
    std::string_view sv = m_name;
    auto pos            = sv.find_last_of('.');
    return std::string(sv.substr(0, pos));
  }

  auto node_declaration::node_name() const -> std::string
  {
    std::string_view sv = m_name;
    auto pos            = sv.find_last_of('.');
    return std::string(sv.substr(pos + 1, sv.npos));
  }

  void node_declaration::_validate()
  {
    std::sort(
      m_default_values.begin(),
      m_default_values.end(),
      [](auto&& lhs, auto&& rhs) { return lhs.first < rhs.first; });

    auto it = std::unique(
      m_default_values.begin(),
      m_default_values.end(),
      [](auto&& lhs, auto&& rhs) { return lhs.first == rhs.first; });

    if (it != m_default_values.end())
      throw std::invalid_argument("Default value should be unique");

    for (auto&& [idx, defval] : m_default_values) {
      if (idx >= m_iss.size())
        throw std::invalid_argument("Invalid index for default value");
      if (!defval)
        throw std::invalid_argument("Null default value");
    }

    // requires '.' delimited path
    static auto re = std::regex(R"(^(\w+)(\.\w+)*$)");

    if (!std::regex_match(m_name, re))
      throw std::invalid_argument("Invalid name format");

    if (m_initializer)
      assert(m_default_values.empty());
  }
} // namespace yave