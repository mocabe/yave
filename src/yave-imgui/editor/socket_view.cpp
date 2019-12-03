//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave-imgui/editor/socket_view.hpp>
#include <yave-imgui/editor/editor_context.hpp>
#include <yave-imgui/editor/style.hpp>
#include <yave/support/log.hpp>

YAVE_DECL_G_LOGGER(socket_view)

namespace yave::editor::imgui {

  // for ImVec2 operators
  using namespace yave::imgui;

  namespace {

    ImVec2 socket_padding()
    {
      return {gridpx(3) / 2, gridpx(1)};
    }

    ImVec2 socket_min_area_size(const std::string& str)
    {
      auto text_size = calc_text_size(str, font_size_level::e15);
      auto padding   = socket_padding();
      // fix 16 px height
      return {text_size.x + 2 * padding.x, gridpx(2) + 2 * padding.y};
    }

    ImVec2 socket_slot_pos(
      socket_type type,
      const ImVec2& base,
      const ImVec2& size)
    {
      switch (type) {
        case socket_type::input:
          return {base.x + size.x, base.y + size.y / 2};
        case socket_type::output:
          return {base.x, base.y + size.y / 2};
      }
      unreachable();
    }

    // basic
    void draw_socket_content(
      const std::string& name,
      socket_type type,
      const ImVec2& pos,
      const ImVec2& size,
      bool is_hovered,
      bool is_selected)
    {
      auto padding_x = gridpx(3) / 2;
      auto padding_y = gridpx(1);

      auto area_base = ImVec2 {pos.x + padding_x, pos.y + padding_y};
      auto area_size = ImVec2 {size.x - 2 * padding_x, size.y - 2 * padding_y};

      auto align = (type == socket_type::input)
                     ? text_alignment::right
                     : (type == socket_type::output) ? text_alignment::left
                                                     : text_alignment::center;

      auto text_pos =
        calc_text_pos(name, font_size_level::e15, area_size, align);

      auto col = get_socket_text_color();

      // socket name
      ImGui::SetCursorScreenPos(area_base + text_pos);
      ImGui::TextColored(col, "%s", name.c_str());
    }

    // basic
    void draw_socket_slot(
      socket_type type,
      const ImVec2& pos,
      const ImVec2& size,
      bool is_hovered,
      bool is_selecetd)
    {
      auto slot_pos = socket_slot_pos(type, pos, size);

      auto draw_list = ImGui::GetWindowDrawList();

      auto col = is_hovered ? get_socket_slot_color_hovered(type)
                            : is_selecetd ? get_socket_slot_color_selected(type)
                                          : get_socket_slot_color(type);

      // slot
      draw_list->AddCircleFilled(slot_pos, gridpx(1), col);
    }
  }

  socket_view::socket_view(const socket_handle& h, const editor_context& ctx)
    : handle {h}
    , info {*ctx.node_graph().get_info(h)}
  {
    init_logger();
  }

  basic_socket_view::basic_socket_view(
    const socket_handle& h,
    const editor_context& ctx)
    : socket_view(h, ctx)
  {
    auto edit_info = ctx.get_editor_info(handle);

    is_selected = edit_info->is_selected;
    is_hovered  = edit_info->is_hovered;

    type = info.type();
    name = info.name();
  }

  auto basic_socket_view::min_size() const -> ImVec2
  {
    return socket_min_area_size(name);
  }

  auto basic_socket_view::slot_pos(const ImVec2& base, const ImVec2& size) const
    -> ImVec2
  {
    return socket_slot_pos(type, base, size);
  }

  void basic_socket_view::draw(const ImVec2& base, const ImVec2& size) const
  {
    auto channel = get_socket_channel_index();
    ImGui::GetWindowDrawList()->ChannelsSetCurrent(channel);

    draw_socket_content(name, type, base, size, is_hovered, is_selected);

    draw_socket_slot(type, base, size, is_hovered, is_selected);
  }

  void basic_socket_view::handle_input(
    const ImVec2& base,
    const ImVec2& size,
    input_state& state,
    editor_context& ctx) const
  {
    ImGui::PushID(handle.id().data);

    auto slot_pos = socket_slot_pos(type, base, size);

    ImGui::SetCursorScreenPos(slot_pos);
    bool hovered = false;
    bool pressed =
      InvisibleButtonEx(name.c_str(), {gridpx(1), gridpx(1)}, &hovered);

    if (pressed) {
      // TODO: pressed
    }

    if (hovered) {
      // TODO: hovered
    }

    ImGui::PopID();
  }

} // namespace yave::editor::imgui