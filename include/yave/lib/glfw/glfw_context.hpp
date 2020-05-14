//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/config/config.hpp>
#include <yave/support/enum_flag.hpp>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>


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
    enum class init_flags
    {
      enable_logging = 1 << 0,
    };

  private:
    static auto _init_flags() noexcept -> init_flags;

  public:
    /// Initialize GLFW
    glfw_context(init_flags flags = _init_flags());
    /// Terminate GLFW
    ~glfw_context() noexcept;

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

namespace yave {

  template <>
  struct is_enum_flag<glfw::glfw_context::init_flags> : std::true_type
  {
  };
} // namespace yave