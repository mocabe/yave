//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/config/config.hpp>
#include <imgui.h>

namespace yave::imgui {

  /// ImGui::InvisibleButton with some extra arguments
  bool InvisibleButtonEx(
    const char* str_id,
    const ImVec2& size_arg,
    bool* out_hovered = nullptr,
    bool* out_held    = nullptr);

} // namespace yave::imgui