//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/wm/window.hpp>

namespace yave::wm {

  /// window visitor
  struct window_visitor
  {
  public:
    /// dtor
    virtual ~window_visitor() noexcept = default;

    /// Visit window
    /// \param win window to visit
    /// \returns continuation flag (true for continue, false to stop)
    virtual bool visit(window* win) = 0;
  };

  /// window traverser
  struct window_traverser
  {
    /// dtor
    virtual ~window_traverser() noexcept = default;

    /// Traverse windows
    /// \param root pointer to root window of window tree
    virtual void traverse(window* root, window_visitor& visitor) = 0;
  };

  // Some useful window visitors

  /// dfs with preordering
  struct dfs_traverser_pre final : window_traverser
  {
    void traverse(window* root, window_visitor& visitor) override;
  };

  /// dfs with postordering
  struct dfs_traverser_post final : window_traverser
  {
    void traverse(window* root, window_visitor& visitor) override;
  };

  /// dfs with reverse preordering
  struct dfs_traverser_reverse_pre final : window_traverser
  {
    void traverse(window* root, window_visitor& visitor) override;
  };

  /// dfs with reverse postordering
  struct dfs_traverser_reverse_post final : window_traverser
  {
    void traverse(window* root, window_visitor& visitor) override;
  };

} // namespace yave::wm