//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/config/config.hpp>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>


namespace yave::glfw {

  class glfw_context;
  class glfw_window;

  /// GLFW window
  class glfw_window
  {
    friend class glfw_context;

  public:
    /// Move ctor
    glfw_window(glfw_window&&) noexcept;
    /// Dtor
    ~glfw_window() noexcept;

  public:
    /// Get GLFWwindow
    [[nodiscard]] auto get() const -> GLFWwindow*;

  public:
    /// Add key-value pair of user data pointer.
    /// \note Adding to existing key fails.
    [[nodiscard]] static bool add_user_data(
      GLFWwindow* window,
      const std::string& key,
      void* data) noexcept;

    /// Get window data
    [[nodiscard]] static auto get_user_data(
      GLFWwindow* window,
      const std::string& key) noexcept -> void*;

    /// Remove window data
    static void remove_user_data(
      GLFWwindow* window,
      const std::string& key) noexcept;

  public:
    /// Get current refresh rate of window
    auto refresh_rate() const -> uint32_t;

  private:
    glfw_window(GLFWwindow* window);

  private:
    /// window
    GLFWwindow* m_window;
  };

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
    [[nodiscard]] auto create_window(
      uint32_t width,
      uint32_t height,
      const char* name) const -> glfw_window;

  public:
    /// poll events
    void poll_events() const;
    /// wait events
    void wait_events() const;
  };

} // namespace yave::glfw
