//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/socket_property.hpp>

namespace yave {

  class socket_info
  {
  public:
    socket_info()                   = delete;
    socket_info(const socket_info&) = default;
    socket_info(socket_info&&)      = default;
    socket_info& operator=(const socket_info&) = default;
    socket_info& operator=(socket_info&&) = default;

    socket_info(std::string name, socket_type type);

    /// name
    [[nodiscard]] auto name() const -> const std::string&;

    /// socket type
    [[nodiscard]] auto type() const -> socket_type;

  private:
    std::string m_name;
    socket_type m_type;
  };
}