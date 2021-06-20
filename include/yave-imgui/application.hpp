//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/core/config.hpp>

#include <memory>

namespace yave::editor::imgui {

  /// yave-imgui editor application
  class application
  {
    class impl;
    std::unique_ptr<impl> m_pimpl;

  public:
    /// Ctor
    application();
    /// Dtor
    ~application() noexcept;

  public:
    /// run app
    void run();
  };

} // namespace yave::editor::imgui