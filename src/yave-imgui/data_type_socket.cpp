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
    data_context& dctx,
    view_context& vctx,
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

      dctx.exec(make_data_command([f, val](auto& ctx) {
        if (*f != val) {
          *f = val;
          ctx.data().executor.notify_execute();
        }
      }));
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
    data_context& dctx,
    view_context& vctx,
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

      dctx.exec(make_data_command([i, val](auto& ctx) {
        if (*i != val) {
          *i = val;
          ctx.data().executor.notify_execute();
        }
      }));
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
    : basic_socket_drawer(s, g, nw)
    , m_holder {holder}
    , m_property {property}
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

      dctx.exec(make_data_command([b, val](auto& ctx) {
        if (*b != val) {
          *b = val;
          ctx.data().executor.notify_execute();
        }
      }));
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
    : basic_socket_drawer(s, g, nw)
    , m_holder {holder}
    , m_property {property}
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

      dctx.exec(make_data_command([str, val](auto& ctx) {
        if (std::string(*str) != val) {
          *str = yave::string(val);
          ctx.data().executor.notify_execute();
        }
      }));
    }
    ImGui::PopStyleColor(4);
    ImGui::PopStyleVar(1);
    ImGui::PopItemWidth();
    ImGui::PopID();
  }

  data_type_socket<Color>::data_type_socket(
    const object_ptr<DataTypeHolder>& holder,
    const object_ptr<ColorDataProperty>& property,
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
    data_context& dctx,
    view_context& vctx,
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

      dctx.exec(make_data_command([c, val](auto& ctx) {
        if (*c != val) {
          *c = val;
          ctx.data().executor.notify_execute();
        }
      }));
    }
    ImGui::PopStyleColor(4);
    ImGui::PopStyleVar(2);
    ImGui::PopItemWidth();
    ImGui::PopID();
  }

  data_type_socket<FVec2>::data_type_socket(
    const object_ptr<DataTypeHolder>& holder,
    const object_ptr<FVec2DataProperty>& property,
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
    data_context& dctx,
    view_context& vctx,
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

      dctx.exec(make_data_command([vec, val](auto& ctx) {
        if (*vec != val) {
          *vec = val;
          ctx.data().executor.notify_execute();
        }
      }));
    }
    ImGui::PopStyleColor(4);
    ImGui::PopStyleVar(1);
    ImGui::PopItemWidth();
    ImGui::PopID();
  }
} // namespace yave::editor::imgui
