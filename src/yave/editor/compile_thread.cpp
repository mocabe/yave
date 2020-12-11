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

YAVE_DECL_G_LOGGER(compile_thread)

namespace yave::editor {

  namespace {

    class compile_thread
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
      compile_thread(data_context& dctx)
        : data_ctx {dctx}
      {
        init_logger();
      }

      bool is_running() const
      {
        return thread.joinable();
      }

    public:
      void start()
      {
        check_failure();

        thread = std::thread([&]() {
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
                  auto lck       = data_ctx.get_data<editor_data>();
                  auto& data     = lck.ref();
                  auto& compiler = data.compile_thread();

                  compiler.clear_results();

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
                  auto lck       = data_ctx.get_data<editor_data>();
                  auto& data     = lck.ref();
                  auto& compiler = data.compile_thread();

                  auto& msgs =
                    pipeline.get_data<compiler::message_map>("msg_map");

                  if (pipeline.success()) {

                    Info(g_logger, "Compile Success");

                    auto& exe = pipeline.get_data<compiler::executable>("exe");

                    compiler.set_results(
                      {.messages = std::move(msgs), .exe = std::move(exe)});

                    data.execute_thread().notify_execute();

                  } else {
                    Info(g_logger, "Compile Failed");

                    compiler.set_results({.messages = std::move(msgs)});
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

      void stop()
      {
        check_failure();
        terminate_flag = true;
        cond.notify_one();
        thread.join();
      }

      void notify_recompile()
      {
        check_failure();
        recompile_flag = true;
        cond.notify_one();
      }
    };
  } // namespace

  class compile_thread_data::impl
  {
    /// thread
    compile_thread m_thread;
    /// compile result
    compiler::message_map m_messages;
    /// result
    std::optional<compiler::executable> m_exe;

  public:
    impl(data_context& dctx)
      : m_thread {dctx}
    {
    }

    bool is_thread_running() const
    {
      return m_thread.is_running();
    }

  public:
    void init()
    {
      m_thread.start();
    }

    void deinit()
    {
      m_thread.stop();
    }

    void notify_recompile()
    {
      m_thread.notify_recompile();
    }

    auto& messages() const
    {
      return m_messages;
    }

    auto& executable()
    {
      return m_exe;
    }

    void clear_results()
    {
      m_messages = {};
      m_exe      = std::nullopt;
    }

    void set_results(compile_results results)
    {
      m_messages = std::move(results.messages);
      m_exe      = std::move(results.exe);
    }
  };

  compile_thread_data::compile_thread_data(data_context& dctx)
    : m_pimpl {std::make_unique<impl>(dctx)}
  {
    assert(!m_pimpl->is_thread_running());
  }

  compile_thread_data::~compile_thread_data() noexcept
  {
    assert(!m_pimpl->is_thread_running());
  }

  void compile_thread_data::init()
  {
    m_pimpl->init();
  }

  void compile_thread_data::deinit()
  {
    m_pimpl->deinit();
  }

  void compile_thread_data::notify_recompile()
  {
    m_pimpl->notify_recompile();
  }

  auto compile_thread_data::messages() const -> const compiler::message_map&
  {
    return m_pimpl->messages();
  }

  auto compile_thread_data::executable() -> std::optional<compiler::executable>&
  {
    return m_pimpl->executable();
  }

  void compile_thread_data::clear_results()
  {
    m_pimpl->clear_results();
  }

  void compile_thread_data::set_results(compile_results results)
  {
    m_pimpl->set_results(std::move(results));
  }

} // namespace yave::editor