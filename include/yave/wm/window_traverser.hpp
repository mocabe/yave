//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/wm/window_visitor.hpp>

namespace yave::wm {

  /// window traverser
  struct window_traverser
  {
    /// dtor
    virtual ~window_traverser() noexcept = default;

    /// Traverse windows
    /// \param root pointer to root window of window tree
    virtual void traverse(window* root, window_visitor& visitor) = 0;
  };

  /* Some useful window visitors */

  /// single window traverser
  class single_window_traverser final : window_traverser
  {
    uid m_target;

  public:
    single_window_traverser(uid target)
      : window_traverser()
      , m_target {target}
    {
    }

    void traverse(window* root, window_visitor& visitor) override;
  };

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

}