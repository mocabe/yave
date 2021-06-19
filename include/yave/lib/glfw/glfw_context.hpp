//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/core/config.hpp>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <string>

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
    /// Get glfw context
    [[nodiscard]] auto glfw_ctx() -> glfw::glfw_context&;
    [[nodiscard]] auto glfw_ctx() const -> const glfw::glfw_context&;
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
    [[nodiscard]] auto refresh_rate() const -> uint32_t;

    /// Get current window pos
    [[nodiscard]] auto pos() const -> glm::u32vec2;

    /// Get current window size
    [[nodiscard]] auto size() const -> glm::u32vec2;

    /// Get current frame buffer size
    [[nodiscard]] auto buffer_size() const -> glm::u32vec2;

    /// Get current windlw title
    [[nodiscard]] auto title() const -> std::string;

    /// Should close?
    [[nodiscard]] bool should_close() const;

    /// focused?
    [[nodiscard]] bool focused() const;

    /// hovered?
    [[nodiscard]] bool hovered() const;

  private:
    glfw_window(
      glfw::glfw_context& ctx,
      std::string titne,
      uint32_t width,
      uint32_t height);

  private:
    glfw::glfw_context& m_ctx;
    /// window
    GLFWwindow* m_window;
    /// window title
    std::string m_title;
  };

  /// GLFW context
  class glfw_context
  {
  public:
    struct create_info
    {
      // currently no option
    };

  private:
    static auto _init_create_info() -> create_info
    {
      return create_info();
    }

  public:
    /// Initialize GLFW
    glfw_context(create_info info = _init_create_info());
    /// Terminate GLFW
    ~glfw_context() noexcept;
    /// Deleted
    glfw_context(const glfw_context&) = delete;

  public:
    /// Create window
    [[nodiscard]] auto create_window(
      uint32_t width,
      uint32_t height,
      const char* name) -> glfw_window;

  public:
    /// poll events
    void poll_events() const;
    /// wait events
    void wait_events() const;
  };

} // namespace yave::glfw