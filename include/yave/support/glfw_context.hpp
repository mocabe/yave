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
#include <unordered_map>

namespace yave {

  struct glfw_window_deleter
  {
    void operator()(GLFWwindow* window) noexcept;
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
    [[nodiscard]] std::unique_ptr<GLFWwindow, glfw_window_deleter>
      create_window(uint32_t width, uint32_t height, const char* name) const;

    /// poll events
    void poll_events() const;
  };

  /// Window data stored in user pointer
  class glfw_window_data
  {
  public:
    glfw_window_data();
    [[nodiscard]] bool add(const std::string& key, void* data);
    [[nodiscard]] void* find(const std::string& str) const;

  private:
    std::unordered_map<std::string, void*> m_map;
  };
} // namespace yave
