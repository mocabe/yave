//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/config/config.hpp>
#include <yave/obj/primitive/primitive.hpp>

#include <string>
#include <optional>

namespace yave {

  /// Node type
  enum class node_type
  {
    normal,    // normal node
    interface, // interface node
  };

  /// Node property class for node graph.
  class node_property
  {
  public:
    /// Non-primitive ctor.
    node_property(const std::string& name, node_type type);
    /// Copy ctor
    node_property(const node_property&);

    /// Normal node?
    [[nodiscard]] bool is_normal() const;
    /// Interface node?
    [[nodiscard]] bool is_interface() const;
    /// Get node name.
    [[nodiscard]] auto name() const -> const std::string&;
    /// Get node type
    [[nodiscard]] auto type() const -> node_type;
    /// visited?
    [[nodiscard]] bool is_visited() const;
    /// !visited?
    [[nodiscard]] bool is_unvisited() const;

    /// Has data?
    [[nodiscard]] bool has_data() const;
    /// Get data
    [[nodiscard]] auto get_data() const -> std::optional<object_ptr<Object>>;

    /// Set data
    void set_data(object_ptr<Object> data);

    /// Set name
    void set_name(const std::string& new_name);

    /// Set as visited.
    void set_visited() const;

    /// Set as unvisited.
    void set_unvisited() const;

  private:
    /// Name of node.
    std::string m_name;
    /// Node type
    const node_type m_type;
    /// Custom data
    std::optional<object_ptr<Object>> m_data;

  private:
    /// marking variable for dfs
    mutable bool m_visited;
  };

} // namespace yave