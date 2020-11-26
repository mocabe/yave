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

  /// definition map
  class node_definition_map
  {
    using map_t =
      std::multimap<std::string, std::shared_ptr<const node_definition>>;

    map_t m_map;

  public:
    node_definition_map()                               = default;
    node_definition_map(const node_definition_map&)     = default;
    node_definition_map(node_definition_map&&) noexcept = default;
    node_definition_map& operator=(const node_definition_map&) = default;
    node_definition_map& operator=(node_definition_map&&) noexcept = default;

  public:
    /// Add definition
    [[nodiscard]] bool add(const node_definition& def);

    /// Remove definitions
    void remove(const std::string& full_name);

    /// Exists?
    [[nodiscard]] bool exists(const std::string& full_name) const;

    /// Exists?
    [[nodiscard]] bool exists(
      const std::string& full_name,
      const size_t& output_socket) const;

    /// Find definition
    [[nodiscard]] auto find(const std::string& full_name) const
      -> std::vector<std::shared_ptr<const node_definition>>;

    /// Get compatible binds
    [[nodiscard]] auto get_binds(
      const std::string& qualified_name,
      const size_t& output_socket) const
      -> std::vector<std::shared_ptr<const node_definition>>;

    // size
    [[nodiscard]] auto size() const -> size_t;

    /// empty?
    [[nodiscard]] bool empty() const;

    /// Clear
    void clear();
  };

  /// dual of node_declaration_store
  class node_definition_store
  {
    /// map
    node_definition_map m_map;

  public:
    /// Constructor
    node_definition_store();

    /// Copy ctor (deleted)
    node_definition_store(const node_definition_store&) = delete;
    /// Copy assign op (deleted)
    node_definition_store& operator=(const node_definition_store&) = delete;

    /// Move ctor
    node_definition_store(node_definition_store&&) noexcept;
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

    /// Get (path, def) map
    [[nodiscard]] auto get_map() const -> const node_definition_map&;

    // size
    [[nodiscard]] auto size() const -> size_t;

    /// Clear
    void clear();

    /// empty?
    bool empty() const;
  };

} // namespace yave