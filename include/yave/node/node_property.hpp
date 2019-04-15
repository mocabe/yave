//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/config/config.hpp>
#include <yave/node/primitive_definition.hpp>

#include <string>
#include <variant>

namespace yave {

  /// Node property class for node graph.
  class NodeProperty
  {
  public:
    /// Primitive ctor.
    NodeProperty(const std::string& name, const primitive_t& prim);
    /// Non-primitive ctor.
    NodeProperty(const std::string& name, std::monostate);

    /// Check primitive node.
    bool is_prim() const;

    /// Get node name.
    std::string name() const;

    /// Get primitive value.
    /// \requires is_prim() == true
    primitive_t prim() const;

    /// Set primitive value.
    void set_prim(const primitive_t& prim);

    /// Unset primitive value.
    void ser_prim(std::monostate);

  private:
    /// name of name
    const std::string m_name;
    /// primitive value
    std::variant<std::monostate, primitive_t> m_prim;
  };

} // namespace yave