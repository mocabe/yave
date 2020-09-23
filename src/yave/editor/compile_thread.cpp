//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/editor/compile_thread.hpp>
#include <yave/editor/editor_data.hpp>
#include <yave/node/parser/node_parser.hpp>
#include <yave/node/compiler/node_compiler.hpp>
#include <yave/node/core/function.hpp>
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

              auto [g, os] = [&] {
                auto data_lck = data_ctx.lock();
                auto& data    = data_lck.get_data<editor_data>();
                // clear last result
                data.compiler.m_result = std::nullopt;

                // clone graph
                auto _ng   = data.node_graph.clone();
                auto _root = _ng.node(data.root_group.id());

                auto _os = _ng.output_sockets(_root).empty()
                             ? socket_handle()
                             : _ng.output_sockets(_root)[0];

                return std::make_tuple(std::move(_ng), _os);
              }();

              // parse
              auto parse_result = parser.parse(
                {.node_graph    = std::move(g),
                 .output_socket = os,
                 .current_group = {}});

              auto parsed_ng = parse_result.take_node_graph();
              auto defs_copy = std::optional<node_definition_store>();

              {
                auto data_lck = data_ctx.lock();
                auto& data    = data_lck.get_data<editor_data>();
                // store parse results
                data.compiler.m_parse_result   = std::move(parse_result);
                data.compiler.m_compile_errors = {};
                // build local copy of defs for compilation
                if (parsed_ng)
                  defs_copy = data.node_defs;
              }

              // parse failed
              if (!parsed_ng) {
                Info(g_logger, "Failed to parse node graph");
                continue;
              }

              Info(g_logger, "Success to parse node graph");

              // compile
              auto exe = compiler.compile(
                std::move(parsed_ng.value()), defs_copy.value());

              if (!exe) {
                auto data_lck = data_ctx.lock();
                auto& data    = data_lck.get_data<editor_data>();
                data.compiler.m_compile_errors = compiler.get_errors();
                Info(g_logger, "Failed to compile node graph");
                continue;
              }

              // result is not framebuffer
              // TODO: make proper error type
              if (!same_type(
                    exe->type(), object_type<node_closure<FrameBuffer>>())) {
                auto data_lck = data_ctx.lock();
                auto& data    = data_lck.get_data<editor_data>();

                data.compiler.m_compile_errors = {};

                auto n = data.root_group;
                auto s = data.node_graph.output_sockets(n).at(0);
                data.compiler.m_compile_errors.push_back(
                  make_error<compile_error::unexpected_error>(
                    s, "Output type is not FrameBuffer"));

                Info(g_logger, "Failed to compiler node graph");
                continue;
              }

              {
                Info(g_logger, "Success to compile node graph");
                auto data_lck = data_ctx.lock();
                auto& data    = data_lck.get_data<editor_data>();
                data.compiler.m_compile_errors = {};
                data.compiler.m_result         = std::move(exe);
                data.executor.notify_execute();
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

} // namespace yave::editor