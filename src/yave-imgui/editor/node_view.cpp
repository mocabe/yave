//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave-imgui/editor/node_view.hpp>
#include <yave-imgui/editor/socket_view.hpp>
#include <yave-imgui/editor/connection_view.hpp>
#include <yave-imgui/editor/editor_context.hpp>
#include <yave-imgui/editor/style.hpp>
#include <yave/support/log.hpp>

YAVE_DECL_G_LOGGER(node_view)

namespace yave::editor::imgui {

  // for ImVec2 operators
  using namespace yave::imgui;

  namespace {

    auto calc_node_header_min_size(const std::string& title) -> ImVec2
    {
      // Width: text width + 32 px
      // Height: 32 px
      // Font: 15 px
      auto text_size = calc_text_size(title, font_size_level::e15);
      return {text_size.x + 2 * gridpx(4), gridpx(4)};
    }

    void draw_node_background(
      const ImVec2& pos,
      const ImVec2& size,
      bool is_hovered,
      bool is_selected)
    {
      auto fill = is_hovered
                    ? get_node_background_fill_color_hovered()
                    : is_selected ? get_node_background_fill_color_selected()
                                  : get_node_background_fill_color_hovered();

      auto rounding = get_node_rounding();
      auto draw_list = ImGui::GetWindowDrawList();

      // fill
      draw_list->AddRectFilled(pos, pos + size, fill, rounding);
    }

    void draw_node_header(
      const std::string& title,
      const ImVec2& pos,
      const ImVec2& size,
      bool is_hovered,
      bool is_selected)
    {
      auto fill = is_hovered
                    ? get_node_background_fill_color_hovered()
                    : is_selected ? get_node_header_fill_color_selected()
                                  : get_node_header_fill_color();

      auto rounding  = get_node_rounding();
      auto draw_list = ImGui::GetWindowDrawList();

      // fill
      draw_list->AddRectFilled(
        pos, {pos.x + size.x, pos.y + size.y / 2}, fill, rounding);
      draw_list->AddRectFilled(
        {pos.x, pos.y + size.y / 2 - 10}, pos + size, fill, 0);

      // text
      auto text_pos = calc_text_pos(
        title, font_size_level::e15, size, text_alignment::center);
      ImGui::SetCursorScreenPos(pos + text_pos);
      ImGui::TextColored(get_node_header_text_color(), "%s", title.c_str());
    }

  } // namespace

  node_view::node_view(const node_handle& h, const editor_context& ctx)
    : handle {h}
    , info {*ctx.node_graph().get_info(h)}
  {
    init_logger();
  }

  basic_node_view::basic_node_view(
    const node_handle& h,
    const editor_context& ctx,
    const std::vector<std::shared_ptr<socket_view>>& sv)
    : node_view(h, ctx)
    , sockets {sv}
  {
    auto edit_info = ctx.get_editor_info(handle);

    is_selected = edit_info->is_selected;
    is_hovered  = edit_info->is_hovered;
    position    = edit_info->position;

    title = info.name();
  }

  auto basic_node_view::min_size() const -> ImVec2
  {
    ImVec2 size = calc_node_header_min_size(title);

    for (auto&& s : sockets) {
      auto sz = s->min_size();
      size    = {std::max(size.x, sz.x), size.y + sz.y};
    }

    return size;
  }

  void basic_node_view::draw(const ImVec2& base, const ImVec2& size) const
  {
    auto channel = get_node_channel_index();
    ImGui::GetWindowDrawList()->ChannelsSetCurrent(channel);

    // draw bg
    draw_node_background(base, size, is_hovered, is_selected);

    // draw header
    auto header_size = calc_node_header_min_size(title);
    draw_node_header(
      title,
      base,
      ImVec2 {size.x, header_size.y}, // use min height
      is_hovered,
      is_selected);

    auto cursor_y = header_size.y;
    for (auto&& s : sockets) {

      auto min_size = s->min_size();

      auto socket_base = ImVec2 {base.x, base.y + cursor_y};
      auto socket_size = ImVec2 {size.x, min_size.y};

      // draw socket
      s->draw(socket_base, socket_size); // use min height

      // move cursor
      cursor_y += socket_size.y;
    }
  }

  auto basic_node_view::slot_pos(
    const socket_handle& socket,
    const ImVec2& pos,
    const ImVec2& size) const -> ImVec2
  {
    for (auto&& s : sockets) {
      if (s->handle == socket)
        return s->slot_pos(pos, size);
    }
    throw std::runtime_error("Invalid socket handle");
  }

  void basic_node_view::handle_input(
    const ImVec2& base,
    const ImVec2& size,
    input_state& state,
    editor_context& ctx) const
  {
    ImGui::PushID(handle.id().data);

    float cursor_y = calc_node_header_min_size(title).y;

    // handle socket input
    for (auto&& s : sockets) {

      auto min_size = s->min_size();

      auto socket_base = ImVec2 {base.x, base.y + cursor_y};
      auto socket_size = ImVec2 {size.x, min_size.y};

      s->handle_input(socket_base, socket_size, state, ctx);

      cursor_y += socket_size.y;
    }

    // background
    ImGui::SetCursorScreenPos(base);

    bool hovered = false;
    bool pressed = InvisibleButtonEx("bg", size, &hovered);

    if (hovered) {
      // TODO: set hovered
    }
    if (pressed) {
      // TODO: set selected
    }
    if (ImGui::IsItemActive()) {
      // TODO: drag
    }

    ImGui::PopID();
  }

} // namespace yave::editor::imgui