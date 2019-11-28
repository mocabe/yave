//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/node_handle.hpp>
#include <yave/lib/imgui/imgui_context.hpp>
#include <yave-imgui/editor/input_state.hpp>

namespace yave::editor::imgui {

  // fwd
  class editor_context;
  struct socket_view;

  /// Node view interface
  struct node_view
  {
    /// calculate minimum size to render
    virtual auto min_size() const -> ImVec2 = 0;
    /// draw node.
    /// \param base cursor pos
    virtual void draw(const ImVec2& base) const = 0;
    /// handle input.
    /// \param base cursor pos
    /// \param state input state
    virtual void handle_input(const ImVec2& base, input_state& state) const = 0;
    /// dtor
    virtual ~node_view() noexcept = 0;
  };

  /// Node view
  struct basic_node_view : node_view
  {
  public:
    basic_node_view(const node_handle& h, const editor_context& ctx);

    /// handle
    node_handle handle;

    /// selected?
    bool is_selected;

    /// hovered?
    bool is_hovered;

    /// position
    ImVec2 position;

    /// size
    ImVec2 size;

    /// title
    std::string title;

    /// sockets
    std::vector<std::unique_ptr<socket_view>> sockets;

    /// socket positions
    std::vector<ImVec2> socket_positions;

  public:
    auto min_size() const -> ImVec2 override;
    void draw(const ImVec2&) const override;
    void handle_input(const ImVec2&, input_state&) const override;
  };

  /// Create node view object
  [[nodiscard]] auto create_node_view(
    const node_handle& node,
    const editor_context& ctx) -> std::unique_ptr<node_view>;

} // namespace yave::editor::imgui