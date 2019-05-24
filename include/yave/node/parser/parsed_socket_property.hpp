//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core.hpp>

namespace yave {

  /// Socket property of parsed_node_graph.
  class parsed_socket_property
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
    parsed_socket_property(const std::string& name, input_t);
    /// Output ctor.
    parsed_socket_property(const std::string& name, output_t);

    /// Check input.
    [[nodiscard]] bool is_input() const;

    /// Check output.
    [[nodiscard]] bool is_output() const;

    /// Get name of socket.
    [[nodiscard]] const std::string& name() const;

  private:
    /// name of socket
    const std::string m_name;
    /// input or output
    const bool m_is_input;
  };

} // namespace yave