//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/editor/compile_thread.hpp>
#include <yave/editor/editor_data.hpp>
#include <yave/compiler/message.hpp>
#include <yave/support/log.hpp>

#include <yave/compiler/compile.hpp>

#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>

YAVE_DECL_LOCAL_LOGGER(compile_thread)

namespace yave::editor {

  class compile_thread::impl
  {
  private:
    data_context& data_ctx;

  private:
    std::thread thread;
    std::mutex mtx;
    std::condition_variable cond;

  private:
    std::atomic<bool> terminate_flag = false;
    std::atomic<bool> recompile_flag = false;

  private:
    std::exception_ptr exception;

    void check_failure()
    {
      if (!thread.joinable()) {
        if (exception != nullptr) {
          throw compile_thread_failure(exception);
        }
      }
    }

  public:
    impl(data_context& dctx)
      : data_ctx {dctx}
    {
    }

    bool is_running() const
    {
      return thread.joinable();
    }

  public:
    void start()
    {
      check_failure();

      thread =
        std::thread([&]() {
          try {
            while (true) {

              {
                std::unique_lock lck {mtx};
                cond.wait(
                  lck, [&] { return terminate_flag || recompile_flag; });
              }

              if (terminate_flag)
                break;

              if (recompile_flag) {

                recompile_flag = false;

                // initialize compiler pipeilne
                auto init_pipeline = [&] {
                  auto lck   = data_ctx.get_data<editor_data>();
                  auto& data = lck.ref().compiler_data();

                  data.clear_results();

                  auto pipeline = compiler::init_pipeline();

                  return pipeline;
                };

                // prepare compiler input
                auto init_input = [&](auto& pipeline) {
                  auto lck   = data_ctx.get_data<editor_data>();
                  auto& data = lck.ref();

                  // clone graph
                  auto _ng   = data.node_graph().clone();
                  auto _root = _ng.node(data.root_group().id());

                  auto _os = _ng.output_sockets(_root).empty()
                               ? socket_handle()
                               : _ng.output_sockets(_root)[0];

                  auto _decls = data.node_declarations().get_map();
                  auto _defs  = data.node_definitions().get_map();

                  compiler::input(
                    pipeline,
                    std::move(_ng),
                    std::move(_os),
                    std::move(_decls),
                    std::move(_defs));
                };

                // compiler stages
                auto parse    = [](auto& p) { compiler::parse(p); };
                auto sema     = [](auto& p) { compiler::sema(p); };
                auto verify   = [](auto& p) { compiler::verify(p); };
                auto optimize = [](auto& p) { compiler::optimize(p); };

                // process compiler output
                auto process_output = [&](compiler::pipeline& pipeline) {
                  auto lck   = data_ctx.get_data<editor_data>();
                  auto& data = lck.ref().compiler_data();

                  auto& msgs =
                    pipeline.get_data<compiler::message_map>("msg_map");

                  if (pipeline.success()) {

                    log_info("Compile Success");

                    auto& exe = pipeline.get_data<compiler::executable>("exe");

                    data.set_results(
                      {.last_msg = std::move(msgs),
                       .last_exe = std::move(exe)});

                  } else {
                    log_info("Compile Failed");

                    data.set_results(
                      {.last_msg = std::move(msgs), .last_exe = {}});
                  }
                };

                auto notify_execute = [&](auto&) {
                  auto lck = data_ctx.get_data<execute_thread>();
                  lck.ref().notify_execute();
                };

                auto pipeline = init_pipeline();

                pipeline //
                  .and_then(init_input)
                  .and_then(parse)
                  .and_then(sema)
                  .and_then(verify)
                  .and_then(optimize)
                  .apply(process_output)
                  .and_then(notify_execute);
              }
            }
            log_info("Stopped compiler thread");
          } catch (...) {
            log_error("Exception detected in compile thread");
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

    void notify_compile()
    {
      check_failure();
      recompile_flag = true;
      cond.notify_one();
    }
  };

  compile_thread::compile_thread(data_context& dctx)
    : m_pimpl {std::make_unique<impl>(dctx)}
  {
  }

  compile_thread::compile_thread(compile_thread&&) noexcept = default;

  compile_thread::~compile_thread() noexcept = default;

  void compile_thread::start()
  {
    m_pimpl->start();
  }

  void compile_thread::stop()
  {
    m_pimpl->stop();
  }

  void compile_thread::notify_compile()
  {
    m_pimpl->notify_compile();
  }

  class compile_thread_data::impl
  {
    /// compile result
    compiler::message_map m_last_msg;
    /// result
    std::optional<compiler::executable> m_last_exe;

  public:
    auto& last_message() const
    {
      return m_last_msg;
    }

    auto& last_executable()
    {
      return m_last_exe;
    }

    void clear_results()
    {
      m_last_msg = {};
      m_last_exe = std::nullopt;
    }

    void set_results(compile_results results)
    {
      m_last_msg = std::move(results.last_msg);
      m_last_exe = std::move(results.last_exe);
    }
  };

  compile_thread_data::compile_thread_data()
    : m_pimpl {std::make_unique<impl>()}
  {
  }

  compile_thread_data::compile_thread_data(compile_thread_data&&) noexcept =
    default;

  compile_thread_data::~compile_thread_data() noexcept = default;

  auto compile_thread_data::last_message() const -> const compiler::message_map&
  {
    return m_pimpl->last_message();
  }

  auto compile_thread_data::last_executable() const
    -> const std::optional<compiler::executable>&
  {
    return m_pimpl->last_executable();
  }

  void compile_thread_data::set_results(compile_results results)
  {
    m_pimpl->set_results(std::move(results));
  }

  void compile_thread_data::clear_results()
  {
    m_pimpl->clear_results();
  }

} // namespace yave::editor