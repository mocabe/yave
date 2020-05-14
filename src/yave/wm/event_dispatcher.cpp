//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/wm/event_dispatcher.hpp>

#include <range/v3/view.hpp>

namespace yave::wm {

  using namespace ranges;

  namespace {

    enum class dispatch_order
    {
      pre,
      post,
    };

    /// dfs dispatcher implementation
    /// \param Order dispatching order
    /// \param Reversed reverse children order?
    template <dispatch_order Order, bool Reversed>
    void dfs_dispatch(
      window* w,
      event& e,
      editor::data_context& data_ctx,
      editor::view_context& view_ctx)
    {
      if constexpr (Order == dispatch_order::pre) {
        w->emit(e, data_ctx, view_ctx);
        if (e.accepted())
          return;
      }

      auto children = [&] {
        if constexpr (Reversed)
          return w->children() | views::reverse;
        else
          return w->children() | views::all;
      }();

      for (auto&& c : children) {
        dfs_dispatch<Order, Reversed>(c.get(), e, data_ctx, view_ctx);
        if (e.accepted())
          return;
      }

      if constexpr (Order == dispatch_order::post) {
        w->emit(e, data_ctx, view_ctx);
        if (e.accepted())
          return;
      }
    }
  } // namespace

  event_dispatcher::event_dispatcher(
    std::unique_ptr<event>&& e,
    editor::data_context& dctx,
    editor::view_context& vctx)
    : m_event {std::move(e)}
    , m_data_ctx {dctx}
    , m_view_ctx {vctx}
  {
  }

  void dfs_dispatcher_pre::dispatch(window* root)
  {
    dfs_dispatch<dispatch_order::pre, false>(
      root, *m_event, m_data_ctx, m_view_ctx);
  }

  void dfs_dispatcher_post::dispatch(window* root)
  {
    dfs_dispatch<dispatch_order::post, false>(
      root, *m_event, m_data_ctx, m_view_ctx);
  }

  void dfs_dispatcher_reverse_pre::dispatch(window* root)
  {
    dfs_dispatch<dispatch_order::post, true>(
      root, *m_event, m_data_ctx, m_view_ctx);
  }

  void dfs_dispatcher_reverse_post::dispatch(window* root)
  {
    dfs_dispatch<dispatch_order::pre, true>(
      root, *m_event, m_data_ctx, m_view_ctx);
  }

} // namespace yave::wm