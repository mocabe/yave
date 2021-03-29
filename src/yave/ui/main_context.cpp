//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/ui/main_context.hpp>
#include <yave/ui/view_context.hpp>
#include <yave/ui/key_events.hpp>
#include <yave/ui/mouse_events.hpp>
#include <yave/ui/glfw_context.hpp>
#include <yave/ui/vulkan_context.hpp>

#include <yave/support/log.hpp>

#include <queue>
#include <mutex>
#include <optional>

namespace yave::ui {

  class main_context::impl
  {
    // ref to context
    main_context& m_self;
    // back ref to view ctx
    view_context* m_pview = nullptr;
    // glfw
    glfw_context m_glfw;
    // vulkan
    vulkan_context m_vulkan;
    // exit flag
    std::atomic<bool> m_exit = false;
    // mutex
    std::mutex m_mtx;
    // command queues
    std::queue<main_command> m_queue;

  public:
    impl(main_context& self)
      : m_self {self}
      , m_glfw {self}
    {
    }

    ~impl() noexcept
    {
    }

    void set_view_ctx(view_context& vctx)
    {
      m_pview = &vctx;
    }

    auto view_ctx() -> view_context&
    {
      assert(m_pview);
      return *m_pview;
    }

    auto& glfw()
    {
      return m_glfw;
    }

    auto& vulkan()
    {
      return m_vulkan;
    }

  private:
    auto lock()
    {
      return std::unique_lock(m_mtx);
    }

    auto pop_cmd() -> std::optional<main_command>
    {
      auto lck = lock();
      if (!m_queue.empty()) {
        auto ret = std::move(m_queue.front());
        m_queue.pop();
        return ret;
      }
      return std::nullopt;
    }

    void exec_all()
    {
      while (auto cmd = pop_cmd()) {
        cmd->exec(m_self);
      }
    }

    bool should_exit() const
    {
      return m_exit.load();
    }

  public:
    void post(main_command cmd)
    {
      {
        auto lck = lock();
        m_queue.push(std::move(cmd));
      }
      wake();
    }

    void run()
    {
      while (!should_exit()) {
        m_glfw.wait();
        exec_all();
      }
    }

    void wake()
    {
      m_glfw.wake();
    }

    void exit()
    {
      post([&](auto&) { m_exit = true; });
    }
  };

  main_context::main_context()
    : m_pimpl {std::make_unique<impl>(*this)}
  {
  }

  main_context::~main_context() noexcept = default;

  void main_context::_post(main_command cmd) const
  {
    m_pimpl->post(std::move(cmd));
  }

  void main_context::set_view_ctx(view_context& vctx, passkey<view_context>)
  {
    m_pimpl->set_view_ctx(vctx);
  }

  auto main_context::view_ctx() -> view_context&
  {
    return m_pimpl->view_ctx();
  }

  auto main_context::glfw_ctx() -> glfw_context&
  {
    return m_pimpl->glfw();
  }

  auto main_context::vulkan_ctx() -> vulkan_context&
  {
    return m_pimpl->vulkan();
  }

  void main_context::run()
  {
    m_pimpl->run();
  }

  void main_context::wake()
  {
    m_pimpl->wake();
  }

  void main_context::exit()
  {
    m_pimpl->exit();
  }

} // namespace yave::ui