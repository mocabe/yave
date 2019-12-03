//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/connection_handle.hpp>
#include <yave/node/core/managed_connection_info.hpp>
#include <yave/node/core/socket_handle.hpp>
#include <yave/lib/imgui/imgui_context.hpp>
#include <yave-imgui/editor/input_state.hpp>

namespace yave::editor::imgui {

  // fwd
  class editor_context;
  struct node_view;
  struct socket_view;

  /// Connection view interface
  struct connection_view
  {
    /// ctor
    connection_view(const connection_handle& h, const editor_context& ctx);

    /// handle
    connection_handle handle;

    /// info
    managed_connection_info info;

    /// draw connection
    ///\param src_pos src slot position
    ///\param dst_pos dst slot position
    virtual void draw(const ImVec2& src_pos, const ImVec2& dst_pos) const = 0;
    /// handle input
    ///\param src_pos src slot position
    ///\param dst_pos dst slot position
    virtual void handle_input(
      const ImVec2& src_pos,
      const ImVec2& dst_pos,
      input_state& state,
      editor_context& ctx) const = 0;
    /// dtor
    virtual ~connection_view() noexcept = 0;
  };

  /// Connection view
  struct basic_connection_view : connection_view
  {
  public:
    basic_connection_view(
      const connection_handle& h,
      const editor_context& ctx,
      const std::shared_ptr<node_view>& srcn,
      const std::shared_ptr<node_view>& dstn,
      const std::shared_ptr<socket_view>& srcs,
      const std::shared_ptr<socket_view>& dsts);

    /// selected?
    bool is_selected;

    /// hovered?
    bool is_hovered;

    /// src node view
    std::shared_ptr<node_view> src_node;
    /// dst node view
    std::shared_ptr<node_view> dst_node;

    /// src socket view
    std::shared_ptr<socket_view> src_socket;
    /// dst socket view
    std::shared_ptr<socket_view> dst_socket;

  private:
    void draw(const ImVec2& src_pos, const ImVec2& dst_pos) const override;
    void handle_input(
      const ImVec2& src_pos,
      const ImVec2& dst_pos,
      input_state& state,
      editor_context& ctx) const override;
  };
}