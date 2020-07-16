//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave-imgui/basic_socket_drawer.hpp>
#include <yave/module/std/primitive/primitive.hpp>

namespace yave::editor::imgui {

  // Dispatch socket type based on data type
  template <class DataTpye>
  class data_type_socket;

  /// Float
  template <>
  class data_type_socket<Float> : public basic_socket_drawer
  {
    object_ptr<DataTypeHolder> m_holder;
    object_ptr<FloatDataProperty> m_property;

  public:
    data_type_socket(
      const object_ptr<DataTypeHolder>& holder,
      const object_ptr<FloatDataProperty>& property,
      const socket_handle& s,
      const structured_node_graph& g,
      const node_window& nw);

    auto min_size(node_window_draw_info&) const -> ImVec2 override;

    void _draw_content(
      const node_window& nw,
      data_context& dctx,
      view_context& vctx,
      node_window_draw_info& draw_info,
      ImVec2 pos,
      ImVec2 size) const override;
  };

  /// Int
  template <>
  class data_type_socket<Int> : public basic_socket_drawer
  {
    object_ptr<DataTypeHolder> m_holder;
    object_ptr<IntDataProperty> m_property;

  public:
    data_type_socket(
      const object_ptr<DataTypeHolder>& holder,
      const object_ptr<IntDataProperty>& property,
      const socket_handle& s,
      const structured_node_graph& g,
      const node_window& nw);

    auto min_size(node_window_draw_info&) const -> ImVec2 override;

    void _draw_content(
      const node_window& nw,
      data_context& dctx,
      view_context& vctx,
      node_window_draw_info& draw_info,
      ImVec2 pos,
      ImVec2 size) const override;
  };

  /// Bool
  template <>
  class data_type_socket<Bool> : public basic_socket_drawer
  {
    object_ptr<DataTypeHolder> m_holder;
    object_ptr<BoolDataProperty> m_property;

  public:
    data_type_socket(
      const object_ptr<DataTypeHolder>& holder,
      const object_ptr<BoolDataProperty>& property,
      const socket_handle& s,
      const structured_node_graph& g,
      const node_window& nw);

    void _draw_content(
      const node_window& nw,
      data_context& dctx,
      view_context& vctx,
      node_window_draw_info& draw_info,
      ImVec2 pos,
      ImVec2 size) const override;
  };

  /// String
  template <>
  class data_type_socket<String> : public basic_socket_drawer
  {
    object_ptr<DataTypeHolder> m_holder;
    object_ptr<StringDataProperty> m_property;

  public:
    data_type_socket(
      const object_ptr<DataTypeHolder>& holder,
      const object_ptr<StringDataProperty>& property,
      const socket_handle& s,
      const structured_node_graph& g,
      const node_window& nw);

    void _draw_content(
      const node_window& nw,
      data_context& dctx,
      view_context& vctx,
      node_window_draw_info& draw_info,
      ImVec2 pos,
      ImVec2 size) const override;
  };

} // namespace yave::editor::imgui