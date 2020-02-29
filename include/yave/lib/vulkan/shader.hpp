//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/lib/vulkan/vulkan_context.hpp>

namespace yave::vulkan {
  /// Compile shader code into SPIRV bytecode
  [[nodiscard]] auto compileVertShader(const char* code)
    -> std::vector<uint32_t>;
  /// Compile shader code into SPIRV bytecode
  [[nodiscard]] auto compileFragShader(const char* code)
    -> std::vector<uint32_t>;
}