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
  enum class basic_node_type
  {
    normal,    // normal node
    interface, // interface node
  };

  /// Node property class for node graph.
  class basic_node_property
  {
  public:
    /// Non-primitive ctor.
    basic_node_property(const std::string& name, basic_node_type type);
    /// Copy ctor
    basic_node_property(const basic_node_property&);

    /// Normal node?
    [[nodiscard]] bool is_normal() const;
    /// Interface node?
    [[nodiscard]] bool is_interface() const;
    /// Get node name.
    [[nodiscard]] auto name() const -> const std::string&;
    /// Get node type
    [[nodiscard]] auto type() const -> basic_node_type;
    /// Has data?
    [[nodiscard]] bool has_data() const;
    /// Get data
    [[nodiscard]] auto get_data() const -> std::optional<object_ptr<Object>>;

    /// Set data
    void set_data(object_ptr<Object> data);

    /// Set name
    void set_name(const std::string& new_name);

  private:
    /// Name of node.
    std::string m_name;
    /// Node type
    const basic_node_type m_type;
    /// Custom data
    std::optional<object_ptr<Object>> m_data;

  private: /* for node_graph */
    friend class basic_node_graph;
    auto get_flags() const -> uint8_t;
    void set_flags(uint8_t bits) const;
    mutable uint8_t m_flags;
  };

} // namespace yave