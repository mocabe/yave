//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/ui/typedefs.hpp>
#include <yave/ui/command.hpp>
#include <yave/ui/passkey.hpp>
#include <yave/ui/size.hpp>
#include <yave/ui/vec.hpp>

#include <memory>
#include <future>
#include <GLFW/glfw3.h>

namespace yave::ui {

  class main_context;
  class view_context;
  class glfw_context;
  class vulkan_context;

  /// Main context command
  using main_command = command<main_context>;

  /// Main loop context
  ///
  /// This class represents main loop of an application.
  ///
  /// Functionalities:
  ///  - Capture events sent from OS
  ///  - Translate OS event to UI events
  ///  - Post UI events to view context
  ///  - Initialize GLFW API
  ///  - Initialize Vulkan API
  ///
  /// Internal components:
  ///  - Vulkan instance
  ///  - GLFW instance
  ///  - Main event loop
  ///  - Event translator
  class main_context
  {
    class impl;
    std::unique_ptr<impl> m_pimpl;

  public:
    main_context();
    ~main_context() noexcept;

  public:
    // for view_context
    void set_view_ctx(view_context&, passkey<view_context>);
    // get view
    auto view_ctx() -> view_context&;

  private:
    void _post(main_command) const;

  public:
    /// Post new command
    template <class F>
    void post(F&& f)
    {
      _post(main_command(std::forward<F>(f)));
    }

    /// Post with future
    template <class F>
    auto sync_post(F&& f)
    {
      using result_t = std::invoke_result_t<F, main_context&>;

      auto promise = std::promise<result_t>();
      auto future  = promise.get_future();

      post(main_command(
        [=, f = std::forward<F>(f), p = std::move(promise)](auto&) mutable {
          if constexpr (std::is_same_v<result_t, void>) {
            f(*this);
            p.set_value();
          } else
            p.set_value(f(*this));
        }));

      return future;
    }

    /// Send empty message to wake up
    void wake();

    /// Run main loop
    void run();

    /// Exit main loop
    void exit();

  public:
    /// GLFW API
    auto glfw_ctx() -> glfw_context&;
    /// Vulkan API
    auto vulkan_ctx() -> vulkan_context&;
  };

} // namespace yave::ui