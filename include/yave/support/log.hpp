//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <spdlog/spdlog.h>

#define YAVE_DECL_G_LOGGER(NAME)                 \
  namespace {                                    \
                                                 \
    std::shared_ptr<spdlog::logger> g_logger;    \
                                                 \
    void init_logger()                           \
    {                                            \
      [[maybe_unused]] static auto init = []() { \
        g_logger = yave::add_logger(#NAME);      \
        return 1;                                \
      }();                                       \
    }                                            \
  }

namespace yave {

  /// Default logger named "yave".
  [[nodiscard]] std::shared_ptr<spdlog::logger> get_default_logger();

  /// Add new logger.
  [[nodiscard]] std::shared_ptr<spdlog::logger> add_logger(const char* name);

  /// Get logger from name.
  [[nodiscard]] std::shared_ptr<spdlog::logger> get_logger(const char* name);

  /// Log level
  enum class LogLevel
  {
    Info,
    Warning,
    Error,
  };

  /// Set loglevel
  void set_level(const std::shared_ptr<spdlog::logger>& logger, LogLevel level);

  /// Log formatted text on a logger.
  /// \param logger a logger
  /// \param level log level
  /// \param args fmtlib style format arguments
  template <class... Args>
  void Log(
    const std::shared_ptr<spdlog::logger>& logger,
    LogLevel level,
    const char* msg,
    Args&&... args)
  {
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
    logger->log(lvl, msg, std::forward<Args>(args)...);
  }

  /// Log information to a logger.
  /// \param logger a logger
  /// \param args format arguments
  template <class... Args>
  void Info(
    const std::shared_ptr<spdlog::logger>& logger,
    const char* msg,
    Args&&... args)
  {
    Log(logger, LogLevel::Info, msg, std::forward<Args>(args)...);
  }

  /// Log information to default logger.
  /// \param args format arguments
  template <class... Args>
  void Info(const char* msg, Args&&... args)
  {
    Log(get_default_logger(), LogLevel::Info, msg, std::forward<Args>(args)...);
  }

  /// Log warning to a logger.
  /// \param logger a logger
  /// \param args format arguments
  template <class... Args>
  void Warning(
    const std::shared_ptr<spdlog::logger>& logger,
    const char* msg,
    Args&&... args)
  {
    Log(logger, LogLevel::Warning, msg, std::forward<Args>(args)...);
  }

  /// Log warning to default logger.
  /// \param args format arguments
  template <class... Args>
  void Warning(const char* msg, Args&&... args)
  {
    Log(
      get_default_logger(),
      LogLevel::Warning,
      msg,
      std::forward<Args>(args)...);
  }

  /// Log error to a logger.
  /// \param logger a logger
  /// \param args format arguments
  template <class... Args>
  void Error(
    const std::shared_ptr<spdlog::logger>& logger,
    const char* msg,
    Args&&... args)
  {
    Log(logger, LogLevel::Error, msg, std::forward<Args>(args)...);
  }

  /// Log error to default logger.
  /// \param args format arguments
  template <class... Args>
  void Error(const char* msg, Args&&... args)
  {
    Log(
      get_default_logger(), LogLevel::Error, msg, std::forward<Args>(args)...);
  }
} // namespace yave