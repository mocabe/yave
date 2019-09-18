//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/lib/imgui/ext_invisible_button.hpp>

#include <yave/lib/imgui/ext_vec2_ops.hpp>
#include <imgui_internal.h>

namespace yave::imgui {

  bool InvisibleButtonEx(
    const char* str_id,
    const ImVec2& size_arg,
    bool* out_hovered,
    bool* out_held)
  {
    using namespace ImGui;

    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
      return false;

    // Cannot use zero-size for InvisibleButton(). Unlike Button() there is not
    // way to fallback using the label size.
    IM_ASSERT(size_arg.x != 0.0f && size_arg.y != 0.0f);

    const ImGuiID id = window->GetID(str_id);
    ImVec2 size      = CalcItemSize(size_arg, 0.0f, 0.0f);
    const ImRect bb(window->DC.CursorPos, window->DC.CursorPos + size);
    ItemSize(bb);
    if (!ItemAdd(bb, id))
      return false;

    bool hovered, held;
    bool pressed =
      ButtonBehavior(bb, id, &hovered, &held, ImGuiButtonFlags_PressedOnClick);

    if (out_hovered)
      *out_hovered = hovered;

    if (out_held)
      *out_held = held;

    return pressed;
  }
}