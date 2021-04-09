//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/ui/typedefs.hpp>
#include <yave/ui/size.hpp>
#include <yave/ui/vec.hpp>
#include <yave/ui/passkey.hpp>

#include <GLFW/glfw3.h>
#include <thread>

namespace yave::ui {

  class main_context;
  class view_context;

  /// GLFW context
  /// TODO: move this into yave::glfw namespace
  class glfw_context
  {
    main_context& m_mctx;
    // main thread ID
    std::thread::id m_tid = {};

    void ensure_on_main();

    struct window_data
    {
      view_context& view_ctx;
    };

    static auto get_window_data(GLFWwindow* w) -> window_data&;

  public:
    glfw_context(main_context&);

    ~glfw_context() noexcept;

    auto main_ctx() -> main_context&;

    void wake();

    void wait();

    // These functions should only be called from main thread.
    auto create_window(std::u8string_view title, ui::size size) -> GLFWwindow*;
    void destroy_window(GLFWwindow* w);
    void show_window(GLFWwindow* w);
    void hide_window(GLFWwindow* w);
    void rename_window(GLFWwindow* w, std::u8string_view name);
    void resize_window(GLFWwindow* w, ui::size size);
    void move_window(GLFWwindow* w, ui::vec pos);
    auto window_size(GLFWwindow* win) -> ui::size;
    auto window_pos(GLFWwindow* win) -> ui::vec;
    void focus_window(GLFWwindow* win);
    void request_window_attention(GLFWwindow* win);
    void set_window_max_size(GLFWwindow* win, ui::size size);
    void set_window_min_size(GLFWwindow* win, ui::size size);
  };

} // namespace yave::ui