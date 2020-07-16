//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave-imgui/data_type_socket.hpp>
#include <yave/editor/editor_data.hpp>

#include <imgui_stdlib.h>

namespace yave::editor::imgui {

  data_type_socket<Float>::data_type_socket(
    const object_ptr<DataTypeHolder>& holder,
    const object_ptr<FloatDataProperty>& property,
    const socket_handle& s,
    const structured_node_graph& g,
    const node_window& nw)
    : m_holder {holder}
    , m_property {property}
    , basic_socket_drawer(s, g, nw)
  {
  }

  void data_type_socket<Float>::_draw_content(
    const node_window& nw,
    data_context& dctx,
    view_context& vctx,
    node_window_draw_info& draw_info,
    ImVec2 pos,
    ImVec2 size) const
  {
    assert(info.type() == socket_type::input);
    assert(m_holder);
    assert(m_property);

    auto s    = handle;
    auto n    = info.node();
    auto name = info.name();

    if (!info.connections().empty())
      return;

    auto slider_bg_col   = get_socket_slider_color();
    auto slider_text_col = get_socket_slider_text_color();

    ImGui::PushID(s.id().data);
    ImGui::SetCursorScreenPos(pos);
    ImGui::PushItemWidth(size.x);
    ImGui::PushStyleColor(ImGuiCol_Text, ImU32(slider_text_col));
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImU32(slider_bg_col));
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImU32(slider_bg_col));
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImU32(slider_bg_col));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, get_node_rounding());
    {
      auto f   = value_cast<Float>(m_holder->data());
      auto val = *f;
      ImGui::DragFloat(
        "",
        &val,
        *m_property->step(),
        *m_property->min(),
        *m_property->max(),
        (name + ":%.3f").c_str());

      if (*f != val) {
        *f = val;
        dctx.exec(make_data_command(
          [](auto& ctx) { ctx.data().executor.notify_execute(); }));
      }
    }
    ImGui::PopStyleColor(4);
    ImGui::PopStyleVar(1);
    ImGui::PopItemWidth();
    ImGui::PopID();
  }

  data_type_socket<Int>::data_type_socket(
    const object_ptr<DataTypeHolder>& holder,
    const object_ptr<IntDataProperty>& property,
    const socket_handle& s,
    const structured_node_graph& g,
    const node_window& nw)
    : m_holder {holder}
    , m_property {property}
    , basic_socket_drawer(s, g, nw)
  {
  }

  void data_type_socket<Int>::_draw_content(
    const node_window& nw,
    data_context& dctx,
    view_context& vctx,
    node_window_draw_info& draw_info,
    ImVec2 pos,
    ImVec2 size) const
  {
    assert(info.type() == socket_type::input);
    assert(m_holder);
    assert(m_property);

    auto s    = handle;
    auto n    = info.node();
    auto name = info.name();

    if (!info.connections().empty())
      return;

    auto slider_bg_col   = get_socket_slider_color();
    auto slider_text_col = get_socket_slider_text_color();

    ImGui::PushID(s.id().data);
    ImGui::SetCursorScreenPos(pos);
    ImGui::PushItemWidth(size.x);
    ImGui::PushStyleColor(ImGuiCol_Text, ImU32(slider_text_col));
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImU32(slider_bg_col));
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImU32(slider_bg_col));
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImU32(slider_bg_col));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, get_node_rounding());
    {
      auto i   = value_cast<Int>(m_holder->data());
      auto val = *i;
      ImGui::DragInt(
        "",
        &val,
        *m_property->step(),
        *m_property->min(),
        *m_property->max(),
        (name + ":%.3f").c_str());

      if (*i != val) {
        *i = val;
        dctx.exec(make_data_command(
          [](auto& ctx) { ctx.data().executor.notify_execute(); }));
      }
    }
    ImGui::PopStyleColor(4);
    ImGui::PopStyleVar(1);
    ImGui::PopItemWidth();
    ImGui::PopID();
  }

  data_type_socket<Bool>::data_type_socket(
    const object_ptr<DataTypeHolder>& holder,
    const object_ptr<BoolDataProperty>& property,
    const socket_handle& s,
    const structured_node_graph& g,
    const node_window& nw)
    : m_holder {holder}
    , m_property {property}
    , basic_socket_drawer(s, g, nw)
  {
  }

  void data_type_socket<Bool>::_draw_content(
    const node_window& nw,
    data_context& dctx,
    view_context& vctx,
    node_window_draw_info& draw_info,
    ImVec2 pos,
    ImVec2 size) const
  {
    assert(info.type() == socket_type::input);
    assert(m_holder);
    assert(m_property);

    auto s    = handle;
    auto n    = info.node();
    auto name = info.name();

    if (!info.connections().empty())
      return;

    auto slider_bg_col   = get_socket_slider_color();
    auto slider_text_col = get_socket_slider_text_color();

    ImGui::PushID(s.id().data);
    ImGui::SetCursorScreenPos(pos);
    ImGui::PushItemWidth(size.x);
    ImGui::PushStyleColor(ImGuiCol_Text, ImU32(slider_text_col));
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImU32(slider_bg_col));
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImU32(slider_bg_col));
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImU32(slider_bg_col));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, get_node_rounding());
    {
      auto b   = value_cast<Bool>(m_holder->data());
      auto val = *b;
      ImGui::Checkbox("", &val);

      if (*b != val) {
        *b = val;
        dctx.exec(make_data_command(
          [](auto& ctx) { ctx.data().executor.notify_execute(); }));
      }
    }
    ImGui::PopStyleColor(4);
    ImGui::PopStyleVar(1);
    ImGui::PopItemWidth();
    ImGui::PopID();
  }

  data_type_socket<String>::data_type_socket(
    const object_ptr<DataTypeHolder>& holder,
    const object_ptr<StringDataProperty>& property,
    const socket_handle& s,
    const structured_node_graph& g,
    const node_window& nw)
    : m_holder {holder}
    , m_property {property}
    , basic_socket_drawer(s, g, nw)
  {
  }

  void data_type_socket<String>::_draw_content(
    const node_window& nw,
    data_context& dctx,
    view_context& vctx,
    node_window_draw_info& draw_info,
    ImVec2 pos,
    ImVec2 size) const
  {
    assert(info.type() == socket_type::input);
    assert(m_holder);
    assert(m_property);

    auto s    = handle;
    auto n    = info.node();
    auto name = info.name();

    if (!info.connections().empty())
      return;

    auto slider_bg_col   = get_socket_slider_color();
    auto slider_text_col = get_socket_slider_text_color();

    ImGui::PushID(s.id().data);
    ImGui::SetCursorScreenPos(pos);
    ImGui::PushItemWidth(size.x);
    ImGui::PushStyleColor(ImGuiCol_Text, ImU32(slider_text_col));
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImU32(slider_bg_col));
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImU32(slider_bg_col));
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImU32(slider_bg_col));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, get_node_rounding());
    {
      auto s   = value_cast<String>(m_holder->data());
      auto val = std::string(*s);
      ImGui::InputText("", &val);

      if (std::string(*s) != val) {
        *s = yave::string(val);
        dctx.exec(make_data_command(
          [](auto& ctx) { ctx.data().executor.notify_execute(); }));
      }
    }
    ImGui::PopStyleColor(4);
    ImGui::PopStyleVar(1);
    ImGui::PopItemWidth();
    ImGui::PopID();
  }

} // namespace yave::editor::imgui
