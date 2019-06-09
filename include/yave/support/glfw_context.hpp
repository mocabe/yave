//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/core/config.hpp>

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <memory>

namespace yave {

  struct glfw_window_deleter
  {
    void operator()(GLFWwindow* window);
  };

  /// GLFW context
  class glfw_context
  {
  public:
    /// Initialize GLFW
    glfw_context();
    /// Terminate GLFW
    ~glfw_context() noexcept;

    /// Create window
    std::unique_ptr<GLFWwindow, glfw_window_deleter>
      create_window(uint32_t width, uint32_t height, const char* name) const;
  };

} // namespace yave
