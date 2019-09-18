//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/lib/imgui/imgui_context.hpp>
#include <yave/lib/imgui/extension.hpp>

using namespace yave;
using namespace yave::imgui;
using namespace ImGui;

void test_normal_button()
{
  static bool open = true;
  Begin("Normal Button", &open);
  auto base_cursor = GetCursorScreenPos();
  SetCursorScreenPos(base_cursor + ImVec2(100, 100));
  Button("normal inner", ImVec2(100, 100));
  SetCursorScreenPos(base_cursor + ImVec2(0, 0));
  Button("normal outer", ImVec2(200, 200));
  End();
}

void test_invisible_button()
{
  static bool open = true;
  Begin("Invisible Button", &open);
  auto base_cursor = GetCursorScreenPos();

  auto draw_list = GetWindowDrawList();

  SetCursorScreenPos(base_cursor + ImVec2(100, 100));
  InvisibleButton("invis inner", ImVec2(100, 100));

  if (IsItemHovered()) {
    draw_list->AddRectFilled(
      base_cursor + ImVec2(100, 100),
      base_cursor + ImVec2(200, 200),
      IM_COL32(75, 75, 75, 255));
  }

  SetCursorScreenPos(base_cursor + ImVec2(0, 0));
  InvisibleButton("invid outer", ImVec2(200, 200));

  if (IsItemHovered()) {
    draw_list->AddRectFilled(
      base_cursor + ImVec2(0, 0),
      base_cursor + ImVec2(200, 200),
      IM_COL32(60, 60, 60, 255));
  }

  End();
}

void test_invisible_button_ex()
{
  static bool open = true;
  Begin("Invisible Button Ex", &open);
  auto base_cursor = GetCursorScreenPos();

  SetCursorScreenPos(base_cursor + ImVec2(100, 100));
  bool inner = false;
  InvisibleButtonEx("ex inner", ImVec2(100, 100), &inner);
  SetCursorScreenPos(base_cursor + ImVec2(0, 0));
  bool outer = false;
  InvisibleButtonEx("ex outer", ImVec2(200, 200), &outer);

  auto draw_list = GetWindowDrawList();

  if (outer) {
    draw_list->AddRectFilled(
      base_cursor + ImVec2(0, 0),
      base_cursor + ImVec2(200, 200),
      IM_COL32(60, 60, 60, 255));
  }
  if (inner) {
    draw_list->AddRectFilled(
      base_cursor + ImVec2(100, 100),
      base_cursor + ImVec2(200, 200),
      IM_COL32(75, 75, 75, 255));
  }
  End();
}

int main()
{
  imgui_context imgui(true);
  while (!imgui.window_context().should_close()) {
    imgui.begin();
    {
      test_normal_button();
      test_invisible_button();
      test_invisible_button_ex();
    }
    imgui.end();
    imgui.render();
  }
}