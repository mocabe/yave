//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave-imgui/data_type_socket.hpp>
#include <yave-imgui/data_commands.hpp>
#include <yave/editor/editor_data.hpp>
#include <yave/editor/data_command.hpp>
#include <imgui_stdlib.h>

namespace yave::editor::imgui {

  namespace {

    template <class T>
    auto get_current_argument_data(
      const object_ptr<NodeArgument>& arg,
      const data_context& dctx)
    {
      auto data = dctx.get_data<editor_data>();

      auto p = arg->prop_tree();

      // get staged change in update channel
      auto staged = data //
                      .ref()
                      .update_channel()
                      .get_current_value(p);

      // return staged data or current value in argument holder
      return get_node_argument_value<T>(staged ? staged : p);
    }
  } // namespace

  data_type_socket<Float>::data_type_socket(
    const object_ptr<NodeArgument>& arg,
    const socket_handle& s,
    const structured_node_graph& g,
    const node_window& nw)
    : basic_socket_drawer(s, g, nw)
    , m_arg {arg}
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

    assert(info.is_input());
    assert(m_arg);

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
      auto val = (float)get_current_argument_data<Float>(m_arg, dctx);

      auto p    = m_arg->prop_tree();
      auto step = (float)*value_cast<Float>(p->property("step"));
      auto min  = (float)*value_cast<Float>(p->property("min"));
      auto max  = (float)*value_cast<Float>(p->property("max"));

      ImGui::DragFloat("", &val, step, min, max, (name + ":%.3f").c_str());

      auto diff = get_node_argument_diff<Float>(p, val);

      if (!diff.empty()) {
        dctx.cmd(std::make_unique<dcmd_push_update>(diff));
        dctx.cmd(std::make_unique<dcmd_notify_execute>());
      }
    }
    ImGui::PopStyleColor(4);
    ImGui::PopStyleVar(1);
    ImGui::PopItemWidth();
    ImGui::PopID();
  }

  data_type_socket<Int>::data_type_socket(
    const object_ptr<NodeArgument>& arg,
    const socket_handle& s,
    const structured_node_graph& g,
    const node_window& nw)
    : basic_socket_drawer(s, g, nw)
    , m_arg {arg}
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
    assert(info.is_input());
    assert(m_arg);

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
      auto val = (int)get_current_argument_data<Int>(m_arg, dctx);

      auto p    = m_arg->prop_tree();
      auto step = (int)*value_cast<Int>(p->property("step"));
      auto min  = (int)*value_cast<Int>(p->property("min"));
      auto max  = (int)*value_cast<Int>(p->property("max"));

      ImGui::DragInt("", &val, step, min, max, (name + ":%.3f").c_str());

      auto diff = get_node_argument_diff<Int>(p, val);

      if (!diff.empty()) {
        dctx.cmd(std::make_unique<dcmd_push_update>(diff));
        dctx.cmd(std::make_unique<dcmd_notify_execute>());
      }
    }
    ImGui::PopStyleColor(4);
    ImGui::PopStyleVar(1);
    ImGui::PopItemWidth();
    ImGui::PopID();
  }

  data_type_socket<Bool>::data_type_socket(
    const object_ptr<NodeArgument>& arg,
    const socket_handle& s,
    const structured_node_graph& g,
    const node_window& nw)
    : basic_socket_drawer(s, g, nw)
    , m_arg {arg}
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
    assert(info.is_input());
    assert(m_arg);

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
      auto val = get_current_argument_data<Bool>(m_arg, dctx);

      ImGui::Checkbox("", &val);

      auto diff = get_node_argument_diff<Bool>(m_arg->prop_tree(), val);

      if (!diff.empty()) {
        dctx.cmd(std::make_unique<dcmd_push_update>(diff));
        dctx.cmd(std::make_unique<dcmd_notify_execute>());
      }
    }
    ImGui::PopStyleColor(4);
    ImGui::PopStyleVar(1);
    ImGui::PopItemWidth();
    ImGui::PopID();
  }

  data_type_socket<String>::data_type_socket(
    const object_ptr<NodeArgument>& arg,
    const socket_handle& s,
    const structured_node_graph& g,
    const node_window& nw)
    : basic_socket_drawer(s, g, nw)
    , m_arg {arg}
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
    assert(info.is_input());
    assert(m_arg);

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
      auto val = (std::string)get_current_argument_data<String>(m_arg, dctx);

      ImGui::InputText("", &val);

      auto diff =
        get_node_argument_diff<String>(m_arg->prop_tree(), data::string(val));

      if (!diff.empty()) {
        dctx.cmd(std::make_unique<dcmd_push_update>(diff));
        dctx.cmd(std::make_unique<dcmd_notify_execute>());
      }
    }
    ImGui::PopStyleColor(4);
    ImGui::PopStyleVar(1);
    ImGui::PopItemWidth();
    ImGui::PopID();
  }

  data_type_socket<Color>::data_type_socket(
    const object_ptr<NodeArgument>& arg,
    const socket_handle& s,
    const structured_node_graph& g,
    const node_window& nw)
    : basic_socket_drawer(s, g, nw)
    , m_arg {arg}
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
    assert(info.is_input());
    assert(m_arg);

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
      auto val = get_current_argument_data<Color>(m_arg, dctx);

      ImGui::ColorEdit4("", &(val.r), ImGuiColorEditFlags_Float);

      auto diff = get_node_argument_diff<Color>(m_arg->prop_tree(), val);

      if (!diff.empty()) {
        dctx.cmd(std::make_unique<dcmd_push_update>(diff));
        dctx.cmd(std::make_unique<dcmd_notify_execute>());
      }
    }
    ImGui::PopStyleColor(4);
    ImGui::PopStyleVar(2);
    ImGui::PopItemWidth();
    ImGui::PopID();
  }

  data_type_socket<Vec2>::data_type_socket(
    const object_ptr<NodeArgument>& arg,
    const socket_handle& s,
    const structured_node_graph& g,
    const node_window& nw)
    : basic_socket_drawer(s, g, nw)
    , m_arg {arg}
  {
  }

  auto data_type_socket<Vec2>::min_size(node_window_draw_info&) const -> ImVec2
  {
    auto label_size = calc_text_size("v:1234.5", font_size_level::e15);
    auto padding    = get_socket_padding();
    auto height     = 16.f;
    return {label_size.x * 2 + 2 * padding.x, height + 2 * padding.y};
  }

  void data_type_socket<Vec2>::_draw_content(
    const node_window& nw,
    const data_context& dctx,
    const view_context& vctx,
    node_window_draw_info& draw_info,
    ImVec2 pos,
    ImVec2 size) const
  {
    (void)nw, (void)vctx, (void)draw_info;
    assert(info.is_input());
    assert(m_arg);

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

      auto val = (glm::fvec2)get_current_argument_data<Vec2>(m_arg, dctx);

      auto step = 1.f;
      auto lo   = std::numeric_limits<float>::lowest();
      auto hi   = std::numeric_limits<float>::max();

      ImGui::SetNextItemWidth((size.x - style.ItemInnerSpacing.x) / 2);
      ImGui::DragFloat("##x", &val.x, step, lo, hi, "x:%.1f");
      ImGui::SetNextItemWidth((size.x - style.ItemInnerSpacing.x) / 2);
      ImGui::SameLine(0, style.ItemInnerSpacing.x);
      ImGui::DragFloat("##y", &val.y, step, lo, hi, "y:%.1f");

      auto diff = get_node_argument_diff<Vec2>(m_arg->prop_tree(), val);

      if (!diff.empty()) {
        dctx.cmd(std::make_unique<dcmd_push_update>(diff));
        dctx.cmd(std::make_unique<dcmd_notify_execute>());
      }
    }
    ImGui::PopStyleColor(4);
    ImGui::PopStyleVar(1);
    ImGui::PopItemWidth();
    ImGui::PopID();
  }
} // namespace yave::editor::imgui
