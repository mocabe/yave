//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/wm/window_visitor.hpp>

#include <range/v3/view.hpp>

namespace yave::wm {

  using namespace ranges;

  namespace {

    enum class traverse_order
    {
      pre,
      post,
    };

    /// dfs dispatcher implementation
    /// \param Order visitation order
    /// \param Reversed reverse children order?
    template <traverse_order Order, bool Reversed>
    bool dfs_traverse(window* w, window_visitor& visitor)
    {
      if constexpr (Order == traverse_order::pre) {
        if (!visitor.visit(w))
          return false;
      }

      auto children = [&] {
        if constexpr (Reversed)
          return w->children() | views::reverse;
        else
          return w->children() | views::all;
      }();

      for (auto&& c : children) {
        if (!dfs_traverse<Order, Reversed>(c.get(), visitor))
          return false;
      }

      if constexpr (Order == traverse_order::post) {
        if (!visitor.visit(w))
          return false;
      }
      return true;
    }
  } // namespace

  void dfs_traverser_pre::traverse(window* root, window_visitor& v)
  {
    dfs_traverse<traverse_order::pre, false>(root, v);
  }

  void dfs_traverser_post::traverse(window* root, window_visitor& v)
  {
    dfs_traverse<traverse_order::post, false>(root, v);
  }

  void dfs_traverser_reverse_pre::traverse(window* root, window_visitor& v)
  {
    dfs_traverse<traverse_order::post, true>(root, v);
  }

  void dfs_traverser_reverse_post::traverse(window* root, window_visitor& v)
  {
    dfs_traverse<traverse_order::pre, true>(root, v);
  }
}