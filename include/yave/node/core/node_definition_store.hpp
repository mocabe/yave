//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/node_definition.hpp>

#include <functional>
#include <map>
#include <memory>

namespace yave {

  /// Node definition store
  class node_definition_store
  {
    /// map type
    using map_type =
      std::multimap<std::string, std::shared_ptr<node_definition>>;

  public:
    /// Constructor
    node_definition_store();

    /// Copy ctor
    node_definition_store(const node_definition_store&);
    /// Move ctor
    node_definition_store(node_definition_store&&) noexcept;

    /// Copy assign op
    node_definition_store& operator=(const node_definition_store&);
    /// Move assign op
    node_definition_store& operator=(node_definition_store&&) noexcept;

    /// Add definition
    [[nodiscard]] bool add(const node_definition& def);
    /// Add definitions
    [[nodiscard]] bool add(const std::vector<node_definition>& defs);
    /// Remove definitions
    void remove(const std::string& qualified_name);
    /// Remove definitions
    void remove(const std::vector<std::string>& qualified_name);

    /// Exists?
    [[nodiscard]] bool exists(const std::string& qualified_name) const;
    /// Exists?
    [[nodiscard]] bool exists(
      const std::string& qualified_name,
      const size_t& output_socket) const;

    /// Find definition
    [[nodiscard]] auto find(const std::string& qualified_name) const
      -> std::vector<std::shared_ptr<const node_definition>>;

    /// Get compatible binds
    [[nodiscard]] auto get_binds(
      const std::string& qualified_name,
      const size_t& output_socket) const
      -> std::vector<std::shared_ptr<const node_definition>>;

    // size
    [[nodiscard]] auto size() const -> size_t;

    /// Clear
    void clear();

    /// empty?
    bool empty() const;

  private:
    /// map
    map_type m_map;
  };

} // namespace yave