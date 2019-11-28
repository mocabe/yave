//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave-imgui/editor/node_view.hpp>
#include <yave-imgui/editor/socket_view.hpp>
#include <yave-imgui/editor/editor_context.hpp>
#include <yave-imgui/editor/style.hpp>

namespace yave::editor::imgui {

  // for ImVec2 operators
  using namespace yave::imgui;

  namespace {

    float calc_header_height(const std::string& title)
    {
      auto text_size = calc_text_size(title, font_size_level::body);
      return text_size.y + 2 * get_text_padding();
    }

    float calc_header_min_width(const std::string& title)
    {
      auto text_size = calc_text_size(title, font_size_level::body);
      return text_size.x + 2 * get_text_padding();
    }

    void draw_bg(
      const ImVec2& pos,
      const ImVec2& size,
      bool is_hovered,
      bool is_selected)
    {
      auto fill = get_node_fill_color();
      if (is_hovered)
        fill = get_node_fill_color_hovered();
      if (is_selected)
        fill = get_node_fill_color_selected();

      auto rounding = get_node_rounding();

      auto draw_list = ImGui::GetWindowDrawList();
      draw_list->AddRectFilled(pos, pos + size, fill, rounding);
    }

    void draw_header(
      const std::string& title,
      const ImVec2& pos,
      const ImVec2& size)
    {
      auto text_pos = calc_text_pos(
        title, font_size_level::caption, size, text_alignment::center);
      ImGui::SetCursorScreenPos(pos + text_pos);
      ImGui::Text("%s", title.c_str());
    }

  } // namespace

  basic_node_view::basic_node_view(
    const node_handle& h,
    const editor_context& ctx)
  {
    auto info      = ctx.node_graph().get_info(handle);
    auto edit_info = ctx.get_editor_info(handle);

    handle      = h;
    is_selected = edit_info->is_selected;
    is_hovered  = edit_info->is_hovered;
    position    = edit_info->position;
    title       = info->name();

    // initialize sockets

    for (auto&& s : ctx.node_graph().output_sockets(h)) {
      sockets.push_back(create_socket_view(s, ctx));
    }

    for (auto&& s : ctx.node_graph().input_sockets(h)) {
      sockets.push_back(create_socket_view(s, ctx));
    }

    // layout

    for (auto&& s : sockets) {
    }
  }

  auto basic_node_view::min_size() const -> ImVec2
  {
    float cursor_height = 0;
    float max_width     = 0;

    // header
    cursor_height = calc_header_height(title);
    max_width     = calc_header_min_width(title);

    for (auto&& s : sockets) {
      max_width = std::max(max_width, s->min_width());
      cursor_height += s->height();
    }

    return {max_width, cursor_height};
  }

  void basic_node_view::draw(const ImVec2& base) const
  {
    auto channel = get_node_channel_index();
    ImGui::GetWindowDrawList()->ChannelsSetCurrent(channel);

    float cursor_height = 0;
    auto size           = min_size();

    // bg
    draw_bg(base, size, is_hovered, is_selected);

    // header
    auto header_height = calc_header_height(title);
    draw_header(title, {0, cursor_height}, {size.x, header_height});
    cursor_height += header_height;

    for (auto&& s : sockets) {
      s->draw({0, cursor_height});
      cursor_height += s->height();
    }
  }

  void basic_node_view::handle_input(const ImVec2& base, input_state& state)
    const
  {
    float cursor_height = calc_header_height(title);

    for (auto&& s : sockets) {
      s->handle_input({base.x, base.y + cursor_height}, state);
      cursor_height += s->height();
    }
  }

  auto create_node_view(const node_handle& node, const editor_context& ctx)
    -> std::unique_ptr<node_view>
  {
    if (!ctx.node_graph().exists(node))
      throw std::runtime_error("Invalid node handle!");

    return std::make_unique<basic_node_view>(node, ctx);
  }

} // namespace yave::editor::imgui