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

#include <yave/core/log.hpp>

#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>

YAVE_DECL_LOCAL_LOGGER(execute_thread);

namespace yave::editor {

  using namespace std::chrono;

  /// internal task thread for execution
  class execute_thread::impl
  {
  private:
    data_context& dctx;

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
    impl(data_context& dctx)
      : dctx {dctx}
    {
    }

    bool is_running()
    {
      return thread.joinable();
    }

  public:
    static auto exec_frame_output(compiler::executable&& exe, yave::time t)
      -> std::shared_ptr<const image>
    {
      try {

        auto r = value_cast<FrameBuffer>(exe.execute(t));
        // load result to host memory
        auto img =
          std::make_shared<image>(r->width(), r->height(), r->format());
        r->read_data(0, 0, r->width(), r->height(), img->data());

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
      return nullptr;
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

              auto run_bgn = steady_clock::now();

              // time argument
              auto arg_time = yave::time();
              // end of continuous exec time window
              auto end_limit = steady_clock::time_point();

              auto exe = [&]() -> std::optional<compiler::executable> {
                auto lck       = dctx.get_data<editor_data>();
                auto& updater  = lck.ref().update_channel();
                auto& compiler = lck.ref().compiler_data();
                auto& executor = lck.ref().executor_data();
                auto& scene    = lck.ref().scene_config();

                // process pending updates
                updater.apply_updates();

                // get next arg time
                arg_time = executor.arg_time();

                // handle continuous/loop execution
                if (executor.continuous_execution()) {

                  if (arg_time == executor.last_arg_time()) {

                    // advance arg time
                    auto fps = scene.frame_rate();
                    auto dt  = time::seconds(1) / fps;
                    arg_time += dt;

                    assert(time::is_compatible_rate(fps));

                    // set wait time for next run
                    end_limit =
                      executor.last_end_time()
                      + duration_cast<steady_clock::duration>(seconds(1)) / fps;
                  }

                  // loop range
                  if (executor.loop_execution()) {
                    if (arg_time < executor.loop_range_min())
                      arg_time = executor.loop_range_min();
                    if (arg_time > executor.loop_range_max())
                      arg_time = executor.loop_range_min();
                  }
                }

                // get compiled result
                if (auto&& r = compiler.last_executable()) {
                  executor.set_arg_time(arg_time);
                  return r->clone();
                }

                return std::nullopt;
              }();

              // no compile result
              if (!exe) {
                continue;
              }

              assert(
                same_type(exe->type(), object_type<signal<FrameBuffer>>()));

              // execute app tree.
              auto img = exec_frame_output(std::move(*exe), arg_time);

              auto run_end = steady_clock::now();
              auto compute_time =
                duration_cast<milliseconds>(run_end - run_bgn);

              // continuous: limit frame rate.
              // TODO: use more accurate timer, or busy loop
              if (run_end < end_limit) {
                std::this_thread::sleep_for(end_limit - run_end);
                run_end = end_limit;
              }

              {
                auto lck       = dctx.get_data<editor_data>();
                auto& executor = lck.ref().executor_data();
                auto& scene    = lck.ref().scene_config();

                executor.set_result(
                  {.arg_time     = arg_time,
                   .image        = img,
                   .compute_time = compute_time,
                   .begin_time   = run_bgn,
                   .end_time     = run_end});

                if (executor.continuous_execution()) {
                  execute_flag = true;
                }
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
      check_failure();
      execute_flag = true;
      cond.notify_one();
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

  class execute_thread_data::impl
  {
  public:
    yave::time arg_time;
    yave::time loop_range_min;
    yave::time loop_range_max;
    bool continuous_execution = false;
    bool loop_execution       = false;

    std::shared_ptr<const yave::image> last_image;
    yave::time last_arg_time;
    std::chrono::milliseconds last_compute_time;
    std::chrono::steady_clock::time_point last_begin_time;
    std::chrono::steady_clock::time_point last_end_time;
  };

  execute_thread_data::execute_thread_data()
    : m_pimpl {std::make_unique<impl>()}
  {
  }

  execute_thread_data::execute_thread_data(execute_thread_data&&) noexcept =
    default;

  execute_thread_data::~execute_thread_data() noexcept = default;

  auto execute_thread_data::arg_time() const -> yave::time
  {
    return m_pimpl->arg_time;
  }

  void execute_thread_data::set_arg_time(yave::time t)
  {
    if (time::zero() <= t) {
      m_pimpl->arg_time = t;
    }
  }

  bool execute_thread_data::continuous_execution() const
  {
    return m_pimpl->continuous_execution;
  }

  void execute_thread_data::set_continuous_execution(bool b)
  {
    m_pimpl->continuous_execution = b;
  }

  auto execute_thread_data::loop_range_min() const -> yave::time
  {
    return m_pimpl->loop_range_min;
  }

  auto execute_thread_data::loop_range_max() const -> yave::time
  {
    return m_pimpl->loop_range_max;
  }

  void execute_thread_data::set_loop_range(yave::time min, yave::time max)
  {
    if (time::zero() <= min && min <= max) {
      m_pimpl->loop_range_min = min;
      m_pimpl->loop_range_max = max;
    }
  }

  bool execute_thread_data::loop_execution() const
  {
    return m_pimpl->loop_execution;
  }

  void execute_thread_data::set_loop_execution(bool b)
  {
    m_pimpl->loop_execution = b;
  }

  auto execute_thread_data::last_arg_time() const -> yave::time
  {
    return m_pimpl->last_arg_time;
  }

  auto execute_thread_data::last_compute_time() const
    -> std::chrono::milliseconds
  {
    return m_pimpl->last_compute_time;
  }

  auto execute_thread_data::last_begin_time() const
    -> std::chrono::steady_clock::time_point
  {
    return m_pimpl->last_begin_time;
  }

  auto execute_thread_data::last_end_time() const
    -> std::chrono::steady_clock::time_point
  {
    return m_pimpl->last_end_time;
  }

  auto execute_thread_data::last_result_image() const
    -> std::shared_ptr<const yave::image>
  {
    return m_pimpl->last_image;
  }

  void execute_thread_data::set_result(result_data results)
  {
    auto& impl = *m_pimpl;

    impl.last_arg_time     = results.arg_time;
    impl.last_image        = results.image;
    impl.last_compute_time = results.compute_time;
    impl.last_begin_time   = results.begin_time;
    impl.last_end_time     = results.end_time;
  }

} // namespace yave::editor