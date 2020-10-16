//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/editor/execute_thread.hpp>
#include <yave/editor/editor_data.hpp>
#include <yave/obj/frame_demand/frame_demand.hpp>
#include <yave/node/core/function.hpp>
#include <yave/lib/image/image.hpp>

#include <thread>
#include <mutex>
#include <atomic>

namespace yave::editor {

  class execute_thread::impl
  {
  public:
    data_context& data_ctx;

  public:
    std::thread thread;
    std::mutex mtx;
    std::condition_variable cond;
    std::atomic<bool> terminate_flag = false;
    std::atomic<bool> execute_flag   = false;

  public:
    std::exception_ptr exception;

    void check_exception()
    {
      if (exception)
        std::rethrow_exception(exception);
    }

  public:
    impl(data_context& dctx)
      : data_ctx {dctx}
    {
    }

  public:
    void start()
    {
      thread = std::thread([&] {
        try {
          while (true) {

            std::unique_lock lck {mtx};
            cond.wait(lck, [&] { return terminate_flag || execute_flag; });

            if (terminate_flag)
              break;

            if (execute_flag) {

              execute_flag = false;

              std::optional<compiler::executable> exe;
              std::optional<time> time;
              {
                auto lck       = data_ctx.lock();
                auto& data     = lck.get_data<editor_data>();
                auto& executor = data.execute_thread();
                auto& compiler = data.compile_thread();
                auto& updates  = data.update_channel();

                if (!executor.initialized())
                  continue;

                // apply updates
                for (auto&& u : updates.consume_updates())
                  u.apply();

                // get compiled result
                if (auto&& r = compiler.executable())
                  exe = r->clone();

                time = executor.time();
              }

              // no compile result
              if (!exe)
                continue;

              assert(exe && time);

              assert(same_type(
                exe->type(), object_type<node_closure<FrameBuffer>>()));

              auto bgn = std::chrono::high_resolution_clock::now();

              // execute app tree.
              auto result = [&]() -> std::optional<image> {
                try {
                  auto r = value_cast<FrameBuffer>(exe->execute(*time));
                  // load to host memory
                  auto img = image(r->width(), r->height(), r->format());
                  r->read_data(0, 0, r->width(), r->height(), img.data());
                  return img;
                } catch (...) {
                  // execution error
                  return std::nullopt;
                }
              }();

              auto end = std::chrono::high_resolution_clock::now();

              {
                auto lck             = data_ctx.lock();
                auto& data           = lck.get_data<editor_data>();
                auto& executor       = data.execute_thread();
                executor.m_result    = std::move(result);
                executor.m_exec_time = end - bgn;
                executor.m_timestamp = std::chrono::steady_clock::now();
              }
            }
          }
        } catch (...) {
          exception = std::current_exception();
        }
      });
    }

    auto wait_task()
    {
      return std::unique_lock {mtx};
    }

    void stop()
    {
      terminate_flag = true;
      cond.notify_one();
      thread.join();
    }

    void notify_execute()
    {
      execute_flag = true;
      cond.notify_one();
    }
  };

  execute_thread::execute_thread(data_context& dctx)
    : m_pimpl {std::make_unique<impl>(dctx)}
  {
    m_pimpl->start();
  }

  execute_thread::~execute_thread() noexcept
  {
    m_pimpl->stop();
  }

  void execute_thread::notify_execute()
  {
    m_pimpl->check_exception();
    m_pimpl->notify_execute();
  }

  auto execute_thread::wait_task() -> std::unique_lock<std::mutex>
  {
    return m_pimpl->wait_task();
  }
} // namespace yave::editor