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
    const object_ptr<const FloatDataProperty>& property,
    const socket_handle& s,
    const structured_node_graph& g,
    const node_window& nw)
    : basic_socket_drawer(s, g, nw)
    , m_holder {holder}
    , m_property {property}
  {
  }

  auto data_type_socket<Float>::min_size(node_window_draw_info&) const -> ImVec2
  {
    auto str       = info.name() + "123.000";
    auto text_size = calc_text_size(str, font_size_level::e15);
    auto height    = 16.f;
    auto padding   = get_socket_padding();
    return {text_size.x + 2 * padding.x, height + 2 * padding.y};
  }

  void data_type_socket<Float>::_draw_content(
    const node_window& nw,
    const data_context& dctx,
    const view_context& vctx,
    node_window_draw_info& draw_info,
    ImVec2 pos,
    ImVec2 size) const
  {
    (void)vctx, (void)nw, (void)draw_info;

    assert(info.type() == socket_type::input);
    assert(m_holder);
    assert(m_property);

    auto s    = handle;
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
        m_holder->set_data(make_object<Float>(val));
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
    const object_ptr<const IntDataProperty>& property,
    const socket_handle& s,
    const structured_node_graph& g,
    const node_window& nw)
    : basic_socket_drawer(s, g, nw)
    , m_holder {holder}
    , m_property {property}
  {
  }

  auto data_type_socket<Int>::min_size(node_window_draw_info&) const -> ImVec2
  {
    auto str       = info.name() + "1234";
    auto text_size = calc_text_size(str, font_size_level::e15);
    auto height    = 16.f;
    auto padding   = get_socket_padding();
    return {text_size.x + 2 * padding.x, height + 2 * padding.y};
  }

  void data_type_socket<Int>::_draw_content(
    const node_window& nw,
    const data_context& dctx,
    const view_context& vctx,
    node_window_draw_info& draw_info,
    ImVec2 pos,
    ImVec2 size) const
  {
    (void)nw, (void)vctx, (void)draw_info;
    assert(info.type() == socket_type::input);
    assert(m_holder);
    assert(m_property);

    auto s    = handle;
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
        m_holder->set_data(make_object<Int>(val));
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
    const object_ptr<const BoolDataProperty>& property,
    const socket_handle& s,
    const structured_node_graph& g,
    const node_window& nw)
    : basic_socket_drawer(s, g, nw)
    , m_holder {holder}
    , m_property {property}
  {
  }

  void data_type_socket<Bool>::_draw_content(
    const node_window& nw,
    const data_context& dctx,
    const view_context& vctx,
    node_window_draw_info& draw_info,
    ImVec2 pos,
    ImVec2 size) const
  {
    (void)nw, (void)vctx, (void)draw_info;
    assert(info.type() == socket_type::input);
    assert(m_holder);
    assert(m_property);

    auto s    = handle;
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
        m_holder->set_data(make_object<Bool>(val));
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
    const object_ptr<const StringDataProperty>& property,
    const socket_handle& s,
    const structured_node_graph& g,
    const node_window& nw)
    : basic_socket_drawer(s, g, nw)
    , m_holder {holder}
    , m_property {property}
  {
  }

  void data_type_socket<String>::_draw_content(
    const node_window& nw,
    const data_context& dctx,
    const view_context& vctx,
    node_window_draw_info& draw_info,
    ImVec2 pos,
    ImVec2 size) const
  {
    (void)nw, (void)vctx, (void)draw_info;
    assert(info.type() == socket_type::input);
    assert(m_holder);
    assert(m_property);

    auto s    = handle;
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
      auto str = value_cast<String>(m_holder->data());
      auto val = std::string(*str);
      ImGui::InputText("", &val);

      if (std::string(*str) != val) {
        m_holder->set_data(make_object<String>(val));
        dctx.exec(make_data_command(
          [](auto& ctx) { ctx.data().executor.notify_execute(); }));
      }
    }
    ImGui::PopStyleColor(4);
    ImGui::PopStyleVar(1);
    ImGui::PopItemWidth();
    ImGui::PopID();
  }

  data_type_socket<Color>::data_type_socket(
    const object_ptr<DataTypeHolder>& holder,
    const object_ptr<const ColorDataProperty>& property,
    const socket_handle& s,
    const structured_node_graph& g,
    const node_window& nw)
    : basic_socket_drawer(s, g, nw)
    , m_holder {holder}
    , m_property {property}
  {
  }

  auto data_type_socket<Color>::min_size(node_window_draw_info&) const -> ImVec2
  {
    auto label_size = calc_text_size("1.234", font_size_level::e15);
    auto padding    = get_socket_padding();
    auto height     = 16.f;
    return {label_size.x * 5 + 2 * padding.x, height + 2 * padding.y};
  }

  void data_type_socket<Color>::_draw_content(
    const node_window& nw,
    const data_context& dctx,
    const view_context& vctx,
    node_window_draw_info& draw_info,
    ImVec2 pos,
    ImVec2 size) const
  {
    (void)nw, (void)vctx, (void)draw_info;
    assert(info.type() == socket_type::input);
    assert(m_holder);
    assert(m_property);

    auto s    = handle;
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
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(5.f, 5.f));
    {
      auto c   = value_cast<Color>(m_holder->data());
      auto val = *c;
      ImGui::ColorEdit4("", &(val.r), ImGuiColorEditFlags_Float);

      if (*c != val) {
        m_holder->set_data(make_object<Color>(val));
        dctx.exec(make_data_command(
          [](auto& ctx) { ctx.data().executor.notify_execute(); }));
      }
    }
    ImGui::PopStyleColor(4);
    ImGui::PopStyleVar(2);
    ImGui::PopItemWidth();
    ImGui::PopID();
  }

  data_type_socket<FVec2>::data_type_socket(
    const object_ptr<DataTypeHolder>& holder,
    const object_ptr<const FVec2DataProperty>& property,
    const socket_handle& s,
    const structured_node_graph& g,
    const node_window& nw)
    : basic_socket_drawer(s, g, nw)
    , m_holder {holder}
    , m_property {property}
  {
  }

  auto data_type_socket<FVec2>::min_size(node_window_draw_info&) const -> ImVec2
  {
    auto label_size = calc_text_size("v:1234.5", font_size_level::e15);
    auto padding    = get_socket_padding();
    auto height     = 16.f;
    return {label_size.x * 2 + 2 * padding.x, height + 2 * padding.y};
  }

  void data_type_socket<FVec2>::_draw_content(
    const node_window& nw,
    const data_context& dctx,
    const view_context& vctx,
    node_window_draw_info& draw_info,
    ImVec2 pos,
    ImVec2 size) const
  {
    (void)nw, (void)vctx, (void)draw_info;
    assert(info.type() == socket_type::input);
    assert(m_holder);
    assert(m_property);

    auto s    = handle;
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
      auto& style = ImGui::GetStyle();

      auto vec = value_cast<FVec2>(m_holder->data());
      auto val = *vec;

      auto step = 1.f;
      auto lo   = std::numeric_limits<float>::lowest();
      auto hi   = std::numeric_limits<float>::max();

      ImGui::SetNextItemWidth((size.x - style.ItemInnerSpacing.x) / 2);
      ImGui::DragFloat("##x", &val.x, step, lo, hi, "x:%.1f");
      ImGui::SetNextItemWidth((size.x - style.ItemInnerSpacing.x) / 2);
      ImGui::SameLine(0, style.ItemInnerSpacing.x);
      ImGui::DragFloat("##y", &val.y, step, lo, hi, "y:%.1f");

      if (*vec != val) {
        m_holder->set_data(make_object<FVec2>(val));
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
