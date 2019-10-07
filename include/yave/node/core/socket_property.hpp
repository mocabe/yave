//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/config/config.hpp>

#include <string>

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
    socket_property(const std::string& name, socket_type type);

    /// Check input.
    [[nodiscard]] bool is_input() const;

    /// Check output.
    [[nodiscard]] bool is_output() const;

    /// Get socket type
    [[nodiscard]] auto get_type() const -> socket_type;

    /// Get name of socket.
    [[nodiscard]] auto name() const -> const std::string&;

  private:
    /// name of socket
    const std::string m_name;
    /// input or output
    const socket_type m_type;
  };

} // namespace yave