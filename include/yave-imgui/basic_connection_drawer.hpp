//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave-imgui/node_window_drawables.hpp>

namespace yave::editor::imgui {

  class basic_connection_drawer : public connection_drawable
  {
  public:
    basic_connection_drawer(
      const connection_handle& c,
      const structured_node_graph& g,
      const node_window& nw);

    void draw(
      const node_window& nw,
      data_context& dctx,
      view_context& vctx,
      node_window_draw_info& draw_info,
      ImDrawListSplitter& splitter,
      int channel) override;
  };

} // namespace yave::editor::imgui
