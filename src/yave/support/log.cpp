//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/support/log.hpp>
#include <yave/config/unreachable.hpp>

#include <spdlog/sinks/sink.h>
#include <spdlog/sinks/stdout_sinks.h>

namespace yave {

  std::shared_ptr<spdlog::logger> g_logger = spdlog::stdout_logger_mt("yave");

  constexpr const char* get_loglevel_name(LogLevel level)
  {
    switch (level) {
      case LogLevel::Info:
        return "info";
      case LogLevel::Warning:
        return "warning";
      case LogLevel::Error:
        return "error";
      default:
        unreachable();
    }
  }

  std::shared_ptr<spdlog::logger> get_default_logger()
  {
    return g_logger;
  }

  std::shared_ptr<spdlog::logger> add_logger(const char* name)
  {
    if (auto logger = spdlog::get(name)) {
      Info(g_logger, "Found logger: {}", name);
      return logger;
    }

    Info(g_logger, "Added new logger: {}", name);
    return spdlog::stdout_logger_mt(name);
  }

  std::shared_ptr<spdlog::logger> get_logger(const char* name)
  {
    return spdlog::get(name);
  }

  void set_level(const std::shared_ptr<spdlog::logger>& logger, LogLevel level)
  {
    if (!logger) {
      Error(g_logger, "Could not set loglevel");
      return;
    }

    auto lvl = [&]() {
      switch (level) {
        case LogLevel::Warning:
          return spdlog::level::warn;
        case LogLevel::Error:
          return spdlog::level::err;
        default:
          return spdlog::level::info;
      }
    }();

    if (lvl != logger->level()) {
      Info(
        g_logger,
        "New log level for logger {}: {}",
        logger->name(),
        get_loglevel_name(level));
      logger->set_level(lvl);
    }
  }
} // namespace yave