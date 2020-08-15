//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/wm/window_traverser.hpp>

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
        if (visitor.visit(w))
          return true;
      }

      auto children =
        [&] {
          if constexpr (Reversed)
            return w->children() | views::reverse;
          else
            return w->children() | views::all;
        }()
        // convert to window*
        | views::transform([&](auto&& c) { return w->as_mut_child(c); });

      for (auto&& c : children) {
        if (dfs_traverse<Order, Reversed>(c, visitor))
          return true;
      }

      if constexpr (Order == traverse_order::post) {
        if (visitor.visit(w))
          return true;
      }
      return false;
    }
  } // namespace

  void single_window_traverser::traverse(window* root, window_visitor& v)
  {
    auto search =
      [id = m_target](auto&& self, window* w, window_visitor& v) -> bool {
      if (w->id() == id) {
        v.visit(w);
        return true;
      }

      for (auto&& c : w->children())
        if (self(w->as_mut_child(c), v))
          return true;

      return false;
    };

    (void)fix_lambda(search)(root, v);
  }

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