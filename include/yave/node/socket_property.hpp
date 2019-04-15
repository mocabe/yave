//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/config/config.hpp>

#include <string>

namespace yave {

  /// Socket property class for node graph.
  class SocketProperty
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
    SocketProperty(const std::string& name, input_t);
    /// Output ctor.
    SocketProperty(const std::string& name, output_t);

    /// Check input.
    bool is_input() const;

    /// Check output.
    bool is_output() const;

    /// Get name of socket.
    [[nodiscard]] const std::string& name() const;

  private:
    /// input
    static constexpr const bool input_v = true;
    /// output
    static constexpr const bool output_v = !input_v;
    /// name of socket
    const std::string m_name;
    /// input or output
    const bool m_io;
  };

} // namespace yave