//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/config/config.hpp>

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <memory>
#include <unordered_map>

namespace yave::glfw {

  // clang-format off

  struct glfw_window_deleter
  {
    void operator()(GLFWwindow* window) noexcept;
  };

  using unique_glfw_window = std::unique_ptr<GLFWwindow, glfw_window_deleter>;

  /// GLFW context
  class glfw_context
  {
  public:
    /// Initialize GLFW
    glfw_context();
    /// Terminate GLFW
    ~glfw_context() noexcept;

  public:
    /// Create window
    [[nodiscard]] 
    unique_glfw_window create_window(uint32_t width, uint32_t height, const char* name) const;
  
  public:
    /// poll events
    void poll_events() const;
    /// wait events
    void wait_events() const;
  };

  /// Window data stored in user pointer
  class glfw_window_data
  {
  public:
    /// Ctor
    glfw_window_data();
  
  public:
    /// Add new data pointer
    [[nodiscard]] 
    bool  add(const std::string& key, void* data);
    /// Find data pointer
    [[nodiscard]] 
    void* find(const std::string& str) const;

  private:
    std::unordered_map<std::string, void*> m_map;
  };

  // clang-format on

} // namespace yave::vulkan
