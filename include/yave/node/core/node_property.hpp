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

  /// Node property class for node graph.
  class node_property
  {
  public:
    /// Primitive ctor.
    node_property(const std::string& name, const primitive_t& prim);
    /// Non-primitive ctor.
    node_property(const std::string& name, std::monostate);

    /// Check primitive node.
    bool is_prim() const;

    /// Get node name.
    [[nodiscard]] const std::string& name() const;

    /// Get primitive value.
    /// \requires is_prim() == true
    [[nodiscard]] std::optional<primitive_t> get_prim() const;

    /// Set primitive value.
    void set_prim(const primitive_t& prim);

    /// Unset primitive value.
    void ser_prim(std::nullopt_t);

    /// Get shared primitive container.
    [[nodiscard]] object_ptr<PrimitiveContainer> get_shared_prim() const;

    /// visited?
    bool is_visited() const
    {
      return m_visited == 1;
    }

    /// unvisited?
    bool is_unvisited() const
    {
      return m_visited == 0;
    }

    /// Set as visited.
    void set_visited() const
    {
      m_visited = 1;
    }

    /// Set as unvisited.
    void set_unvisited() const
    {
      m_visited = 0;
    }

  private:
    /// Name of node.
    const std::string m_name;
    /// Primitive value.
    /// If the node is not primitive node, stores std::monostate.
    std::optional<object_ptr<PrimitiveContainer>> m_prim;
    /// marking variable for dfs
    mutable uint8_t m_visited;
  };

} // namespace yave