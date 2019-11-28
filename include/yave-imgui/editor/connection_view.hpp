//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/connection_handle.hpp>
#include <yave/lib/imgui/imgui_context.hpp>

namespace yave::editor::imgui {

  // fwd
  class editor_context;
  struct socket_view;

  /// Connection view interface
  struct connection_view
  {
    /// draw connection
    ///\param base cursor pos
    virtual void draw(const ImVec2& base) const = 0;
    /// dtor
    virtual ~connection_view() noexcept = 0;
  };

  /// Connection view
  struct basic_connection_view : connection_view
  {
  public:
    basic_connection_view(
      const connection_handle& h,
      const editor_context& ctx);

    /// handle
    connection_handle handle;

    /// selected?
    bool is_selected;

    /// hovered?
    bool is_hovered;

    // src socket
    std::unique_ptr<socket_view> src;

    // dst socket
    std::unique_ptr<socket_view> dst;
  };
}