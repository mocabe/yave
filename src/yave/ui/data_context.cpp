
//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/ui/data_context.hpp>

#include <mutex>
#include <queue>
#include <array>

namespace yave::ui {

  class data_context::impl
  {
    data_context& m_self;
    view_context* m_pview;

  public:
    impl(data_context& self)
      : m_self {self}
    {
    }

  public:
    void set_view_ctx(view_context& vctx)
    {
      m_pview = &vctx;
    }

    auto& view_ctx()
    {
      assert(m_pview);
      return *m_pview;
    }
  };

  data_context::data_context()
    : m_pimpl {std::make_unique<impl>(*this)}
  {
  }

  data_context::~data_context() noexcept = default;

  void data_context::set_view_ctx(view_context& vctx, passkey<view_context>)
  {
    m_pimpl->set_view_ctx(vctx);
  }

} // namespace yave::ui