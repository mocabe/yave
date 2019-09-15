//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/config/config.hpp>

#include <string>

namespace yave {

  /// Socket property class for node graph.
  class socket_property
  {
  public:
    /// input tag
    struct input_t
    {
    };
    /// output tag
    struct output_t
    {
    };
    /// input tag
    static constexpr input_t input = {};
    /// output tag
    static constexpr output_t output = {};

    /// Input ctor.
    socket_property(const std::string& name, input_t);
    /// Output ctor.
    socket_property(const std::string& name, output_t);

    /// Check input.
    [[nodiscard]] bool is_input() const;

    /// Check output.
    [[nodiscard]] bool is_output() const;

    /// Get name of socket.
    [[nodiscard]] auto name() const -> const std::string&;

  private:
    /// name of socket
    const std::string m_name;
    /// input or output
    const bool m_is_input;
  };

} // namespace yave