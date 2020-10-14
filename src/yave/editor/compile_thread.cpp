//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/editor/compile_thread.hpp>
#include <yave/editor/editor_data.hpp>
#include <yave/compiler/message.hpp>
#include <yave/node/core/function.hpp>
#include <yave/support/log.hpp>

#include <yave/compiler/compile.hpp>

#include <thread>
#include <mutex>
#include <atomic>

YAVE_DECL_G_LOGGER(compile_thread)

namespace yave::editor {

  class compile_thread::impl
  {
  public:
    data_context& data_ctx;

  public:
    std::thread thread;
    std::mutex mtx;
    std::condition_variable cond;
    std::atomic<bool> terminate_flag = false;
    std::atomic<bool> recompile_flag = false;

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
      init_logger();
    }

  public:
    void start()
    {
      Info(g_logger, "Starting compile thread");

      thread = std::thread([&]() {
        try {
          while (true) {

            std::unique_lock lck {mtx};
            cond.wait(lck, [&] { return terminate_flag || recompile_flag; });

            if (terminate_flag)
              break;

            if (recompile_flag) {

              recompile_flag = false;

              // initialize compiler pipeilne
              auto init_pipeline = [&] {
                auto lck       = data_ctx.lock();
                auto& data     = lck.get_data<editor_data>();
                auto& compiler = data.compile_thread();

                compiler.clear_results();

                auto pipeline = compiler::init_pipeline();

                // thread interface is unlinked
                if (!compiler.initialized()) {
                  pipeline.get_data<compiler::message_map>("msg_map").add(
                    compiler::internal_compile_error(
                      "Compile thread is not initialized!"));
                  pipeline.set_failed();
                }

                return pipeline;
              };

              // prepare compiler input
              auto init_input = [&](auto& pipeline) {
                auto lck   = data_ctx.lock();
                auto& data = lck.get_data<editor_data>();

                // clone graph
                auto _ng   = data.node_graph().clone();
                auto _root = _ng.node(data.root_group().id());

                auto _os = _ng.output_sockets(_root).empty()
                             ? socket_handle()
                             : _ng.output_sockets(_root)[0];

                auto _defs = data.node_definitions();

                compiler::input(
                  pipeline, std::move(_ng), std::move(_os), std::move(_defs));
              };

              // compiler stages
              auto parse    = [](auto& p) { compiler::parse(p); };
              auto sema     = [](auto& p) { compiler::sema(p); };
              auto verify   = [](auto& p) { compiler::verify(p); };
              auto optimize = [](auto& p) { compiler::optimize(p); };

              // process compiler output
              auto process_output = [&](compiler::pipeline& pipeline) {
                auto lck       = data_ctx.lock();
                auto& data     = lck.get_data<editor_data>();
                auto& compiler = data.compile_thread();

                auto& msgs =
                  pipeline.get_data<compiler::message_map>("msg_map");

                compiler.m_messages = std::move(msgs);

                if (pipeline.success()) {

                  Info(g_logger, "Compile Success");

                  auto& exe = pipeline.get_data<compiler::executable>("exe");

                  compiler.m_exe = std::move(exe);

                  data.execute_thread().notify_execute();

                } else {
                  Info(g_logger, "Compile Failed");
                }
              };

              auto pipeline = init_pipeline();

              pipeline //
                .and_then(init_input)
                .and_then(parse)
                .and_then(sema)
                .and_then(verify)
                .and_then(optimize)
                .apply(process_output);
            }
          }
        } catch (...) {
          Error(g_logger, "Exception detected in compile thread");
          exception = std::current_exception();
        }
      });
    }

    auto wait_task()
    {
      return std::unique_lock(mtx);
    }

    void stop()
    {
      terminate_flag = true;
      cond.notify_one();
      thread.join();
      Info(g_logger, "Terminated compile thread");
    }

    void notify_recompile()
    {
      Info(g_logger, "Requesting recompile");
      recompile_flag = true;
      cond.notify_one();
    }
  };

  compile_thread::compile_thread(data_context& dctx)
    : m_pimpl {std::make_unique<impl>(dctx)}
  {
    m_pimpl->start();
  }

  compile_thread::~compile_thread() noexcept
  {
    m_pimpl->stop();
  }

  void compile_thread::notify_recompile()
  {
    m_pimpl->check_exception();
    m_pimpl->notify_recompile();
  }

  auto compile_thread::wait_task() -> std::unique_lock<std::mutex>
  {
    return m_pimpl->wait_task();
  }

} // namespace yave::editor