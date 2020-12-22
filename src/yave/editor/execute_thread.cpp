//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/editor/execute_thread.hpp>
#include <yave/editor/editor_data.hpp>
#include <yave/obj/frame_demand/frame_demand.hpp>
#include <yave/signal/specifier.hpp>
#include <yave/rts/to_string.hpp>
#include <yave/lib/image/image.hpp>

#include <yave/support/log.hpp>

#include <thread>
#include <mutex>
#include <atomic>

YAVE_DECL_LOCAL_LOGGER(execute_thread);

namespace yave::editor {

  /// internal task thread for execution
  class execute_thread::impl
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

  private:
    time arg_time;

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
    impl(data_context& dctx)
      : data_ctx {dctx}
    {
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

        log_error("Failed to execute frame output: {}", msg);

        // print additional info

        if (auto err = value_cast_if<BadValueCast>(erro)) {
          log_error(
            "  BadValueCast: from:{}, to:{}",
            to_string(err->from),
            to_string(err->to));
        }

        if (auto err = value_cast_if<TypeError>(erro)) {
          log_error(
            "  TypeError: type:{}, t1:{}, t2:{}",
            err->error_type,
            to_string(err->t1),
            to_string(err->t2));
        }

        if (auto err = value_cast_if<ResultError>(erro)) {
          log_error("  ResultError: type:{}", err->error_type);
        }

      } catch (...) {
        log_error("unknown exception detected during execution!");
      }
      return std::nullopt;
    }

    void start()
    {
      using namespace std::chrono;
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

              auto run_bgn = steady_clock::now();

              std::optional<compiler::executable> exe;
              {
                auto lck = data_ctx.get_data<editor_data>();

                // process pending updates
                {
                  auto& update_ch = lck.ref().update_channel();
                  update_ch.apply_updates();
                }

                // get compiled result
                {
                  auto& compiler_data = lck.ref().compiler_data();
                  if (auto&& r = compiler_data.last_executable()) {
                    exe = r->clone();
                  }
                }
              }

              // no compile result
              if (!exe)
                continue;

              assert(
                same_type(exe->type(), object_type<signal<FrameBuffer>>()));

              auto bgn = high_resolution_clock::now();

              // execute app tree.
              auto img = exec_frame_output(std::move(*exe), arg_time);

              auto end = high_resolution_clock::now();

              auto run_end = steady_clock::now();

              {
                auto lck       = data_ctx.get_data<editor_data>();
                auto& executor = lck.ref().executor_data();

                executor.set_result(
                  {.arg_time     = arg_time,
                   .image        = std::move(img),
                   .compute_time = duration_cast<milliseconds>(end - bgn),
                   .begin_time   = run_bgn,
                   .end_time     = run_end});
              }
            }
          }
          log_info("Stopped executor thread");
        } catch (...) {
          log_error("Exception detected in executor thread");
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
      notify_execute(arg_time);
    }

    void notify_execute(time arg)
    {
      check_failure();
      {
        auto lck     = std::unique_lock {mtx};
        arg_time     = arg;
        execute_flag = true;
        cond.notify_one();
      }
    }
  };

  execute_thread::execute_thread(data_context& dctx)
    : m_pimpl {std::make_unique<impl>(dctx)}
  {
  }

  execute_thread::execute_thread(execute_thread&&) noexcept = default;

  execute_thread::~execute_thread() noexcept = default;

  void execute_thread::start()
  {
    m_pimpl->start();
  }

  void execute_thread::stop()
  {
    m_pimpl->stop();
  }

  void execute_thread::notify_execute()
  {
    m_pimpl->notify_execute();
  }

  void execute_thread::notify_execute(time time)
  {
    m_pimpl->notify_execute(time);
  }

  class execute_thread_data::impl
  {
    /// result image
    std::optional<yave::image> m_result;
    /// arg time
    yave::time m_arg_time;
    /// duration of run
    std::chrono::milliseconds m_exec_duration;
    /// timestamp of run
    std::chrono::steady_clock::time_point m_begin_time;
    std::chrono::steady_clock::time_point m_end_time;

  public:
    auto last_arg_time() const
    {
      return m_arg_time;
    }

    auto last_compute_time() const
    {
      return m_exec_duration;
    }

    auto last_begin_time() const
    {
      return m_begin_time;
    }

    auto last_end_time() const
    {
      return m_end_time;
    }

    auto& last_result_image()
    {
      return m_result;
    }

    void set_exec_results(result_data results)
    {
      m_arg_time      = results.arg_time;
      m_result        = std::move(results.image);
      m_exec_duration = results.compute_time;
      m_begin_time    = results.begin_time;
      m_end_time      = results.end_time;
    }
  };

  execute_thread_data::execute_thread_data()
    : m_pimpl {std::make_unique<impl>()}
  {
  }

  execute_thread_data::execute_thread_data(execute_thread_data&&) noexcept =
    default;

  execute_thread_data::~execute_thread_data() noexcept = default;

  auto execute_thread_data::last_arg_time() const -> yave::time
  {
    return m_pimpl->last_arg_time();
  }

  auto execute_thread_data::last_compute_time() const
    -> std::chrono::milliseconds
  {
    return m_pimpl->last_compute_time();
  }

  auto execute_thread_data::last_begin_time() const
    -> std::chrono::steady_clock::time_point
  {
    return m_pimpl->last_begin_time();
  }

  auto execute_thread_data::last_end_time() const
    -> std::chrono::steady_clock::time_point
  {
    return m_pimpl->last_end_time();
  }

  auto execute_thread_data::last_result_image() const
    -> const std::optional<yave::image>&
  {
    return m_pimpl->last_result_image();
  }

  void execute_thread_data::set_result(result_data results)
  {
    m_pimpl->set_exec_results(std::move(results));
  }

} // namespace yave::editor