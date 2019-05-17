//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/core/config.hpp>
#include <yave/core/data_types/primitive.hpp>

#include <string>
#include <variant>

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
    [[nodiscard]] primitive_t prim() const;

    /// Set primitive value.
    void set_prim(const primitive_t& prim);

    /// Unset primitive value.
    void ser_prim(std::monostate);

  private:
    /// Name of node.
    const std::string m_name;
    /// Primitive value.
    /// If the node is not primitive node, stores std::monostate.
    std::variant<std::monostate, primitive_t> m_prim;
  };

} // namespace yave