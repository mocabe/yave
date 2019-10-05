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
    normal,
    primitive,
    interface,
  };

  /// Node property class for node graph.
  class node_property
  {
  public:
    struct normal_construct_t
    {
    };

    struct primitive_construct_t
    {
    };

    struct interface_construct_t
    {
    };

  public:
    /// Non-primitive ctor.
    node_property(normal_construct_t, const std::string& name);
    /// Primitive ctor.
    node_property(
      primitive_construct_t,
      const std::string& name,
      const primitive_t& prim);
    /// Interface construct
    node_property(interface_construct_t, const std::string& name);

    /// Normal node?
    [[nodiscard]] bool is_normal() const;
    /// Primitive node?
    [[nodiscard]] bool is_prim() const;
    /// Interface node?
    [[nodiscard]] bool is_interface() const;
    /// Get node type
    [[nodiscard]] auto get_node_type() const -> node_type;

    /// Get node name.
    [[nodiscard]] auto name() const -> const std::string&;

    /// Get primitive value.
    /// \requires is_prim() == true
    [[nodiscard]] auto get_prim() const -> std::optional<primitive_t>;

    /// Get shared primitive container.
    [[nodiscard]] auto get_shared_prim() const
      -> object_ptr<PrimitiveContainer>;

    /// visited?
    [[nodiscard]] bool is_visited() const;

    /// unvisited?
    [[nodiscard]] bool is_unvisited() const;

    /// Set primitive value.
    void set_prim(const primitive_t& prim);

    /// Unset primitive value.
    void ser_prim(std::nullopt_t);

    /// Set as visited.
    void set_visited() const;

    /// Set as unvisited.
    void set_unvisited() const;

  private:
    /// Name of node.
    const std::string m_name;
    /// Node type
    node_type m_type;
    /// Primitive value.
    /// If the node is not primitive node, stores std::monostate.
    std::optional<object_ptr<PrimitiveContainer>> m_prim;
    /// marking variable for dfs
    mutable bool m_visited;
  };

} // namespace yave