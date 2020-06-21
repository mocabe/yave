//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/editor/compile_thread.hpp>
#include <yave/editor/editor_data.hpp>
#include <yave/node/parser/node_parser.hpp>
#include <yave/node/compiler/node_compiler.hpp>
#include <yave/support/log.hpp>

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
    node_parser parser;
    node_compiler compiler;

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

              auto g = [&] {
                auto data_lck = data_ctx.lock();
                auto& data    = data_ctx.data();
                return data.node_graph.clone();
              }();

              // parse
              auto parsed = parser.parse(std::move(g));

              if (!parsed) {
                auto data_lck                = data_ctx.lock();
                auto& data                   = data_ctx.data();
                data.compiler.m_parse_errors = parser.get_errors();
                Info(g_logger, "Failed to parse node graph");
                continue;
              }
              Info(g_logger, "Success to parse node graph");

              // FIXME: use lock for node defs
              auto exe =
                compiler.compile(std::move(*parsed), data_ctx.data().node_defs);

              if (!exe) {
                auto data_lck                  = data_ctx.lock();
                auto& data                     = data_ctx.data();
                data.compiler.m_compile_errors = compiler.get_errors();
                Info(g_logger, "Failed to compile node graph");
                continue;
              }

              {
                Info(g_logger, "Success to compile node graph");
                auto data_lck          = data_ctx.lock();
                auto& data             = data_ctx.data();
                data.compiler.m_result = std::move(exe);
              }
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
      terminate_flag = 1;
      cond.notify_one();
      thread.join();
      Info(g_logger, "Terminated compile thread");
    }

    void notify_recompile()
    {
      Info(g_logger, "Requesting recompile");
      recompile_flag = 1;
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

} // namespace yave::editor