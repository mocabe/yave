//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace yave {

  /// Default logger named "yave".
  std::shared_ptr<spdlog::logger> g_logger;

  /// Add new logger.
  std::shared_ptr<spdlog::logger> add_logger(const char* name);

  /// Get logger from name.
  std::shared_ptr<spdlog::logger> get_logger(const char* name);

  /// Log level
  enum class LogLevel
  {
    Info,
    Warning,
    Error,
  };

  /// Log formatted text on a logger.
  /// \param logger a logger
  /// \param level log level
  /// \param args fmtlib style format arguments
  template <class... Args>
  void Log(
    const std::shared_ptr<spdlog::logger>& logger,
    LogLevel level,
    Args&&... args)
  {
    spdlog::level lvl = [&]() {
      switch (level) {
      case LogLevel::Warning:
        return spdlog::level::warn;
      case LogLevel::Error:
        return spdlog::level::err;
      default:
        return spdlog::level::info;
      }
    }();
    logger->log(lvl, std::forward<Args>(args)...);
  }

  /// Log information to a logger.
  /// \param logger a logger
  /// \param args format arguments
  template <class... Args>
  void Info(const std::shared_ptr<spdlog::logger>& logger, Args&&... args)
  {
    Log(logger, LogLevel::Info, std::forward<Args>(args)...);
  }

  /// Log information to default logger.
  /// \param args format arguments
  template <class... Args>
  void Info(Args&&... args)
  {
    Info(g_logger, std::forward<Args>(args)...);
  }

  /// Log warning to a logger.
  /// \param logger a logger
  /// \param args format arguments
  template <class... Args>
  void Warning(const std::shared_ptr<spdlog::logger>& logger, Args&&... args)
  {
    Log(logger, LogLevel::Warning, std::forward<Args>(args)...);
  }

  /// Log warning to default logger.
  /// \param args format arguments
  template <class... Args>
  void Warning(Args&&... args)
  {
    Warning(g_logger, std::forward<Args>(args)...);
  }

  /// Log error to a logger.
  /// \param logger a logger
  /// \param args format arguments
  template <class... Args>
  void Error(const std::shared_ptr<spdlog::logger>& logger, Args&&... args)
  {
    Log(logger, LogLevel::Error, std::forward<Args>(args)...);
  }

  /// Log error to default logger.
  /// \param args format arguments
  template <class... Args>
  void Error(Args&&... args)
  {
    Error(g_logger, std::forward<Args>(args)...);
  }
}