//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/editor/execute_thread.hpp>
#include <yave/editor/editor_data.hpp>
#include <yave/obj/frame_demand/frame_demand.hpp>
#include <yave/node/core/function.hpp>
#include <yave/lib/image/image.hpp>

#include <yave/support/log.hpp>

#include <thread>
#include <mutex>
#include <atomic>

YAVE_DECL_G_LOGGER(execute_thread);

namespace yave::editor {

  namespace {

    /// internal task thread for execution
    class execute_thread
    {
    private:
      data_context& data_ctx;

    private:
      std::thread thread;
      std::mutex mtx;
      std::condition_variable cond;

    private:
      std::atomic<bool> terminate_flag = false;
      std::atomic<bool> execute_flag   = false;

    private:
      std::exception_ptr exception;

      void check_failure()
      {
        if (!thread.joinable()) {
          if (exception != nullptr) {
            // exception thrown inside thread
            throw execute_thread_failure(exception);
          }
        }
      }

    public:
      execute_thread(data_context& dctx)
        : data_ctx {dctx}
      {
        init_logger();
      }

      bool is_running()
      {
        return thread.joinable();
      }

    public:
      static auto exec_frame_output(compiler::executable&& exe, yave::time t)
        -> std::optional<image>
      {
        try {
          auto r = value_cast<FrameBuffer>(exe.execute(t));
          // load result to host memory
          auto img = image(r->width(), r->height(), r->format());
          r->read_data(0, 0, r->width(), r->height(), img.data());
          return img;
        } catch (const exception_result& e) {

          // exception object
          auto eo   = e.exception();
          auto msg  = eo->message();
          auto erro = eo->error();

          Error(g_logger, "Failed to execute frame output: {}", msg);

          // print additional info

          if (auto err = value_cast_if<BadValueCast>(erro)) {
            Error(
              g_logger,
              "  BadValueCast: from:{}, to:{}",
              to_string(err->from),
              to_string(err->to));
          }

          if (auto err = value_cast_if<TypeError>(erro)) {
            Error(
              g_logger,
              "  TypeError: type:{}, t1:{}, t2:{}",
              err->error_type,
              to_string(err->t1),
              to_string(err->t2));
          }

          if (auto err = value_cast_if<ResultError>(erro)) {
            Error(g_logger, "  ResultError: type:{}", err->error_type);
          }

        } catch (...) {
          Error(g_logger, "unknown exception detected during execution!");
        }
        return std::nullopt;
      }

      void start()
      {
        check_failure();

        thread = std::thread([&] {
          try {
            while (true) {

              {
                std::unique_lock lck {mtx};
                cond.wait(lck, [&] { return terminate_flag || execute_flag; });
              }

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

                  // apply updates
                  for (auto&& u : updates.consume_updates())
                    u.apply();

                  // get compiled result
                  if (auto&& r = compiler.executable())
                    exe = r->clone();

                  time = executor.arg_time();
                }

                // no compile result
                if (!exe)
                  continue;

                assert(exe && time);

                assert(same_type(
                  exe->type(), object_type<node_closure<FrameBuffer>>()));

                auto bgn = std::chrono::high_resolution_clock::now();

                // execute app tree.
                auto img = exec_frame_output(std::move(*exe), *time);

                auto end = std::chrono::high_resolution_clock::now();

                {
                  auto lck       = data_ctx.lock();
                  auto& data     = lck.get_data<editor_data>();
                  auto& executor = data.execute_thread();

                  using namespace std::chrono;

                  executor.set_exec_results(
                    {.img       = std::move(img),
                     .duration  = duration_cast<milliseconds>(end - bgn),
                     .timestamp = steady_clock::now()});
                }
              }
            }
          } catch (...) {
            exception = std::current_exception();
          }
        });
      }

      void stop()
      {
        check_failure();
        terminate_flag = true;
        cond.notify_one();
        thread.join();
      }

      void notify_execute()
      {
        check_failure();
        execute_flag = true;
        cond.notify_one();
      }
    };

  } // namespace

  class execute_thread_data::impl
  {
    /// execute thread
    execute_thread m_thread;
    /// result image
    std::optional<yave::image> m_result;
    /// arg time
    yave::time m_arg_time;
    /// duration of run
    std::chrono::milliseconds m_exec_duration;
    /// timestamp of run
    std::chrono::steady_clock::time_point m_exec_timestamp;

  public:
    impl(data_context& dctx)
      : m_thread {dctx}
    {
    }

    bool is_thread_running()
    {
      return m_thread.is_running();
    }

    void init()
    {
      m_thread.start();
    }

    void deinit()
    {
      m_thread.stop();
    }

    void notify_execute()
    {
      m_thread.notify_execute();
    }

  public:
    auto arg_time() const
    {
      return m_arg_time;
    }

    void set_arg_time(yave::time t)
    {
      m_arg_time = t;
    }

    auto exec_duration() const
    {
      return m_exec_duration;
    }

    auto exec_timestamp() const
    {
      return m_exec_timestamp;
    }

    auto& exec_result()
    {
      return m_result;
    }

    void set_exec_results(exec_results results)
    {
      m_result         = std::move(results.img);
      m_exec_duration  = results.duration;
      m_exec_timestamp = results.timestamp;
    }
  };

  execute_thread_data::execute_thread_data(data_context& dctx)
    : m_pimpl {std::make_unique<impl>(dctx)}
  {
    // call init() to start
    assert(!m_pimpl->is_thread_running());
  }

  execute_thread_data::~execute_thread_data() noexcept
  {
    // deinit() should be called
    assert(!m_pimpl->is_thread_running());
  }

  void execute_thread_data::init()
  {
    m_pimpl->init();
  }

  void execute_thread_data::deinit()
  {
    m_pimpl->deinit();
  }

  void execute_thread_data::notify_execute()
  {
    m_pimpl->notify_execute();
  }

  auto execute_thread_data::arg_time() const -> yave::time
  {
    return m_pimpl->arg_time();
  }

  void execute_thread_data::set_arg_time(yave::time t)
  {
    return m_pimpl->set_arg_time(t);
  }

  auto execute_thread_data::exec_duration() const -> std::chrono::milliseconds
  {
    return m_pimpl->exec_duration();
  }

  auto execute_thread_data::exec_timestamp() const
    -> std::chrono::steady_clock::time_point
  {
    return m_pimpl->exec_timestamp();
  }

  auto execute_thread_data::exec_result() -> std::optional<yave::image>&
  {
    return m_pimpl->exec_result();
  }

  void execute_thread_data::set_exec_results(exec_results results)
  {
    m_pimpl->set_exec_results(std::move(results));
  }

} // namespace yave::editor