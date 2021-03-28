//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/ui/command.hpp>
#include <yave/ui/passkey.hpp>

namespace yave::ui {

  class view_context;
  class data_context;

  /// Data context command
  using data_command = command<data_context>;


  /// Application data context
  class data_context
  {
    class impl;
    std::unique_ptr<impl> m_pimpl;

  public:
    // ctor
    data_context();
    // dtor
    ~data_context() noexcept;

  public:
    // for view_context
    void set_view_ctx(view_context& vctx, passkey<view_context>);
    /// view
    auto view_ctx() -> view_context&;
    auto view_ctx() const -> const view_context&;

  private:
    void _post(data_command op) const;
    void _post_delay(data_command op) const;
    void _post_empty() const;

  public:
    template <class F>
    void post(F&& f)
    {
      _post(data_command(std::forward<F>(f)));
    }

    template <class F>
    void post_delay(F&& f)
    {
      _post_delay(data_command(std::forward<F>(f)));
    }

    void post_empty()
    {
      _post_empty();
    }
  };

} // namespace yave::ui