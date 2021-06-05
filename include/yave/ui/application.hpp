//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/ui/view_context.hpp>
#include <yave/ui/data_context.hpp>

namespace yave::ui {

  /// application context
  class application
  {
    data_context m_dctx;
    view_context m_vctx;

  public:
    /// init application
    application()
      : m_dctx {}
      , m_vctx {m_dctx}
    {
    }

    /// run threads
    void run()
    {
      m_vctx.post([&](auto& ctx) { init(ctx); });
      m_vctx.run();
    }

  public:
    /// Initialize application
    virtual void init(view_context&) = 0;
  };

} // namespace yave::ui