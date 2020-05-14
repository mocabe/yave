//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/wm/event.hpp>
#include <yave/wm/window.hpp>

namespace yave::wm {

  /// Event dispatcher
  class event_dispatcher
  {
  public:
    /// Ctor
    event_dispatcher(
      std::unique_ptr<event>&& e,
      editor::data_context& dctx,
      editor::view_context& vctx);
    /// Dtor
    virtual ~event_dispatcher() noexcept = default;
    /// Dispatch event to window tree
    /// \param root pointer to root window of window tree
    virtual void dispatch(window* root) = 0;

  protected:
    std::unique_ptr<event> m_event;
    editor::data_context& m_data_ctx;
    editor::view_context& m_view_ctx;
  };

  /// depth first pre
  struct dfs_dispatcher_pre : event_dispatcher
  {
    void dispatch(window* root) override;
  };

  /// depth first post
  struct dfs_dispatcher_post : event_dispatcher
  {
    void dispatch(window* root) override;
  };

  /// reverse depth first pre
  struct dfs_dispatcher_reverse_pre : event_dispatcher
  {
    void dispatch(window* root) override;
  };

  /// reverse depth first post
  struct dfs_dispatcher_reverse_post : event_dispatcher
  {
    void dispatch(window* root) override;
  };

} // namespace yave::wm