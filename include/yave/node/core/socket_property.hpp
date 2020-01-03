//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/config/config.hpp>
#include <yave/rts/object_ptr.hpp>

#include <string>
#include <optional>

namespace yave {

  /// Socket type
  enum class socket_type
  {
    input,
    output,
  };

  /// Socket property class for node graph.
  class socket_property
  {
  public:
    /// Ctor
    socket_property(std::string name, socket_type type);

    /// Copy ctor
    socket_property(const socket_property&);

    /// Check input.
    [[nodiscard]] bool is_input() const;

    /// Check output.
    [[nodiscard]] bool is_output() const;

    /// Get socket type
    [[nodiscard]] auto type() const -> socket_type;

    /// Get name of socket.
    [[nodiscard]] auto name() const -> const std::string&;

    /// Has data?
    [[nodiscard]] bool has_data() const;

    /// Get data
    [[nodiscard]] auto get_data() const -> std::optional<object_ptr<Object>>;

    /// Set data. Enables has_data() after this call.
    void set_data(object_ptr<Object> data);

    /// Set new name
    void set_name(std::string new_name);

  private:
    /// name of socket
    std::string m_name;
    /// input or output
    const socket_type m_type;

  private:
    std::optional<object_ptr<Object>> m_data;
  };

} // namespace yave