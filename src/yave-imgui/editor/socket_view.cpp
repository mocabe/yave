//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave-imgui/editor/socket_view.hpp>
#include <yave-imgui/editor/editor_context.hpp>
#include <yave-imgui/editor/style.hpp>

namespace yave::editor::imgui {

  // for ImVec2 operators
  using namespace yave::imgui;

  basic_socket_view::basic_socket_view(
    const socket_handle& h,
    const editor_context& ctx)
  {
    auto info      = ctx.node_graph().get_info(handle);
    auto edit_info = ctx.get_editor_info(handle);

    handle      = h;
    is_selected = edit_info->is_selected;
    is_hovered  = edit_info->is_hovered;
    type        = info->type();
  }

  auto basic_socket_view::min_width() const -> float
  {
  }

  auto basic_socket_view::height() const -> float
  {
  }

  auto basic_socket_view::slot_pos(const ImVec2&) const -> ImVec2
  {
  }

  void basic_socket_view::draw(const ImVec2& pos) const
  {
  }

  void basic_socket_view::handle_input(const ImVec2&, input_state&) const
  {
  }

} // namespace yave::editor::imgui